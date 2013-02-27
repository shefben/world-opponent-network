#include "packetry.h"

/******** Constructor/Destructor ********/

PacketManagement::PacketManagement() {

   /* socket stuff */
   sizeofClientAddress = sizeof(CurrentClientSockAddress);



}
PacketManagement::~PacketManagement() {

   //DisableNetwork(); //we have to init it separately, same for destroy.
}



/******** The Only Public Function ********/

/* DESCRIPTION: ProcessInput
//
// This function is the root function in our class.  On each call it
// blocks and waits for a packet.
*/
void PacketManagement::ProcessInput() {

   /* Let's handle one packet, shall we? */

   CurrentNumberOfBytesReceived = recvfrom(socket_GLOBAL, (char *)packetbuffer, MAX_PACKET_LENGTH, 0, (struct sockaddr *)&CurrentClientSockAddress, &sizeofClientAddress);
   pthread_testcancel();

   if(CurrentNumberOfBytesReceived == SOCKET_ERROR) {
      ptrIO->inc___FailedPacketSends();
      if(WSAGetLastError() != WSAENETRESET && WSAGetLastError() != WSAECONNRESET) {
         PRINTERROR;  //CONN/NETRESET mean a message we sent failed.  Regrettable, but not uncommon, so we don't need to know about it.  CONNRESET seems to be a BUG in the Windows API though.
      }
   }
   else if(CurrentNumberOfBytesReceived > 0) {
      ptrIO->UpdateBytesReceived(CurrentNumberOfBytesReceived);
      ptrIO->inc___TotalPacketsReceived();
      packetbuffer[CurrentNumberOfBytesReceived] = '\0';
      ptrIO->OutputFileData(4, "PACKET received, size:%i\n", CurrentNumberOfBytesReceived);
      ptrIO->OutputFileDataRaw(4, packetbuffer, CurrentNumberOfBytesReceived);
      ptrIO->OutputFileData(4, "\n");
      ProcessPacket();
   }
}

/* DESCRIPTION: ProcessPacket
//
// This function makes heavy use of the variables packetbuffer and
// ClientAddress.
*/
void PacketManagement::ProcessPacket() {

#define A2M_GETMODLIST			'n'
#define A2M_SELECTMOD			'p'

   //Before we do anything, drop the packet if we don't like the sender
   if((ptrBanlist->IsSockaddrBanned(CurrentClientSockAddress))) {
      Printf("Rejected packet from blocked address (in hex) %X.", CurrentClientSockAddress.sin_addr.s_addr);
      return;
   }
   switch(packetbuffer[0]) {

   case A2M_SELECTMOD:
      ptrIO->IN_x_ProcessModRequest);
      printf("Mod request received.");
      StripOutUnwantedNullsInPacket(packetbuffer);
      IN_x_ProcessModRequest();
      break;
   case A2M_GETMODLIST:
      ptrIO->IN_x_ProcessModListRequest();
      Printf("Mod list request received.");
      IN_x_ProcessModListRequest();
      break;
   default:
     printf("Unknown packet recieved");
    }
}



/* DESCRIPTION: StripOutUnwantedNullsInPacket
//
// To be called immediately after receiving a function that would be hindered
// by nulls.  In processing, very few packets actually care about returns or
// nulls, so we can save ourselves some hassle later on by removing all but
// the last one.
//
*/
void PacketManagement::StripOutUnwantedNullsInPacket(char* packet) {

   int i = 0;
   int j = 0;

   for(; i < CurrentNumberOfBytesReceived; i++) {
      if(packet[i] == '\0' || packet[i] == '\r' || packet[i] == '\n') {
         continue;
      }
      packet[j] = tolower(packet[i]);
      j++;
   }
   packet[j] = '\0';
   CurrentNumberOfBytesReceived = j;
}

/* DESCRIPTION: GetKeyPairValue
//
// Half-Life seems to be a bit lenient in how these things can be formatted.
// Rather than force an order, it just seems to look for the key and assume
// the next string is the value.  We follow suit.  Note that there are possible
// problems here; malicious tampering isn't a huge issue.  However, keys in
// server descriptions can be if they're not placed at the end (they always
// should be).  Overall, this isn't complicated.
//
// Returns a pointer to a string.  The string contains only a null if we
// failed to find the token.  To do: make more like the next two functions.
*/
char* PacketManagement::GetKeyPairValue(const char* string, const char* value) {

   //At the moment only one thread ever uses this, so there is no need to mess
   //with making it threadable.
   static char returnValue[64];
   int i = 0;
   char* ptr;

   if(strlen(value) > 20) {
      // There are no tokens that long.  If we get one that long, just fail and
      // leave the programmer scratching his head as I'm not mallocing in this
      // function.
      returnValue[0] = '\0';
      return(returnValue);
   }

   // Surround our token with backslashes.  That's how they appear in packets,
   // and that way we won't trigger a map or something called 'version'.
   sprintf(returnValue, "\\%s\\", value);

   ptr = strstr(string, value);
   if(ptr == NULL) { returnValue[0] = '\0'; }
   else {
      // If we are here, we found this token.  The next token should be what we
      // return.  Now, not EVERY token is encased in slashes (the last isn't),
      // and returns might be desired.  Who knows.  So we only break for nulls
      // and slashes.  And remember, all input is evil.

      //First let's jump past this token we found...
      ptr += strlen(value)+1;

      //now let's go until we hit something we should break for.
      while(*ptr != '\0' && *ptr != '\\' && i < 63) {
         returnValue[i] = *ptr;
         i++;
         ptr++;
      }
      //This tacks on a null.  It also leaves us with an empty string if we
      //had someone send us a funny packet.
      returnValue[i] = '\0';
   }
   return(returnValue);

}

/* DESCRIPTION: GetNextToken
//
// Takes a string and returns tokens.  The second argument is where the copied
// tokens will be placed (instead of a static variable), the third is the max
// length.  And the fourth is what we are looking for.  The fifth is our
// packet's length.
*/
unsigned int PacketManagement::GetNextToken(const char* string, char* retval, const unsigned int length, const unsigned int packlen) {
   return(GetNextToken(string, retval, length, packlen, '\0'));
}
unsigned int PacketManagement::GetNextToken(const char* string, char* retval, const unsigned int length, const unsigned int packlen, const char delim) {

   unsigned int i = 0;

   while(string[i] != delim && i < length && i < packlen) {
      retval[i] = string[i];
      i++;
   }
   return(i);
}



/* DESCRIPTION: StartTheNetwork
//
// Copy and pasted from other programs I've done.  Does that annoying
// behind-the-scenes stuff to get the network started.
//
// Is now a public function.  We have to start the NETWORKING class before the
// INPUT class, but until we've started the input class we can't find out
// what port we should run on.
*/
int PacketManagement::StartTheNetwork() {

   //remember, we have socket_GLOBAL just waiting to be initialized
   struct sockaddr_in ServerAddress;

   DoWindowsNetThing();

   socket_GLOBAL = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   if(socket_GLOBAL == (signed)INVALID_SOCKET) {
       printf("Error invalid socket");
      DisableNetwork();
      return(254);
   }

   memset(&ServerAddress, 0, sizeof(ServerAddress));

   ServerAddress.sin_family = AF_INET;
   ServerAddress.sin_port = htons(ptrIO->GetPort()); //port=Global
   ServerAddress.sin_addr.s_addr=INADDR_ANY; //I hope this doesn't cause problems

   if(bind(socket_GLOBAL, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress))==SOCKET_ERROR) {
      printf("Cant bind to local acdress");
      DisableNetwork();
      return(253);
   }

   //Yep, for UDP, that's it.
   return(0);
}

void PacketManagement::DisableNetwork() {

   if(socket_GLOBAL != (signed)INVALID_SOCKET) {

      shutdown(socket_GLOBAL, 2);
      close(socket_GLOBAL);
      socket_GLOBAL = INVALID_SOCKET;
   }
   WSACleanup(); //We can call this repeatedly w/o issue.
}
