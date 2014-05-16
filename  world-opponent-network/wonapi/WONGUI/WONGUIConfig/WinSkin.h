#ifndef __WON_SKIN_H__
#define __WON_SKIN_H__

#include "WONGUI/RawImage.h"
#include "WONGUI/StretchImage.h"
#include "WONGUI/WONControls.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WinSkin : public RefCount
{
protected:
	std::string mFileName;
	std::string mFilePath;

	typedef std::map<int,int> ColorMap;
	typedef std::map<int,FontPtr> FontMap;

	Window *mWindow;
	ColorMap mColorMap;
	FontMap mFontMap;

	int GetColor(int theIndex);
	Font* GetFont(int theIndex);

	int GetColor(const char *theSection, const char *theKey);
	int GetColorSchemeColor(const char *theKey);
	Font* GetFont(const char *theSection, const char *theKey);

	RawImagePtr GetRawImage(const char *theSection, const char *theKey, bool checkTrans = true);
	StretchImagePtr GetStretchImage(const char *theSection, const char *theKey);
	std::string GetPathParam(const char *theSection, const char *theKey);
	std::string GetStrParam(const char *theSection, const char *theKey, const char *theDefault = "");
	int GetIntParam(const char *theSection, const char *theKey, int theDefault = 0);

	WONButtonPtr CalcButton(const char *theSection, const char *theBitmapName, WONButton *orig);
	WONButtonPtr CalcCheckButton(const char *theBitmapName, WONButton *orig);
	void CalcArrows();
	void CalcComboButton();
	void CalcColorScheme();
	void CalcTabCtrlFrame();
	void CalcSunkEdge();
	void CalcPopupBack();
	void CalcDialogBack();
	void CalcCloseButton();

	WONSkinPtr mSkin;


public:
	WinSkin();
	WONSkin* Load(const char *theFilePath);

	WONSkin* GetSkin() { return mSkin; }
};
typedef SmartPtr<WinSkin> WinSkinPtr;

};

#endif