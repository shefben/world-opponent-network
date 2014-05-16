//----------------------------------------------------------------------------------
// DebugLog.h
//----------------------------------------------------------------------------------
#ifndef __DebugLog_H__
#define __DebugLog_H__

#ifndef __WON_GUISTRING_H__
#include "WONGUI/GUIString.h"
#endif


//----------------------------------------------------------------------------------
// Prototypes.
//----------------------------------------------------------------------------------
void DebugLog(const char* sFormat, ...);
void DebugLog(const WONAPI::GUIString& sFormat, ...);
void DeleteDebugLog(void);


#endif
