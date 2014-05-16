#ifndef __WON_CLIPBOARD_H__
#define __WON_CLIPBOARD_H__
#include "GUISystem.h"
#include "GUIString.h"

namespace WONAPI
{
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Clipboard
{
public:
	static GUIString Get();
	static void Set(const GUIString &theStr);
};

}; // namespace WONAPI

#endif 

