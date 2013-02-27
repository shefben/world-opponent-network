#include "packetry.h"

/* DESCRIPTION: IN_x_ProcessModRequest
//
// Surprisigly similar to requesting server lists.  This one however
// requests a MOD listing.
*/
int PacketManagement::IN_x_ProcessModRequest() {

   OUT_y_SendModRequest(CurrentClientSockAddress, packetbuffer+1);
   return(0);
}
