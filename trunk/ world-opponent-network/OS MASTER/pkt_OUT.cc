#include "packetry.h"

/* Everything in this file refers to the sending of specific packets.  This
** file, pkt_IN.cc, and packetry.cc all make up the master server class.
** For better organization, the three are separated into smaller files.
*/


/* DESCRIPTION: OUT_d_SendServerList
//
// Assumed identical to OUT_serverlist_f but wothout the first few bytes
// acting as a placeholder.  UNPROVEN ASSUMPTION.
//
*/
void PacketManagement::OUT_d_SendServerList(struct sockaddr_in DestinationAddress) {

   unsigned int packetlen = 6;
   unsigned long int position = 0; //0 is the default, but we must pass by reference
   strcpy(packetbuffer, "\xff\xff\xff\xff" "o\n");
   packetlen += ptrServerList->FillPacketBufferWithIPs(&position, packetbuffer+6,  MAX_PACKET_LENGTH-6, 0, NULL);

   OUT_SendSpecificReply(packetbuffer, packetlen, DestinationAddress);
  ptrIO->inc_d_ServerListPacketsSent();
}

/* DESCRIPTION: OUT_j_SendAck
//
// Not any creativity here.  We just fire a quick, constant message out.
*/
void PacketManagement::OUT_j_SendAck(struct sockaddr_in DestinationAddress) {

   OUT_SendSpecificReply("\xff\xff\xff\xff" "j", strlen("\xff\xff\xff\xff" "j"), DestinationAddress);
  ptrIO->inc_j_ACKPacketsSent();
}


/* DESCRIPTION: OUT_y_SendModRequest
//
// Much like when we return servers, we attempt to return as many mods as we
// can in one packet, and give HL something to let it continue later.  It's
// not a hash that HL returns this time--it's a string.
//
*/
void PacketManagement::OUT_y_SendModRequest(struct sockaddr_in DestinationAddress, const char * Mod) {

   unsigned int packetlen = 6;

   //The char we were given a pointer to is packetbuffer, we need a copy.
   //Fixme: a bit roundabout, pass a hash or something instead.
   char TempArray[64];
   int offset = 0;

   //This is pretty much a strncpy that doesn't copy non-printable chars.
   while(*Mod != '\0' && offset < 63) { //gibberish bypassing

      if(isprint(*Mod) != 0) {

         TempArray[offset] = *Mod;
         offset++;
      }

      Mod++;
   }

   if(offset < 1) { return; } //Wouldn't crash us, but wastes our time.
   TempArray[offset] = '\0';

   strcpy(packetbuffer, "\xff\xff\xff\xff" "y\n");

   packetlen += ptrMods->FillPacketBufferWithMods(TempArray, packetbuffer+6,  MAX_PACKET_LENGTH-6);

   OUT_SendSpecificReply(packetbuffer, packetlen, DestinationAddress);
  ptrIO->inc_y_ModReplyPacketsSent();
}



/* DESCRIPTION: OUT_SendClientPing
//
// Sends the HL ping packet to a server.  Useful when verifying shutdowns.
*/
void PacketManagement::OUT_SendClientPing(struct sockaddr_in DestinationAddress) {

   OUT_SendSpecificReply("\xff\xff\xff\xff" "ping", strlen("\xff\xff\xff\xff" "ping"), DestinationAddress);
  ptrIO->inc___PingPacketsSent();
}




/* DESCRIPTION: OUT_SendSpecificReply
//
// Mostly just a sendto wrapper, this is public to allow other things to send--
// such as WONnetwk, which handles w packets.
*/
void PacketManagement::OUT_SendSpecificReply(char* SendingPacket, unsigned int SizeofPacket, struct sockaddr_in DestinationAddress) {
   if(sendto(socket_GLOBAL, SendingPacket, SizeofPacket, 0, (struct sockaddr *)&DestinationAddress, sizeof(DestinationAddress)) == SOCKET_ERROR) {
      PRINTERROR;
   }
   ptrIO->UpdateBytesSent(SizeofPacket);
   ptrIO->inc___TotalPacketsSent();
   ptrIO->OutputFileData(4, "PACKET sent, size:%i\n", CurrentNumberOfBytesReceived);
   ptrIO->OutputFileDataRaw(4, packetbuffer, CurrentNumberOfBytesReceived);
   ptrIO->OutputFileData(4, "\n");
}
