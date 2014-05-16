#ifndef __WON_COLORSCHEME_H__
#define __WON_COLORSCHEME_H__

#include "WONCommon/SmartPtr.h"
#include <vector>

namespace WONAPI
{

enum 
{
	COLOR_INVALID = 0xffffffff
};

enum StandardColor
{
	StandardColor_3DDarkShadow = 0,
	StandardColor_3DFace,
	StandardColor_3DHilight,
	StandardColor_3DShadow,
	StandardColor_Scrollbar,

	StandardColor_ButtonText,
	StandardColor_GrayText,
	StandardColor_Hilight,
	StandardColor_HilightText,

	StandardColor_ToolTipBack,
	StandardColor_ToolTipText,

	StandardColor_MenuBack,
	StandardColor_MenuText,

	StandardColor_Back,
	StandardColor_Text,

	StandardColor_Link,
	StandardColor_LinkDown,


	StandardColor_Max
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ColorScheme : public RefCount
{
protected:
	DWORD mColorArray[StandardColor_Max];

public:
	ColorScheme() { memset(mColorArray,0,sizeof(DWORD)*StandardColor_Max); }
	DWORD GetStandardColor(StandardColor theColorIndex) { return mColorArray[theColorIndex]; }
	void SetStandardColor(StandardColor theColorIndex, DWORD theColor) { mColorArray[theColorIndex] = theColor; }

	void CopyFrom(ColorScheme *theColorScheme) { memcpy(mColorArray,theColorScheme->mColorArray,sizeof(DWORD)*StandardColor_Max); }

	static DWORD GetColorRef(StandardColor theColor) { return 0x80000000 + theColor; }
};
typedef SmartPtr<ColorScheme> ColorSchemePtr;

}; // namespace WONAPI


#endif