#include "WONControlsConfig.h"
#include "ResourceConfig.h"
#include "WinSkin.h"

using namespace WONAPI;

static WONSkinPtr gSkin;

static Component* PushButtonFactory() { return gSkin->GetPushButton(); }
static Component* RadioButtonFactory() { return gSkin->GetRadioButton(); }
static Component* CheckboxFactory() { return gSkin->GetCheckbox(); }
static Component* TabButtonFactory() { return gSkin->GetTabButton(); }
static Component* HeaderButtonFactory() { return gSkin->GetHeaderButton(); }
static Component* CloseButtonFactory() { return gSkin->GetCloseButton(); }

static Component* InputBoxFactory() { return gSkin->GetInputBox(); }
static Component* ComboBoxFactory() { return gSkin->GetComboBox(); }

static Component* ScrollContainerFactory() { return gSkin->GetScrollContainer(); }
static Component* ListCtrlFactory() { return gSkin->GetListCtrl(); }

static Component* TabCtrlFactory() { return gSkin->GetTabCtrl(); }
static Component* WONFrameComponentFactory() { return new WONFrameComponent; }
static Component* PopupBackFactory() { return gSkin->GetPopupBack(); }
static Component* DialogBackFactory() { return gSkin->GetDialogBack(); }

static Component* WONButtonSkinFactory() { return new WONButton; }
static Component* WONHeaderButtonSkinFactory() { return new WONHeaderButton; }
static Component* WONSimpleButtonSkinFactory() { return new WONSimpleButton; }
static Component* WONScrollContainerSkinFactory() { return new WONScrollContainer; }
static Component* WONListCtrlSkinFactory() { return new WONListCtrl; }
static Component* WONTabCtrlSkinFactory() { return new WONTabCtrl(gSkin); }
static Component* WONComboBoxSkinFactory() { return new WONComboBox; }
static Component* WONScrollbarSkinFactory() { return new WONScrollbar; }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::WONControlsConfig_Init(ResourceConfigTable *theTable)
{
	theTable->AddAllocator("WONSkin",WONSkinConfig::CfgFactory,ComponentConfig::CompFactory);
	theTable->AddAllocator("WONButtonSkin",WONButtonConfig::CfgFactory,WONButtonSkinFactory);
	theTable->AddAllocator("WONScrollbarSkin",WONScrollbarConfig::CfgFactory,WONScrollbarSkinFactory);
	theTable->AddAllocator("WONHeaderButtonSkin",WONButtonConfig::CfgFactory,WONHeaderButtonSkinFactory);
	theTable->AddAllocator("WONSimpleButtonSkin",WONSimpleButtonConfig::CfgFactory,WONSimpleButtonSkinFactory);
	theTable->AddAllocator("WONScrollContainerSkin",WONScrollContainerConfig::CfgFactory,WONScrollContainerSkinFactory);
	theTable->AddAllocator("WONListCtrlSkin",WONListCtrlConfig::CfgFactory,WONListCtrlSkinFactory);
	theTable->AddAllocator("WONTabCtrlSkin",WONTabCtrlConfig::CfgFactory,WONTabCtrlSkinFactory);
	theTable->AddAllocator("WONComboBoxSkin",WONComboBoxConfig::CfgFactory,WONComboBoxSkinFactory);

	theTable->AddAllocator("WONButton",WONButtonConfig::CfgFactory,PushButtonFactory);
	theTable->AddAllocator("WONCheckbox",WONButtonConfig::CfgFactory,CheckboxFactory);
	theTable->AddAllocator("WONRadioButton",WONButtonConfig::CfgFactory,RadioButtonFactory);
	theTable->AddAllocator("WONTabButton",WONButtonConfig::CfgFactory,TabButtonFactory);
	theTable->AddAllocator("WONHeaderButton",WONButtonConfig::CfgFactory,HeaderButtonFactory);
	theTable->AddAllocator("WONCloseButton",WONButtonConfig::CfgFactory,CloseButtonFactory);
	theTable->AddAllocator("WONPopupBack",WONFrameComponentConfig::CfgFactory,PopupBackFactory);
	theTable->AddAllocator("WONDialogBack",WONFrameComponentConfig::CfgFactory,DialogBackFactory);

	theTable->AddAllocator("WONInputBox",InputBoxConfig::CfgFactory,InputBoxFactory);
	theTable->AddAllocator("WONComboBox",WONComboBoxConfig::CfgFactory,ComboBoxFactory);
	theTable->AddAllocator("WONTabCtrl",WONTabCtrlConfig::CfgFactory,TabCtrlFactory);
	theTable->AddAllocator("WONScrollContainer",WONScrollContainerConfig::CfgFactory,ScrollContainerFactory);
	theTable->AddAllocator("WONListCtrl",WONListCtrlConfig::CfgFactory,ListCtrlFactory);
	theTable->AddAllocator("WONFrameComponent",WONFrameComponentConfig::CfgFactory,WONFrameComponentFactory);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::WONControlsConfig_InitSkin(WONSkin *theSkin)
{
	gSkin = theSkin;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static WONFrame ReadFrame(ComponentConfig *theConfig)
{
	ImagePtr anImage = theConfig->ReadImage();
	StretchImage *aStretchImage = dynamic_cast<StretchImage*>(anImage.get());
	if(aStretchImage!=NULL)
		return aStretchImage;

	theConfig->EnsureComma();
	int left = theConfig->ReadInt(); 
	if(theConfig->EndOfString())
		return WONFrame(anImage,left);

	theConfig->EnsureComma();
	int top = theConfig->ReadInt(); theConfig->EnsureComma();
	int right = theConfig->ReadInt(); theConfig->EnsureComma();
	int bottom = theConfig->ReadInt(); 
	return WONFrame(anImage,left,top,right,bottom);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSimpleButtonConfig::HandleButtonState()
{
	GUIString aStateStr = ReadString();
	EnsureComma();

	int aState = 0;
	bool checked = false;
	if(aStateStr.compareNoCase("Normal")==0) aState = 0;
	else if(aStateStr.compareNoCase("Pressed")==0) aState = 1;
	else if(aStateStr.compareNoCase("Disabled")==0) aState = 2;
	else if(aStateStr.compareNoCase("Over")==0) aState = 3;
	else
		throw ConfigObjectException("Invalid button state: " + (std::string)aStateStr);

	ImagePtr anImage = ReadImage();
	mButton->SetImage(aState,anImage);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSimpleButtonConfig::HandleButtonStates()
{	
	for(int i=0; i<4; i++)
	{
		mButton->SetImage(i,ReadImage()); 
		if(EndOfString())
			return;

		EnsureComma();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONSimpleButtonConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="SCALEIMAGE")
		mButton->SetScaleImage(ReadBool());
	else if(theInstruction=="TYPE")
	{
		GUIString aType = ReadString();
		if(!gSkin->CopySimpleButtonType(aType,mButton))
			throw ConfigObjectException("Type not found: " + std::string(aType));
	}
	else if(theInstruction=="BUTTONSTATE")
		HandleButtonState();
	else if(theInstruction=="BUTTONSTATES")
		HandleButtonStates();
	else if(theInstruction=="CHECKMOUSEBUTTONDOWN")
		mButton->SetCheckMouseButtonDown(ReadBool());
	else
		return ButtonConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButtonConfig::HandleSetButtonState()
{
	GUIString aStateStr = ReadString();
	EnsureComma();

	int aState = 0;
	bool checked = false;
	if(aStateStr.compareNoCase("Normal")==0) aState = 0;
	else if(aStateStr.compareNoCase("Pressed")==0) aState = 1;
	else if(aStateStr.compareNoCase("Disabled")==0) aState = 2;
	else if(aStateStr.compareNoCase("Over")==0) aState = 3;
	else if(aStateStr.compareNoCase("Default")==0) aState = 4;
	else if(aStateStr.compareNoCase("CheckedNormal")==0) { aState = 0; checked = true; }
	else if(aStateStr.compareNoCase("CheckedPressed")==0) { aState = 1; checked = true; }
	else if(aStateStr.compareNoCase("CheckedDisabled")==0) { aState = 2; checked = true; }
	else if(aStateStr.compareNoCase("CheckedOver")==0) { aState = 3; checked = true; }
	else if(aStateStr.compareNoCase("CheckedDefault")==0) { aState = 4; checked = true; }
	else
		throw ConfigObjectException("Invalid button state: " + (std::string)aStateStr);

	ImagePtr anImage = ReadImage();
	int aColor = -1;
	FontPtr aFont;
	if(!EndOfString())
	{
		EnsureComma();
		aColor = ReadColor();
		if(!EndOfString())
		{
			EnsureComma();
			aFont = ReadFont();
		}
	}

	mButton->SetButtonState(aState,anImage,aColor,aFont,checked);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONButtonConfig::HandleInstruction(const std::string& theInstruction)
{
	if(theInstruction=="FONT")
		mButton->SetFont(ReadFont());
	else if(theInstruction=="TYPE")
	{
		GUIString aType = ReadString();
		if(!gSkin->CopyButtonType(aType,mButton))
			throw ConfigObjectException("Type not found: " + std::string(aType));
	}
	else if(theInstruction=="TEXTCOLOR")
		mButton->SetTextColor(ReadColor());
	else if(theInstruction=="DEFAULTTEXTCOLOR")
		mButton->SetDefaultTextColor(ReadColor());
	else if(theInstruction=="VERTALIGN")
		mButton->SetVertAlign(ReadVertAlign());
	else if(theInstruction=="HORZALIGN")
		mButton->SetHorzAlign(ReadHorzAlign());
	else if(theInstruction=="TRANSPARENT")
		mButton->SetTransparent(ReadBool());
	else if(theInstruction=="ADJUSTHEIGHTTOFITTEXT")
		mButton->SetAdjustHeightToFitText(ReadBool());
	else if(theInstruction=="WRAP")
		mButton->SetWrapText(ReadBool());
	else if(theInstruction=="DRAWGRAYTEXT")
		mButton->SetDrawGrayText(ReadBool());
	else if(theInstruction=="TEXTPADDING")
	{
		int left = ReadInt(); EnsureComma();
		int top = ReadInt(); EnsureComma();
		int right = ReadInt(); EnsureComma();
		int bottom = ReadInt();
		mButton->SetTextPadding(left,top,right,bottom);
	}
	else if(theInstruction=="FOCUSPADDING")
	{
		int left = ReadInt(); EnsureComma();
		int top = ReadInt(); EnsureComma();
		int right = ReadInt(); EnsureComma();
		int bottom = ReadInt();
		mButton->SetFocusPadding(left,top,right,bottom);
	}
	else if(theInstruction=="HORZTEXTPAD")
	{
		int left = ReadInt(); 
		int right = mButton->GetRightTextPad();
		if(!EndOfString())
		{
			EnsureComma(); 
			right = ReadInt();
		}
		mButton->SetHorzTextPad(left,right);
	}
	else if(theInstruction=="INCREMENTHORZTEXTPAD")
	{
/*		int left = ReadInt(); EnsureComma();
		int right = ReadInt();
		mButton->IncrementHorzTextPad(left,right);
*/	}
	else if(theInstruction=="INCREMENTVERTEXTPAD")
	{
/*		int top = ReadInt(); EnsureComma();
		int bottom = ReadInt();
		mButton->IncrementVertTextPad(top,bottom);
*/	}
	else if(theInstruction=="BUTTONSTATE")
		HandleSetButtonState();
	else if(theInstruction=="SCALEIMAGE")
		mButton->SetScaleImage(ReadBool());
	else if(theInstruction=="DRAWFOCUSRECT")
		mButton->SetFocusStyle(ReadBool()?1:0);
	else
		return ButtonConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONComboBoxConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="DRAWFRAME")
		mComboBox->SetDrawFrame(ReadBool());
	else if(theInstruction=="BUTTONINFRAME")
		mComboBox->SetButtonInFrame(ReadBool());
	else if(theInstruction=="FRAME")
		mComboBox->SetFrame(ReadFrame(this));
	else
		return ComboBoxConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONTabCtrlConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="TRANSPARENT")
		mTabCtrl->SetTransparent(ReadBool());
	else if(theInstruction=="ADDSTANDARDTAB")
	{
		GUIString aName = ReadString(); EnsureComma();
		Container *aTabContainer = SafeConfigGetComponent<Container*>(ReadComponent(),"Container");
		mTabCtrl->AddStandardTab(aName,aTabContainer);
	}
	else if(theInstruction=="FRAME")
		mTabCtrl->SetFrame(ReadFrame(this));
	else 
		return TabCtrlConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONFrameComponentConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="BORDERS")
	{
		mFrameComponent->SetLeft(ReadInt()); EnsureComma();
		mFrameComponent->SetTop(ReadInt()); EnsureComma();
		mFrameComponent->SetRight(ReadInt()); EnsureComma();
		mFrameComponent->SetBottom(ReadInt());
	}
	else if(theInstruction=="IMAGE")
		mFrameComponent->SetImage(ReadImage());
	else
		return ComponentConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONFrameComponentConfig::GetNamedIntValue(const std::string &theName, int &theValue)
{
	if(theName=="LEFTPAD")
		theValue = mFrameComponent->Left();
	else if(theName=="RIGHTPAD")
		theValue = mFrameComponent->Right();
	else if(theName=="TOPPAD")
		theValue = mFrameComponent->Top();
	else if(theName=="BOTTOMPAD")
		theValue = mFrameComponent->Bottom();
	else if(theName=="HORZPAD")
		theValue = mFrameComponent->Left() + mFrameComponent->Right();
	else if(theName=="VERTPAD")
		theValue = mFrameComponent->Top() + mFrameComponent->Bottom();
	else
		return ComponentConfig::GetNamedIntValue(theName, theValue);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONScrollbarConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="TRANSPARENT")
	{
		if(ReadBool())
			mScrollbar->SetScrollRect(new Button);
	}
	else
		return ScrollbarConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONScrollContainerConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="DRAWFRAME")
	{
		if(!ReadBool())
			mScrollContainer->SetFrame(WONFrame());
	}
	else if(theInstruction=="TRANSPARENT")
		mScrollContainer->SetTransparent(ReadBool());
	else if(theInstruction=="FRAME")
		mScrollContainer->SetFrame(ReadFrame(this));
	else if(theInstruction=="CORNER")
		mScrollContainer->SetCorner(ReadImage());
	else
		return ScrollbarScrollerConfig::HandleInstruction(theInstruction);
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONScrollContainerConfig::GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, 
													 Component* &theComponent)
{
	if(theName=="HORZSCROLLBAR")
	{
		theConfig = new WONScrollbarConfig;
		theComponent = mScrollContainer->GetScrollbar(false);
	}
	else if(theName=="VERTSCROLLBAR")
	{
		theConfig = new WONScrollbarConfig;
		theComponent = mScrollContainer->GetScrollbar(true);
	}
	else
		return ScrollbarScrollerConfig::GetPredefinedComponent(theName,theConfig,theComponent);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONHeaderControlConfig::HandleInstruction(const std::string &theInstruction)
{
	if(MultiListHeaderConfig::HandleInstruction(theInstruction))
		return true;

	if(theInstruction=="SETSTANDARDCOLUMN")
	{
		int aColumn = ReadInt(); EnsureComma();
		GUIString aTitle = ReadString();
		mHeader->SetColumn(aColumn,aTitle);
	}
	else
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONListCtrlConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="HEADERHEIGHT")
		mListCtrl->SetHeaderHeight(ReadInt());
	else if(theInstruction=="NUMCOLUMNS")
		mListCtrl->GetListArea()->SetNumColumns(ReadInt());
	else if(theInstruction=="DYNAMICSIZECOLUMN")
		mListCtrl->GetListArea()->SetDynamicSizeColumn(ReadInt());
	else if(theInstruction=="LISTFLAGS")
		ListAreaConfig::ReadListFlags(this,mListCtrl->GetListArea());
	else if(theInstruction=="SETSTANDARDCOLUMN")
	{
		int aCol = ReadInt(); EnsureComma();
		GUIString aTitle = ReadString(); EnsureComma();
		int aWidth = ReadInt();
		mListCtrl->SetStandarColumnHeader(aCol,aTitle,aWidth);
	}
	else if(theInstruction=="SETCOLUMN")
	{
		int aCol = ReadInt(); EnsureComma();
		Component *aComponent = ReadComponent(); EnsureComma();
		int aWidth = ReadInt();
		mListCtrl->SetColumnHeader(aCol,aComponent,aWidth);
	}	
	else
		return WONScrollContainerConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONListCtrlConfig::GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, Component* &theComponent)
{
	if(theName=="HEADER")
	{
		theComponent = mListCtrl->GetHeader();
		theConfig = new WONHeaderControlConfig;
	}
	else if(theName=="MULTILISTAREA")
	{
		theComponent = mListCtrl->GetListArea();
		theConfig = new MultiListAreaConfig;
	}
	else
		return WONScrollContainerConfig::GetPredefinedComponent(theName,theConfig,theComponent);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONSkinConfig::ReadVert()
{
	GUIString aStr = ReadString();
	return aStr.compareNoCase("Vert")==0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONSkinConfig::PreParse(ConfigParser &theParser)
{
	if(!ComponentConfig::PreParse(theParser))
		return false;

	if(gSkin.get()==NULL)
		gSkin = new WONMSSkin;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSkinConfig::HandleButtonClickSound()
{
	SoundPtr aSound = ReadSound();
	gSkin->mPushButton->SetClickSound(aSound);
	gSkin->mCheckbox->SetClickSound(aSound);
	gSkin->mRadioButton->SetClickSound(aSound);
	gSkin->mHeaderButton->SetClickSound(aSound);
	gSkin->mTabButton->SetClickSound(aSound);
	gSkin->mCloseButton->SetClickSound(aSound);
	gSkin->mComboButton->SetClickSound(aSound);

	WONSkin::ButtonTypeMap::iterator anItr = gSkin->mButtonTypeMap.begin();
	for(; anItr!=gSkin->mButtonTypeMap.end(); ++anItr)
		anItr->second->SetClickSound(aSound);

	//WONSimpleButtonPtr mArrowButton[4];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONSkinConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="WINSKIN")
	{
#ifdef _DEBUG
		WinSkinPtr aSkin = new WinSkin;
		GUIString aStr = ReadString();
		gSkin = aSkin->Load(ComponentConfig::GetResourceFile(aStr).c_str());
#endif
	}
	else if(theInstruction=="ADDBUTTONTYPE")
	{
		GUIString aName = ReadString(); EnsureComma();
		WONButton* aButton = SafeConfigGetComponent<WONButton*>(ReadComponent(),"WONButton");
		gSkin->mButtonTypeMap[aName] = aButton;
	}
	else if(theInstruction=="ADDSIMPLEBUTTONTYPE")
	{
		GUIString aName = ReadString(); EnsureComma();
		WONSimpleButton* aButton = SafeConfigGetComponent<WONSimpleButton*>(ReadComponent(),"WONSimpleButton");
		gSkin->mSimpleButtonTypeMap[aName] = aButton;
	}
	else if(theInstruction=="SETPUSHBUTTON")
		gSkin->mPushButton = SafeConfigGetComponent<WONButton*>(ReadComponent(),"WONButton");
	else if(theInstruction=="SETCHECKBOX")
		gSkin->mCheckbox = SafeConfigGetComponent<WONButton*>(ReadComponent(),"WONButton");
	else if(theInstruction=="SETRADIOBUTTON")
	{
		gSkin->mRadioButton = SafeConfigGetComponent<WONButton*>(ReadComponent(),"WONButton");
		gSkin->mRadioButton->SetButtonFlags(ButtonFlag_Radio,true);
	}
	else if(theInstruction=="SETHEADERBUTTON")
		gSkin->mHeaderButton = SafeConfigGetComponent<WONHeaderButton*>(ReadComponent(),"WONHeaderButton");
	else if(theInstruction=="SETTABBUTTON")
		gSkin->mTabButton = SafeConfigGetComponent<WONButton*>(ReadComponent(),"WONButton");
	else if(theInstruction=="SETARROWBUTTON")
	{
		Direction aDirection = ReadDirection(); EnsureComma();
		gSkin->mArrowButton[aDirection] = SafeConfigGetComponent<WONSimpleButton*>(ReadComponent(),"WONSimpleButton");

	}
	else if(theInstruction=="SETCOMBOBUTTON")
		gSkin->mComboButton = SafeConfigGetComponent<WONSimpleButton*>(ReadComponent(),"WONSimpleButton");
	else if(theInstruction=="SETCLOSEBUTTON")
		gSkin->mCloseButton = SafeConfigGetComponent<WONSimpleButton*>(ReadComponent(),"WONSimpleButton");
	else if(theInstruction=="SETSCROLLGUTTER")
	{
		bool isVert = ReadVert(); EnsureComma();
		gSkin->mScrollGutter[isVert?1:0] = SafeConfigGetComponent<WONSimpleButton*>(ReadComponent(),"WONSimpleButton");
	}
	else if(theInstruction=="SETSCROLLTHUMB")
	{
		bool isVert = ReadVert(); EnsureComma();
		gSkin->mScrollThumb[isVert?1:0] = SafeConfigGetComponent<WONSimpleButton*>(ReadComponent(),"WONSimpleButton");
	}
	else if(theInstruction=="SETSCROLLBAR")
	{
		bool isVert = ReadVert(); EnsureComma();
		gSkin->mScrollbar[isVert?1:0] = SafeConfigGetComponent<WONScrollbar*>(ReadComponent(),"WONScrollbar");
	}
	else if(theInstruction=="SETSCROLLCONTAINER")
		gSkin->mScrollContainer = SafeConfigGetComponent<WONScrollContainer*>(ReadComponent(),"WONScrollContainer");
	else if(theInstruction=="SETLISTCTRL")
		gSkin->mListCtrl = SafeConfigGetComponent<WONListCtrl*>(ReadComponent(),"WONListCtrl");
	else if(theInstruction=="SETTABCTRL")
	{
		gSkin->mTabCtrl = SafeConfigGetComponent<WONTabCtrl*>(ReadComponent(),"WONTabCtrl");
		gSkin->mTabCtrl->SetSkin(NULL);
	}
	else if(theInstruction=="SETCOMBOBOX")
		gSkin->mComboBox = SafeConfigGetComponent<WONComboBox*>(ReadComponent(),"WONComboBox");
	else if(theInstruction=="SETPOPUPBACK")
		gSkin->mPopupBack = SafeConfigGetComponent<WONFrameComponent*>(ReadComponent(),"WONPopupBack");
	else if(theInstruction=="SETDIALOGBACK")
		gSkin->mDialogBack = SafeConfigGetComponent<WONFrameComponent*>(ReadComponent(),"WONDialogBack");
	else if(theInstruction=="BUTTONCLICKSOUND")
		HandleButtonClickSound();
	else if(theInstruction=="IMAGECURSOR")
	{
		StandardCursor aType = ReadStandardCursor(); EnsureComma();
		ImagePtr anImage = ReadImage();
		CursorPtr aCursor = new ComponentCursor(new ImageComponent(anImage));
		Cursor::SetStandardCursor(aType,aCursor);
	}
	else
		return false;

	return true;
}
