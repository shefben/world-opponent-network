/*
//
// This class deals with all the console input, as well as parameter parsing in
// the command line.
//
// Curses is desired.  This also has its own thread.
//
*/

#ifndef __MASTER__SERVER__IOS_H_
#define __MASTER__SERVER__IOS_H_

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <pthread.h>
#include <time.h>
#include <ctype.h>

#include "externs.h"
#include "WONnetwk.h"
#include "SrvrNtwk.h"
#include "banlist.h"
#include "port.h"

#include LIBCONFIG //make sure you include this AFTER port.h, which holds the define
/*** The class ***/

typedef class InputOutputStats {

private:

   /* Server stats */
   unsigned long int TotalMods;

   std::string ConfigFile;

public:
   int ProcessConfigFileInput();

    void addTotalMods() { TotalMods++; }

   InputOutputStats();
   ~InputOutputStats();

} InputOutputStats_c;
#endif
