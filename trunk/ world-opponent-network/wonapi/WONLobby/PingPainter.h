#ifndef __WON_PINGPAINTER_H__
#define __WON_PINGPAINTER_H__

#include "WONGUI/Align.h"

namespace WONAPI
{

class Graphics;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPaintPingBars(Graphics &g, unsigned short thePing, int theHeight, HorzAlign theHorzAlign = HorzAlign_Left, int theWidth = 0);

} // namespace WONAPI

#endif