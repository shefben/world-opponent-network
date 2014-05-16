#ifndef __WON_SELECTIONCOLOR_H__
#define __WON_SELECTIONCOLOR_H__

#include "GUISystem.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum ForegroundSelectionMode
{
	ForegroundSelectionMode_UseOriginalColor,
	ForegroundSelectionMode_InvertOriginalColor,
	ForegroundSelectionMode_UseForegroundColor
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SelectionColor
{
private:
	DWORD mBackgroundColor;
	DWORD mForegroundColor;
	ForegroundSelectionMode mMode;

public:
	SelectionColor();
	SelectionColor(DWORD theBackgroundColor, ForegroundSelectionMode theMode = ForegroundSelectionMode_InvertOriginalColor);
	SelectionColor(DWORD theBackgroundColor, DWORD theForegroundColor);

	void SetForegroundColor(DWORD theForegroundColor);
	void SetBackgroundColor(DWORD theBackgroundColor);
	void SetForegroundMode(ForegroundSelectionMode theMode) { mMode = theMode; }

	DWORD GetBackgroundColor(Graphics& g) const;
	DWORD GetForegroundColor(Graphics& g, DWORD theOriginalColor) const;
};

};	// namespace WONAPI

#endif