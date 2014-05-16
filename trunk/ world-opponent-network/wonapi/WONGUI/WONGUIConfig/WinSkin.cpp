#include "WinSkin.h"
#include "WONCommon/WONFile.h"
#include "WONGUI/ImageFilter.h"
#include "WONGUI/StretchImage.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/ImageButton.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WinSkin::WinSkin()
{
	mSkin = new WONMSSkin;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string WinSkin::GetStrParam(const char *theSection, const char *theKey, const char *theDefault)
{
	char aStr[512];
	GetPrivateProfileString(theSection,theKey,theDefault,aStr,512,mFileName.c_str());
	return aStr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string WinSkin::GetPathParam(const char *theSection, const char *theKey)
{
	char aStr[512];
	GetPrivateProfileString(theSection,theKey,"",aStr,512,mFileName.c_str());
	return mFilePath + aStr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int WinSkin::GetIntParam(const char *theSection, const char *theKey, int theDefault)
{
	char aStr[512];
	GetPrivateProfileString(theSection,theKey,"",aStr,512,mFileName.c_str());
	if(aStr[0]=='\0')
		return theDefault;
	else
		return atoi(aStr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int WinSkin::GetColor(int theIndex)
{
	ColorMap::iterator anItr = mColorMap.insert(ColorMap::value_type(theIndex,-1)).first;
	if(anItr->second==-1)
	{
		char aSection[50];
		sprintf(aSection,"Colour%d",theIndex);
		int r = GetIntParam(aSection,"R");
		int g = GetIntParam(aSection,"G");
		int b = GetIntParam(aSection,"B");
		anItr->second = ((r&0xff)<<16) + ((g&0xff)<<8) + (b&0xff);
	}

	return anItr->second;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Font* WinSkin::GetFont(int theIndex)
{
	if(theIndex==255)
		return mWindow->GetWindowManager()->GetNamedFont("MSButton");

	FontMap::iterator anItr = mFontMap.insert(FontMap::value_type(theIndex,NULL)).first;
	if(anItr->second==NULL)
	{
		char aSection[50];
		sprintf(aSection,"Font%d",theIndex);

		string aName = GetStrParam(aSection,"FontName");
		int aHeight = GetIntParam(aSection,"FontHeight")*72/96;
		int aWeight = GetIntParam(aSection,"FontWeight");
		bool italics = GetIntParam(aSection,"Italics")?true:false;
		bool underline = GetIntParam(aSection,"Underline")?true:false;
		bool antialias = GetIntParam(aSection,"AntiAlias")?true:false;

		int aStyle = 0;
		if(italics) aStyle |= FontStyle_Italic;
		if(underline) aStyle |= FontStyle_Underline;
		if(aWeight>=600) aStyle |= FontStyle_Bold;

		anItr->second = mWindow->GetFont(FontDescriptor(aName,aStyle,aHeight));
	}

	return anItr->second;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int WinSkin::GetColorSchemeColor(const char *theKey)
{
	char aStr[512];
	GetPrivateProfileString("Colours",theKey,"",aStr,512,mFileName.c_str());
	int r,g,b;
	sscanf(aStr,"%d%d%d",&r,&g,&b);

	return ((r&0xff)<<16) | ((g&0xff)<<8) | (b&0xff);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int WinSkin::GetColor(const char *theSection, const char *theKey)
{
	int anIndex = GetIntParam(theSection,theKey,-1);
	if(anIndex<0)
		return -1;
	else
		return GetColor(anIndex);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Font* WinSkin::GetFont(const char *theSection, const char *theKey)
{
	int anIndex = GetIntParam(theSection,theKey,-1);
	if(anIndex<0)
		return NULL;
	else
		return GetFont(anIndex);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr WinSkin::GetRawImage(const char *theSection, const char *theKey, bool checkTrans)
{
	RawImagePtr anImage = mWindow->DecodeImageRaw(GetPathParam(theSection,theKey).c_str());
	if(anImage.get()==NULL)
		return NULL;

	if(!checkTrans)
		return anImage;

	int trans = GetIntParam(theSection,"trans");
	if(!trans)
		return anImage;

	TransparentImageFilterPtr aTrans;
	if(trans==2) 
		aTrans = new TransparentImageFilter(TransparentImageFilter::UpperLeft);
	else 
		aTrans = new TransparentImageFilter(0xff00ff);

	anImage = aTrans->Filter(anImage);
	return anImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WinSkin::CalcColorScheme()
{
	ColorSchemePtr aColorScheme = WindowManager::GetDefaultWindowManager()->GetColorScheme();
	
	aColorScheme->SetStandardColor(StandardColor_Scrollbar,GetColorSchemeColor("Scrollbar"));
	aColorScheme->SetStandardColor(StandardColor_Back,GetColorSchemeColor("Window"));
	aColorScheme->SetStandardColor(StandardColor_Text,GetColorSchemeColor("WindowText"));
	aColorScheme->SetStandardColor(StandardColor_MenuBack,GetColorSchemeColor("Menu"));
	aColorScheme->SetStandardColor(StandardColor_MenuText,GetColorSchemeColor("MenuText"));
	aColorScheme->SetStandardColor(StandardColor_Hilight,GetColorSchemeColor("Hilight"));
	aColorScheme->SetStandardColor(StandardColor_HilightText,GetColorSchemeColor("HilightText"));
	aColorScheme->SetStandardColor(StandardColor_3DFace,GetColorSchemeColor("ButtonFace"));
	aColorScheme->SetStandardColor(StandardColor_3DShadow,GetColorSchemeColor("ButtonShadow"));
	aColorScheme->SetStandardColor(StandardColor_GrayText,GetColorSchemeColor("GrayText"));
	aColorScheme->SetStandardColor(StandardColor_ButtonText,GetColorSchemeColor("ButtonText"));
	aColorScheme->SetStandardColor(StandardColor_3DHilight,GetColorSchemeColor("ButtonHilight"));
	aColorScheme->SetStandardColor(StandardColor_3DDarkShadow,GetColorSchemeColor("ButtonDkShadow"));
	aColorScheme->SetStandardColor(StandardColor_ToolTipText,GetColorSchemeColor("InfoText"));
	aColorScheme->SetStandardColor(StandardColor_ToolTipBack,GetColorSchemeColor("InfoWindow"));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONButtonPtr WinSkin::CalcButton(const char *theSection, const char *theBitmapName, WONButton *orig)
{
	RawImagePtr aButtonMasterImage = GetRawImage(theSection,theBitmapName);
	if(aButtonMasterImage.get()==NULL)
		return orig;

	bool isTab = false;
	bool isHeader = false;

	if(stricmp(theSection,"HeaderBar")==0)
	{
		isHeader = true;
		mSkin->mListCtrl->SetHeaderHeight(aButtonMasterImage->GetHeight());
	}
	else if(stricmp(theSection,"Tabs")==0)
	{
		mSkin->mTabCtrl->SetTabBarHeight(aButtonMasterImage->GetHeight());
		isTab = true;
	}

	WONButtonPtr aButton;
	if(isHeader)
		aButton = new WONHeaderButton;
	else
		aButton = new WONButton;

	int leftWidth = GetIntParam(theSection,"LeftWidth");
	int topHeight = GetIntParam(theSection,"TopHeight");
	int rightWidth = GetIntParam(theSection,"RightWidth");
	int bottomHeight = GetIntParam(theSection,"BottomHeight");
	int tile = GetIntParam(theSection,"Tile");
	bool mouseOver = GetIntParam(theSection,"MouseOver")?true:false;
	aButton->SetButtonFlags(ButtonFlag_InvalidateOnMouseOver,mouseOver);
//	aButton->SetTextPaddingAtLeast(leftWidth+2,topHeight+2,rightWidth+2,bottomHeight+2);

	int aNumFrames = GetIntParam(theSection,"FrameCount");
	if(aNumFrames<5)
		aNumFrames = 5;

	int aWidth = aButtonMasterImage->GetWidth()/aNumFrames;

	const char *colornames[] = { "NormalColour", "PressedColour", "DisabledColour", "FocusColour", "DefaultColour" };
	const char *fontnames[] = { "NormalFont", "PressedFont", "DisabledFont", "FocusFont", "DefaultFont" };

	CropImageFilterPtr aCrop = new CropImageFilter;
	aCrop->SetSize(aWidth,aButtonMasterImage->GetHeight());
	for(int i=0; i<5; i++)
	{
		StretchImagePtr aStretchImage = NULL;
		if(i<5)
		{
			aCrop->SetPos(i*aWidth,0);
			RawImagePtr aRawImage = aCrop->Filter(aButtonMasterImage);

			aStretchImage = new StretchImage(aRawImage,leftWidth,topHeight,rightWidth,bottomHeight);
			switch(tile)
			{
				case 0: aStretchImage->SetTile(false,false); break;
				case 1: aStretchImage->SetTile(true,true); break;
				case 2: aStretchImage->SetTile(true,false); break;
				case 3: aStretchImage->SetTile(false,true); break;
			}
		}

		int aColor = GetColor(theSection,colornames[i]);
		FontPtr aFont = GetFont(theSection,fontnames[i]);
		if(i==0)
		{
			if(aFont.get()==NULL)
				aFont = mWindow->GetWindowManager()->GetNamedFont("MSButton");
			if(aColor==-1)
				aColor = 0;
		}

		if(i==1 && isTab)
		{
			aButton->SetHasCheck(true);
			aButton->SetButtonState(0,aStretchImage,aColor,aFont,true);
		}
		else
			aButton->SetButtonState(i,aStretchImage,aColor,aFont);
	}

	aButton->SetTransparent(aButtonMasterImage->HasTransparency());
	return aButton;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONButtonPtr WinSkin::CalcCheckButton(const char *theBitmapName, WONButton *orig)
{
	WONButtonPtr aCheckbox = new WONButton;
	aCheckbox->SetPushOffsets(0,0);
	aCheckbox->SetHasCheck(true);
	aCheckbox->SetScaleImage(false);

	RawImagePtr aButtonMasterImage = GetRawImage("Buttons",theBitmapName);
	int aNumPictures = GetIntParam("Buttons","EnhancedMode")?5:4;
	if(aButtonMasterImage.get()==NULL)
		return orig;

	int aWidth = aButtonMasterImage->GetWidth()/aNumPictures;
	aCheckbox->SetTextPadding(aWidth + 4,2,2,2);

	int aColor = GetColor("Buttons","NormalColour");
	FontPtr aFont = GetFont("Buttons","NormalFont");
	if(aFont.get()==NULL)
		aFont = mWindow->GetWindowManager()->GetNamedFont("MSButton");
	if(aColor==-1)
		aColor = 0;

	CropImageFilterPtr aCrop = new CropImageFilter;
	aCrop->SetSize(aWidth,aButtonMasterImage->GetHeight());
	for(int i=0; i<4; i++)
	{
		aCrop->SetPos(i*aWidth,0);
		RawImagePtr aRawImage = aCrop->Filter(aButtonMasterImage);
		NativeImagePtr aNativeImage = aRawImage->GetNative(mWindow->GetDisplayContext());

		bool checked = i&1?true:false;
		int anIndex = i>=2?2:0;

		aCheckbox->SetButtonState(anIndex,aNativeImage,aColor,aFont,checked);
		if(anIndex==0)
		{
			RawImagePtr aRawImage2 = (RawImage*)aRawImage->Duplicate().get();
			TintImageFilterPtr aTint = new TintImageFilter(100,100,150);
			aTint->Filter(aRawImage);
			aNativeImage = aRawImage->GetNative(mWindow->GetDisplayContext());

			aCheckbox->SetButtonState(3,aNativeImage,aColor,aFont,checked);

			aTint = new TintImageFilter(100,150,100);
			aTint->Filter(aRawImage2);
			aNativeImage = aRawImage2->GetNative(mWindow->GetDisplayContext());

			aCheckbox->SetButtonState(1,aNativeImage,aColor,aFont,checked);
		}
	}
	return aCheckbox;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WinSkin::CalcArrows()
{

	RawImagePtr aScrollMasterImage = GetRawImage("Scrollbar","Image");
	if(aScrollMasterImage.get()==NULL)
		return;

	int aWidth = aScrollMasterImage->GetWidth()/23;
	bool mouseOver = GetIntParam("Scrollbar","MouseOver")?true:false;

	CropImageFilterPtr aCrop = new CropImageFilter;
	aCrop->SetSize(aWidth,aScrollMasterImage->GetHeight());

	int i,j;

	int curX = 0;
	for(i=0; i<4; i++)
	{
		Direction aDirection;
		WONSimpleButtonPtr aButton = new WONSimpleButton;
		switch(i)
		{
			case 0: aDirection = Direction_Left; break;
			case 1: aDirection = Direction_Right; break;
			case 2: aDirection = Direction_Up; break;
			case 3: aDirection = Direction_Down; break;
		}

		for(j=0; j<4; j++)
		{
			aCrop->SetPos(curX,0);
			curX += aWidth;
			RawImagePtr aRawImage = aCrop->Filter(aScrollMasterImage);
			NativeImagePtr aNativeImage = aRawImage->GetNative(mWindow->GetDisplayContext());
			aButton->SetImage(j,aNativeImage);
		}
		
		mSkin->mArrowButton[aDirection] = aButton;
	}

	const char *thumbName[] = { "HorzScrollThumb","VertScrollThumb","HorzScroll","VertScroll" };
	for(i=0; i<4; i++)
	{
		RawImagePtr aThumbMasterImage = GetRawImage(thumbName[i],"Image");
		int leftWidth = GetIntParam(thumbName[i],"LeftWidth");
		int topHeight = GetIntParam(thumbName[i],"TopHeight");
		int rightWidth = GetIntParam(thumbName[i],"RightWidth");
		int bottomHeight = GetIntParam(thumbName[i],"BottomHeight");
		int tile = GetIntParam(thumbName[i],"tile");
		bool showDot = GetIntParam(thumbName[i],"ShowDot")?true:false;
		int aFrameCount = GetIntParam(thumbName[i],"FrameCount");
		int aNumImages = i<2?3:4;
		if(aFrameCount<aNumImages)
			aFrameCount = aNumImages;

		int aThumbWidth = aThumbMasterImage->GetWidth()/aFrameCount;
		CropImageFilterPtr aThumbCrop = new CropImageFilter;
		aThumbCrop->SetSize(aThumbWidth,aThumbMasterImage->GetHeight());
		
		WONSimpleButtonPtr aThumb;
		WONSimpleButtonPtr aGutter;
		
		if(i<2)
		{
			aThumb = new WONScrollThumb;
			aThumb->SetScaleImage(true);
		}
		else
		{
			aGutter = new WONSimpleButton;
			aGutter->SetScaleImage(true);
		}


		for(j=0; j<aNumImages; j++)
		{
			aThumbCrop->SetPos(j*aThumbWidth,0);
			RawImagePtr aRawImage = aThumbCrop->Filter(aThumbMasterImage);
			StretchImagePtr aStretchImage = new StretchImage(aRawImage,leftWidth,topHeight,rightWidth,bottomHeight);
			switch(tile)
			{
				case 0: aStretchImage->SetTile(false,false); break;
				case 1: aStretchImage->SetTile(true,true); break;
				case 2: aStretchImage->SetTile(true,false); break;
				case 3: aStretchImage->SetTile(false,true); break;
			}

			if(j!=aNumImages-1 || mouseOver)
			{
				if(i<2)
					;//aThumb->SetThumb(j,aStretchImage,false);
				else
					aGutter->SetImage(j,aStretchImage);
			}
			
			if(i<2)
			{
				aCrop->SetPos(curX,0);
				curX += aWidth;
				aRawImage = aCrop->Filter(aScrollMasterImage);

				if(j!=2 || mouseOver)
				{
					int index = j; if(index==2) index=3;
					if(showDot)
						aThumb->SetImage(index,new StretchCenterImage(aStretchImage,aRawImage->GetNative(mWindow->GetDisplayContext())));
					else
						aThumb->SetImage(index,aStretchImage);
				}
			}

			if(i==2 && j==0) // corner piece
			{
				aCrop->SetPos(curX,0);
				aRawImage = aCrop->Filter(aScrollMasterImage);
				mSkin->mScrollContainer->SetCorner(aRawImage->GetNative(mWindow->GetDisplayContext()));
			}
		}

		if(i<2)
			mSkin->mScrollThumb[i] = aThumb;
		else
			mSkin->mScrollGutter[i-2] = aGutter;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WinSkin::CalcComboButton()
{
	RawImagePtr aMasterImage = GetRawImage("ComboButton","Image");
	RawImagePtr anArrowImage = GetRawImage("ExtraImages","Image");
	if(aMasterImage.get()==NULL)
		return;

	int leftWidth = GetIntParam("ComboButton","LeftWidth");
	int topHeight = GetIntParam("ComboButton","TopHeight");
	int rightWidth = GetIntParam("ComboButton","RightWidth");
	int bottomHeight = GetIntParam("ComboButton","BottomHeight");
	int tile = GetIntParam("ComboButton","tile");
	bool mouseOver = GetIntParam("ComboButton","MouseOver")?true:false;

	CropImageFilterPtr aCrop = new CropImageFilter;
	int aWidth = aMasterImage->GetWidth()/4;
	aCrop->SetSize(aWidth,aMasterImage->GetHeight());
	
	WONSimpleButtonPtr aButton = new WONSimpleButton;
	aButton->SetScaleImage(true);

	int curX = 0;
	for(int i=0; i<4; i++)
	{
		aCrop->SetPos(curX,0);
		curX += aWidth;

		RawImagePtr aRawImage = aCrop->Filter(aMasterImage);
		StretchImagePtr aStretchImage = new StretchImage(aRawImage,leftWidth,topHeight,rightWidth,bottomHeight);
		switch(tile)
		{
			case 0: aStretchImage->SetTile(false,false); break;
			case 1: aStretchImage->SetTile(true,true); break;
			case 2: aStretchImage->SetTile(true,false); break;
			case 3: aStretchImage->SetTile(false,true); break;
		}
		ImagePtr anImage;


		if(i!=3 || mouseOver)
		{
			if(anArrowImage.get()!=NULL)
			{
				CropImageFilterPtr aCrop = new CropImageFilter;
				aCrop->SetSize(anArrowImage->GetWidth()/5,anArrowImage->GetHeight());
				aCrop->SetPos(anArrowImage->GetWidth()*i/5,0);
				anImage = new StretchCenterImage(aStretchImage,aCrop->Filter(anArrowImage)->GetNative(mWindow->GetDisplayContext()));
			}
			else
				anImage = aStretchImage;

			anImage->SetSize(aWidth,aMasterImage->GetHeight());
			aButton->SetImage(i,anImage);
		}
	}
	mSkin->mComboButton = aButton;
	mSkin->mComboBox->SetButtonInFrame(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WinSkin::CalcTabCtrlFrame()
{
	RawImagePtr aRawImage = GetRawImage("Tabs","Border",false);
	if(aRawImage.get()==NULL)
		return;

	int leftWidth = GetIntParam("Tabs","LeftWidth");
	int topHeight = GetIntParam("Tabs","TopHeight");
	int rightWidth = GetIntParam("Tabs","RightWidth");
	int bottomHeight = GetIntParam("Tabs","BottomHeight");
	int tile = GetIntParam("Tabs","tile");
	int tabBarOverlap = GetIntParam("Tabs","TabBarOverlap");
	if(tabBarOverlap==0)
		tabBarOverlap = 3;


	if(leftWidth + rightWidth >= aRawImage->GetWidth())
		return;
	if(topHeight + bottomHeight >= aRawImage->GetHeight())
		return;

	StretchImagePtr aStretchImage = new StretchImage(aRawImage,leftWidth,topHeight,rightWidth,bottomHeight);
	switch(tile)
	{
		case 0: aStretchImage->SetTile(false,false); break;
		case 1: aStretchImage->SetTile(true,true); break;
		case 2: aStretchImage->SetTile(true,false); break;
		case 3: aStretchImage->SetTile(false,true); break;
	}

	aStretchImage->SetSkipCenter(true);

	mSkin->mTabCtrl->SetTabOverlap(0);
	mSkin->mTabCtrl->SetTabVertOffset(0);
	mSkin->mTabCtrl->SetTabHorzOffset(0);
	mSkin->mTabCtrl->SetTabBarOverlap(tabBarOverlap);

	mSkin->mTabCtrl->SetFrame(aStretchImage.get());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WinSkin::CalcSunkEdge()
{
	RawImagePtr aMasterImage = GetRawImage("SunkEdge","Image",false);
	if(aMasterImage.get()==NULL)
		return;

	int leftWidth = GetIntParam("SunkEdge","LeftWidth");
	int topHeight = GetIntParam("SunkEdge","TopHeight");
	int rightWidth = GetIntParam("SunkEdge","RightWidth");
	int bottomHeight = GetIntParam("SunkEdge","BottomHeight");
	int tile = GetIntParam("SunkEdge","tile");
	bool mouseOver = GetIntParam("SunkEdge","MouseOver")?true:false;

	CropImageFilterPtr aCrop = new CropImageFilter;
	int aWidth = aMasterImage->GetWidth()/4;
	aCrop->SetSize(aWidth,aMasterImage->GetHeight());
	
	int curX = 0;
	for(int i=0; i<4; i++)
	{
		aCrop->SetPos(curX,0);
		curX += aWidth;

		RawImagePtr aRawImage = aCrop->Filter(aMasterImage);
		StretchImagePtr aStretchImage = new StretchImage(aRawImage,leftWidth,topHeight,rightWidth,bottomHeight);
		switch(tile)
		{
			case 0: aStretchImage->SetTile(false,false); break;
			case 1: aStretchImage->SetTile(true,true); break;
			case 2: aStretchImage->SetTile(true,false); break;
			case 3: aStretchImage->SetTile(false,true); break;
		}

		aStretchImage->SetSkipCenter(true);
		if(i==0)
		{
			mSkin->mComboBox->SetFrame(aStretchImage.get());
//			mSkin->mInputBox->SetFrame(aStretchImage.get());
//			mSkin->mScrollContainer->SetFrame(aStretchImage.get());
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WinSkin::CalcPopupBack()
{
	RawImagePtr aRawImage = GetRawImage("MenuBackground","Image");
	if(aRawImage.get()==NULL)
		return;

	int leftWidth = GetIntParam("MenuBackground","LeftWidth");
	int topHeight = GetIntParam("MenuBackground","TopHeight");
	int rightWidth = GetIntParam("MenuBackground","RightWidth");
	int bottomHeight = GetIntParam("MenuBackground","BottomHeight");
	int tile = GetIntParam("MenuBackground","tile");

	StretchImagePtr aStretchImage = new StretchImage(aRawImage,leftWidth,topHeight,rightWidth,bottomHeight);
	switch(tile)
	{
		case 0: aStretchImage->SetTile(false,false); break;
		case 1: aStretchImage->SetTile(true,true); break;
		case 2: aStretchImage->SetTile(true,false); break;
		case 3: aStretchImage->SetTile(false,true); break;
	}

	mSkin->mPopupBack->SetImage(aStretchImage);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WinSkin::CalcDialogBack()
{
	RawImagePtr aRawImage = GetRawImage("MenuBackground","Image");
	if(aRawImage.get()==NULL)
		return;

	int leftWidth = GetIntParam("MenuBackground","LeftWidth");
	int topHeight = GetIntParam("MenuBackground","TopHeight");
	int rightWidth = GetIntParam("MenuBackground","RightWidth");
	int bottomHeight = GetIntParam("MenuBackground","BottomHeight");
	int tile = GetIntParam("MenuBackground","tile");

	StretchImagePtr aStretchImage = new StretchImage(aRawImage,leftWidth,topHeight,rightWidth,bottomHeight);
	switch(tile)
	{
		case 0: aStretchImage->SetTile(false,false); break;
		case 1: aStretchImage->SetTile(true,true); break;
		case 2: aStretchImage->SetTile(true,false); break;
		case 3: aStretchImage->SetTile(false,true); break;
	}

	mSkin->mDialogBack->SetImage(aStretchImage);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WinSkin::CalcCloseButton()
{
	char section[256];
	int i;
	for(i=0; i<50; i++)
	{
		sprintf(section,"Button%d",i);
		if(GetIntParam(section,"Action",-1)==0)
			break;
	}
	if(i==50)
		return;

	RawImagePtr aMasterImage = GetRawImage(section,"ButtonImage");
	if(aMasterImage.get()==NULL)
		return;

	int leftWidth = GetIntParam(section,"LeftWidth");
	int topHeight = GetIntParam(section,"TopHeight");
	int rightWidth = GetIntParam(section,"RightWidth");
	int bottomHeight = GetIntParam(section,"BottomHeight");
	int tile = GetIntParam(section,"tile");
	int numImages = GetIntParam("Personality","TripleImages")?6:3;
	bool mouseOver = GetIntParam(section,"MouseOver")?true:false;

	CropImageFilterPtr aCrop = new CropImageFilter;
	int aWidth = aMasterImage->GetWidth()/numImages;
	aCrop->SetSize(aWidth,aMasterImage->GetHeight());
	
	WONSimpleButtonPtr aButton = new WONSimpleButton;

	int curX = 0;
	for(i=0; i<3; i++)
	{
		aCrop->SetPos(curX,0);
		curX += aWidth;

		RawImagePtr aRawImage = aCrop->Filter(aMasterImage);
		int index = i; if(i==2) index = 3;
		aButton->SetImage(index,aRawImage->GetNative(mWindow->GetDisplayContext()));
	}

	mSkin->mCloseButton = aButton;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONSkin* WinSkin::Load(const char *theFilePath)
{
	mFontMap.clear();
	mColorMap.clear();

	WONFile aFile(theFilePath);

	mFileName = theFilePath;
	mFilePath = aFile.GetFileDirectory() + "\\";


	mWindow = Window::GetDefaultWindow();

//	mSkin->mInputBox = new WONInputBox;
//	mSkin->mInputBox->SetFrame(new ProgramImage(0,0,DrawFrame));
	
//	mSkin->mComboBox = new WONComboBox;
//	mSkin->mComboBox->SetFrame(new ProgramImage(0,0,DrawFrame));

//	mSkin->mScrollContainer = new WONScrollContainer;
//	mSkin->mScrollContainer->SetFrame(new ProgramImage(0,0,DrawFrame));

	CalcColorScheme();
	
	mSkin->mPushButton = CalcButton("Buttons","Bitmap",mSkin->mPushButton);
	mSkin->mHeaderButton = (WONHeaderButton*)CalcButton("HeaderBar","Image",mSkin->mHeaderButton).get();
	mSkin->mTabButton = CalcButton("Tabs","Image",mSkin->mTabButton);
	mSkin->mCheckbox = CalcCheckButton("CheckButton",mSkin->mCheckbox);
	mSkin->mRadioButton = CalcCheckButton("RadioButton",mSkin->mRadioButton);

	mSkin->mPushButton->SetButtonFlags(ButtonFlag_WantDefault,true);
	mSkin->mRadioButton->SetButtonFlags(ButtonFlag_Radio,true);

	CalcArrows();
	CalcComboButton();
	CalcTabCtrlFrame();
	CalcSunkEdge();
	CalcPopupBack();
	CalcDialogBack();
	CalcCloseButton();

	return mSkin;
}


