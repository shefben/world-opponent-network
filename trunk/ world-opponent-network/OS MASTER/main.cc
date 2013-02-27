#include "externs.h"
#include "input.h"
#include "WONnetwk.h"
#include "banlist.h"
#include "packetry.h"
#include "port.h"
#include "ModList.h"

/*** Globals ***/

//Read extern.h for the reasoning behind this.
IPBanList_c * ptrBanlist;
PacketManagement_c * ptrSocketClass;
ModList_c * ptrMods;

/*** MAIN ***/

int main(int argc, char *argv[]) {

   ptrSocketClass = new PacketManagement(); // keep before WON2ServNetwk
   ptrMods = new ModList();
   ptrBanlist = new IPBanList();

   // Parse config files */
   ptrIO->ProcessConfigFileInput();

   // Parse command line
   if((argc > 1 && ptrIO->ProcessCommandlineInput(argc, argv) != 0) || ptrSocketClass->StartTheNetwork() != 0) {
    while(getchar == 27)
        return(0);
    //loop
   }
    ptrSocketClass->DisableNetwork();
   delete ptrMods;
   delete ptrBanlist;
   delete ptrIO;
   delete ptrSocketClass;

   return(0);
}
