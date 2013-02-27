/* Just a few functions I could siphon away to somewhere else.
*/

#ifndef OSHLDS_SV_CHALLENGE_HEADER
#define OSHLDS_SV_CHALLENGE_HEADER

#include "NET.h"

int  SV_CheckChallenge(netadr_t *, int);
void SVC_GetChallenge();
void SVC_ServiceChallenge();

#endif
