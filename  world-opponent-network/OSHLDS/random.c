#include <time.h>

#include "random.h"

// None of these are in QW.  HL needs its own 'random' number generator
// for various reasons.

static long int idum = 0; //dumb name.
void SeedRandomNumberGenerator() {

   idum = -time(NULL);

   //I can't think of a single reason why we would do this.
   if(idum > 1000) {

      idum = -idum;
   }
   else if(idum > -1000) {

      idum -= 0x153AD38; //22261048.  I don't see the significance.
   }
}

long int ran1() {


   long int i;
   long int temp;

   static long iy_272 = 0;
   static long iv_273[0x20];

   if(idum <= 0 || iy_272 == 0) {

      if(idum < 0) {
         idum = -idum;
      }
      else {
         idum = 1;
      }

      for(i = 39; i >=0; i--) {

         //God this is a mess in ASM.
         temp = idum / 0x1F31D;
         idum = ((0x41A7 * (idum - (temp * 0x1F31D))) - (0xB14 * temp));

         if(idum < 0) { idum += 0x7FFFFFFF; }
         if(temp < 32) { iv_273[temp] = idum; }
      }

      iy_272 = iv_273[0];
   }

   temp = (idum)/ 0x1F31D;
   idum = ((0x41A7 * (idum - (temp * 0x1F31D))) - (0xB14 * temp));

   if(idum < 0) { idum += 0x7FFFFFFF; }

   i = iy_272 >> 0x1A;
   iy_272 = iv_273[i];
   iv_273[i] = idum;

   return(iy_272);
}
float fran1() { //I'd want a double, personally...

   //4.656612875245796924105750827168e-10 = 1/0x7FFFFFFF
   float temp = (1.0 / 0x7FFFFFFF) * ran1();

   //Bet we could tweak this.  Maybe if(temp >= 1), try again?
   if(temp > 0.99999988) { return(0.99999988); }

   return(temp);
}

HLDS_DLLEXPORT long int RandomLong(long int arg_0_low, long int arg_4_high) {

   unsigned long int HighestModAllowed;
   unsigned long int RelativeMax;
   unsigned long int temp;

   RelativeMax = 1 + arg_4_high - arg_0_low;
   if(RelativeMax == 0 || RelativeMax-1 > 0x7FFFFFFF) { return(arg_0_low); }

   HighestModAllowed = 0x7FFFFFFF - (0x80000000 % RelativeMax);

   do {
      temp = ran1();
   } while(temp > HighestModAllowed); //To help unbias things.

   return((temp % RelativeMax) + arg_0_low);
}
HLDS_DLLEXPORT float    RandomFloat(float arg_0_low, float arg_4_high) {

   return((fran1() * (arg_4_high - arg_0_low)) + arg_0_low);
}
