/* Making new random numbers.  Rand() must be too much trouble;
// Rest assured the 'bad guys' aren't thwarted.
*/

#ifndef OSHLDS_RANDOM_HEADER
#define OSHLDS_RANDOM_HEADER

#include "ModCalls.h"

void SeedRandomNumberGenerator();
HLDS_DLLEXPORT long int RandomLong(long int, long int);
HLDS_DLLEXPORT float RandomFloat(float, float);

#endif
