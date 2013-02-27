/* I'm not sure I want to bother implementing security modules.  But for
// now, I'm going to implement the functions I can do easily.
//
// This is a major conflictor in NET.h, were we to add in the actual function
// table.  This file's pretty useless as is.  I'll remove it eventually.
*/

#ifndef HLDS_LINUX_SECURITY_MODULE_HIJACK
#define HLDS_LINUX_SECURITY_MODULE_HIJACK


#include "Q_memdef.h"
#include "endianin.h"

typedef struct security_module_s { //0x18 most likely

   void * module;
   uint32 padding04;
  /* uint32 padding08; //a 0x10 byte name?
   uint32 padding0C;
   uint32 padding10;
   uint32 padding14;
 */
   char buf10[0x10];
} security_module_t;

void SV_ClearSecurityModule(security_module_t *);


#endif
