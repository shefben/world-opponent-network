#include <stdlib.h>

#include "secmod.h"

void SV_ClearSecurityModule(security_module_t * arg_0) {

   if(arg_0->module != NULL) {
      Q_Free(arg_0->module);
   }
   Q_memset(arg_0, 0, sizeof(security_module_t));
}
