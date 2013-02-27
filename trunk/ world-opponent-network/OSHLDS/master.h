/* Header file lovingly exported from the SDK.
// CRCs are annoying, and QW doesn't have them.
*/
#ifndef OSHLDS_MASTER_HEADER
#define OSHLDS_MASTER_HEADER

#include "endianin.h"

void SVC_Heartbeat();
void Master_Heartbeat();
void Master_Shutdown(int);

#endif
