#include "connectedclientclass.h"
#include "SrvrNtwk.h"
#include "RoutingServer.h"
#include "PManip.h"

#define VERSIONCHECK_PACKET     "\x05\x02\x00\x67\x00\x01\x06\x00\x00\x00" \
                                "\x00\x0D" "\x00""/" "\x00""T" "\x00""i" "\x00""t" "\x00""a" "\x00""n" "\x00""S" "\x00""e" "\x00""r" "\x00""v" "\x00""e" "\x00""r" "\x00""s" \
                                "\x00\x01" "\x00\x15" "SilencerValidVersions"
#define VERSIONCHECK_RESPONSE   "\x32\x00\x00\x00\x05\x02\x00\x03\x00\x00\x00\x80\x01\x0a\x00\x00\x01\x00\x01" \
                                "\x00\x15" "SilencerValidVersions" \
                                "\x00\x04" "0110" "\x00"

/*** Globals ***/

//We'll periodically need access to this here.  Declaring it here should
//cause its constructor to be called before main().
class ServerList ActiveGameServers;
class RoutingServer ActiveConflicts;

/*** Constructor/Destructor***/

ConnectedClient::ConnectedClient() {

   ConnectedClient(INVALID_SOCKET);
}
ConnectedClient::~ConnectedClient() {

   //Close the socket.  At first I wasn't certain a terminating thread
   //wound through the stack calling destructors, but it does.
   //If we already did this, it'll return things like notsock.  I feel it's
   //safe to ignore them since we're terminating now.
   KillThisConnection();
}

/* DESCRIPTION: IgnoreRemainingData/KillThisConnection
//
// These functions exist to more orderly terminate a connected socket.
*/
int ConnectedClient::IgnoreRemainingData() {

   char junk[1024];
   int ret;

   shutdown(TCP_Socket, SD_RECEIVE);

   do {
      ret = recv(TCP_Socket, junk, sizeof(junk), 0);
   } while(ret != 0 && ret != SOCKET_ERROR);
   return(ret);
}
void ConnectedClient::KillThisConnection() {

   char junk[1024];
   int ret;

   shutdown(TCP_Socket, SD_BOTH);
   do {
      ret = recv(TCP_Socket, junk, sizeof(junk), 0);
   } while(ret != 0 && ret != SOCKET_ERROR);

   closesocket(TCP_Socket);
}


/*** Port 15100 ***/

/* DESCRIPTION: ProcessClient15100
//
// The server on port 15100 is at an address given to the client by
// the aserver running on port 15101.  It is responsible for managing
// the lobby, and by proxy all conneted clients and open conflicts.
//
// Clients stay connected to the routing server until they create a
// game.  Unfortunately there are some instances where the connection
// between the client and routing server is lost--situations which
// seem to be unknown to the client.  The client queries the server
// periodically, but not everything the two programs can do to each
// other is understood.
//
// At present there is one major hack.  Clients don't query for a list
// of conflicts--the routing server sends to broadcast them immediately.
// This would require a MAJOR remodeling of the program right now,
// so the workaround is: since clients can't chat in the lobby,
// any chat message is inerpreted as a conflict query.
//
// It's not pretty, but at this point I'm questioning whether it ever can be.
*/
void ConnectedClient::ProcessClient15100() {

   unsigned char RecvBuffer[MAX_RECV_SIZE];
   unsigned char * ptr;

   union CharShort {
      unsigned char littleendiandata[2];
      unsigned short lengthofpacket;
   } PacketSize;

   int ret;
   unsigned int counter;

   //unlike with our 15101 server, this one handles more than just one packet.

   while(1) {

      counter = 0;
      do {
         ret = recv(TCP_Socket, (char *)PacketSize.littleendiandata + counter, 2 - counter, 0);
         printf("Received packet size %i\n", PacketSize.lengthofpacket);
         if(ret == 0 || ret == SOCKET_ERROR) {
            KillThisConnection();
            return;
         }
         counter += ret;
      } while(counter < 2);


      PacketSize.lengthofpacket = letohost16(PacketSize.lengthofpacket) - 2;
      if(PacketSize.lengthofpacket > MAX_RECV_SIZE-1 || PacketSize.lengthofpacket < 4) {
         KillThisConnection();
         return;
      }

      counter = 0;
      do {
         ret = recv(TCP_Socket, (char *)(RecvBuffer + counter), PacketSize.lengthofpacket - counter, 0);

         if(ret == 0 || ret == SOCKET_ERROR) {
            KillThisConnection();
            return;
         }
         counter += ret;
      } while(counter < PacketSize.lengthofpacket);


      //Packet IDing time.  Unlike before, we need some of the data contained within.

      /* A registration packet starts out with \x03\x02\x1f.  It then has the
      // name of the user (preceded with size of course) as an A string, and the
      // password of the user (again, prweceded w size) in a W string.  It is
      // terminated with a 0x04.
      */
      #define UserAcknowledgementPacket "\x9d\x00\x05\x02\x20\x00\x00" \
      "\x13\x00" "B e t a R o u t i n g S e r v e r 0 7 " \
      "\x32\x00" "W o n 2 .   R e v i v i n g   O l d   G a m e s .   O n e   r e v e r s i n g   a t   a   t i m e . " \
      "\x00\x00" //user ID is the last one
      #define NOMESSAGES "\x00\x00\x00\x00\x36\xff\xff\x00\x00" \
      "\x26\x00" "*The lobby doesn't support chat, mkay."
      #define USAGE "\x00\x00\x00\x00\x36\xff\xff\x00\x00" \
      "\x28\x00" "*To query for conflicts, type something."

      //identifiers:
      #define FIRST_TWO_BYTES  "\x03\x02"

      #define NEW_CONFLICT_ID  0x09
      #define ABORT_CONFLICT   0x0b
      #define CLIENTQUERY_ID   0x0f
      #define USER_TERMINATION 0x1d
      #define INIT_ID          0x1f
      #define CONFLICTQUERY_ID 0x29
      #define CHATMESSAGE_ID   0x35

      if(memcmp(RecvBuffer, FIRST_TWO_BYTES, sizeof(FIRST_TWO_BYTES)-1) != 0) {

         printf("Unknown packet from port 15100, is missing proper header:\n");
         PrintUnknownPacket(RecvBuffer, PacketSize.lengthofpacket);
         KillThisConnection();
         return;

      }

      ptr = RecvBuffer + sizeof(FIRST_TWO_BYTES)-1;
      switch(*ptr) {

      case INIT_ID:

         /* A user is connecting.  They give us a username and password, which
         // for a LAN isn't that big of a deal.  We give them a user ID, which
         // we later use to keep track of them (but don't because that's hard).
         */

         ret = ActiveConflicts.ProcessNewClient(ptr+1, PacketSize.lengthofpacket - sizeof(FIRST_TWO_BYTES));
         if(ret < 0) {

            printf("Client connected, but the packet was bad\n");
            KillThisConnection();
            return;
         }

         memcpy(RecvBuffer, UserAcknowledgementPacket, sizeof(UserAcknowledgementPacket)-3);
         (* (UINT16 *)(RecvBuffer + sizeof(UserAcknowledgementPacket) - 3)) = (UINT16)ret;
         send(TCP_Socket,  (char *)RecvBuffer, sizeof(UserAcknowledgementPacket), 0);
         sleep(2);
         send(TCP_Socket, USAGE, sizeof(USAGE)-1, 0);
         break;

      case CHATMESSAGE_ID:
         printf("User tried to chat\n");
         ret = ActiveConflicts.GetConflictPacket(&ptr);
         send(TCP_Socket, (char *)ptr, ret, 0);
         break;

      case USER_TERMINATION:
         printf("A user indicated (I believe) that he was ending communication with the router.\n");
       //  KillThisConnection();
       //  return;
         break;

      case CLIENTQUERY_ID:
         printf("A user requested a client listing.  Ignoring...\n");
         break;

      case NEW_CONFLICT_ID:
         ActiveConflicts.ProcessNewConflict(ptr+1, PacketSize.lengthofpacket - sizeof(FIRST_TWO_BYTES));
         printf("New conflict registered.\n");
         //DO NOT BREAK
      case CONFLICTQUERY_ID:

         ret = ActiveConflicts.GetConflictPacket(&ptr);
         send(TCP_Socket, (char *)ptr, ret, 0);
         printf("Conflict listing sent.\n");
         break;

      default:
         printf("Unknown packet from port 15100:\n");
         PrintUnknownPacket(RecvBuffer, PacketSize.lengthofpacket);
      }
   }
}


/*** Port 15101 ***/

/* DESCRIPTION: ProcessClient15101
//
// The server on port 15101 is at a hardcoded (and hard to change) address in
// Silencer.  The server is queried in five different instances:
//
// When a client wants to verify its version is current
// When a client wants to know the address of the auth server
// When a client wants to know the address of the routing server
// When a client wants to know the address of a game server
// When a game server wants to register itself
//
// Each interaction is a one packet in, maybe one out deal.
*/
void ConnectedClient::ProcessClient15101() {

   unsigned char RecvBuffer[MAX_RECV_SIZE];
   unsigned char * ptr;

   unsigned int counter = 0;
   int ret;

   union CharInt {
      unsigned char littleendiandata[4];
      unsigned int lengthofpacket;
   } PacketSize;

   //Get the first four bytes that tell us the SIZE.
   do {
      ret = recv(TCP_Socket, (char *)PacketSize.littleendiandata + counter, 4 - counter, 0);
      printf("Received packet size %i\n", PacketSize.lengthofpacket);
      if(ret == 0 || ret == SOCKET_ERROR) {
         KillThisConnection();
         return;
      }
      counter += ret;
   } while(counter < 4);

   //Quick sanity check on the sizes
   PacketSize.lengthofpacket = letohost32(PacketSize.lengthofpacket) - 4;
   if(PacketSize.lengthofpacket > MAX_RECV_SIZE-1 || PacketSize.lengthofpacket < 1) {
      KillThisConnection();
      return;
   }
   counter = 0;

   //Get the rest of this packet
   do {
      ret = recv(TCP_Socket, (char *)(RecvBuffer + counter), PacketSize.lengthofpacket - counter, 0);

      if(ret == 0 || ret == SOCKET_ERROR) {
         KillThisConnection();
         return;
      }
      counter += ret;
   } while(counter < PacketSize.lengthofpacket);

   //A complete packet (minus the four byte 'header' that listed the size) is
   //a waiting to be processed.

   ret = IdentifyPacket15101(RecvBuffer, PacketSize.lengthofpacket);

   switch(ret) {

   case 0:

      ActiveGameServers.ProcessHeartbeat(RecvBuffer, PacketSize.lengthofpacket);
      printf("Processed heartbeat\n");
      break;
   case 1:

      send(TCP_Socket, VERSIONCHECK_RESPONSE, sizeof(VERSIONCHECK_RESPONSE), 0);
      printf("Sent version info\n");
      break;
   case 2:

      ret = ActiveGameServers.GetAuthPacket(&ptr);
      send(TCP_Socket, (char *)ptr, ret, 0);
      printf("Sent auth server info\n");
      break;
   case 3:

      ret = ActiveGameServers.GetRoutingPacket(&ptr);
      send(TCP_Socket, (char *)ptr, ret, 0);
      printf("Sent routing server info\n");
      break;
   case 4:

      ret = BuildGameServerPacket(RecvBuffer);
      send(TCP_Socket, (char *)RecvBuffer, ret, 0);
      printf("Sent list of game servers\n");
      break;
   default:

      printf("Unknown packet from port 15101:\n");
      PrintUnknownPacket(RecvBuffer, PacketSize.lengthofpacket);
      break;
   }

   sleep(1);
   shutdown(TCP_Socket, SD_SEND);
   IgnoreRemainingData();
}

/* DESCRIPTION: IdentifyPacket15101
//
// Returns a value identifier for the five cases listed in ProcessClient15101.
// I don't really like enums, so I'll just use numbers:
//
// 0 = heartbeat
// 1 = version check
// 2 = auth req
// 3 = routing req
// 4 = game req
// anything else = unknown
//
*/
#define GAMESERVERREQ_PACKET_HEADER "\x05\x02\x00\x67\x00\x32\x0e\x00\x06\x00\x00"
#define GAMESERVER_REQ_TEXT "\x15\x00/\x00S\x00i\x00l\x00e\x00n\x00c\x00e\x00r\x00/\x00G\x00a\x00m\x00e\x00S\x00e\x00r\x00v\x00e\x00r\x00s\x00"
#define GAMESERVERREQ_PACKET_FOOTER "\x01\x00\x01\x00
int IdentifyPacket15101(const unsigned char * ptr, unsigned int len) {

   //The smallest packet we handle here is 31 bytes long.  Anything less than that we might as well ignore so that we don't have to do length checks everywhere.
   //counter is being reused as a boolean.  0 = identified (0 means we don't print it)
   if(len > 30) {

      ExtractByte(&ptr, &len);
      if(len == 5) {
         ExtractWord(&ptr, &len);
         if(len == 2) {
            ExtractWord(&ptr, &len);
            if(len == 0x66) {
               ExtractDWord(&ptr, &len);
               if(len == 0x0600001B) {
                  ExtractWord(&ptr, &len);
                  if(len == 0) {
                     //We could extract the strings, but why bother.
                     ExtractWord(&ptr, &len);
                     if(len == 18) {
                        return(2);
                     }
                     else if(len == 9) {
                        return(3);
                     }
                  }
               }
            }
            else if(len == 0x67) {
               if(ptr[0] == 0x01) { //ver request
                  return(1);
               }
               else if(ptr[0] == 0x32) { //game ser request
                  return(4);
               }
            }
            else if(len == 0xCB) { //heartbeat.
               return(0);
            }
         }
      }
   }
   return(-1);
}


/* DESCRIPTION: BuildGameServerPacket
//
// Takes a buffer and assumes a max length of
// MAX_SEND_SIZE and attempts to list all of the game servers we know
// of inside.  It will return the length of the finished packet.
//
// This is one of those functions that's a bit too sloppy and needs
// a little work.
*/
#define GAMESERVER_PACKET_HEADER "\x45\x52\x60\x03\x00\x90\xA0\xB0\xC1\xDA\xE0\xF0"
int BuildGameServerPacket(unsigned char * buffer) {

   unsigned char * ptr;
   unsigned int length;

   ptr = buffer + 4; //We do the size last
   length = 4;


   //First we should copy our header in.  Most of it is meaningless, but the three is not.
   WriteAndPassStringA(&ptr, GAMESERVER_PACKET_HEADER, sizeof(GAMESERVER_PACKET_HEADER)-1);
   length += sizeof(GAMESERVER_PACKET_HEADER)-1;

   //Now we pass it on to the Serverlist class to fill in the holes.
   length += ActiveGameServers.FillPacketBufferWithGameServers(ptr, MAX_SEND_SIZE-length);
   WriteDWord(buffer, length);

   return(length);
}
