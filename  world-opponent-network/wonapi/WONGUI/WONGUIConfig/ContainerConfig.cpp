#include "ContainerConfig.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/Window.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ContainerConfig::ContainerConfig()
{
	mLayoutRef = NULL;
	mCurLayoutId = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ContainerConfig::SetComponent(Component *theComponent)
{
	ComponentConfig::SetComponent(theComponent);
	mContainer = (Container*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class PercentLayoutConfig : public ComponentConfig
{
public:
	Component *target;
	Component *refs[4];
	int params[8];

	PercentLayoutConfig();
	virtual bool HandleInstruction(const std::string &theInstruction);
	virtual bool PreParse(ConfigParser &theParser);
	virtual bool PostParse(ConfigParser &theParser) { return true; }
};
typedef SmartPtr<PercentLayoutConfig> PercentLayoutConfigPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PercentLayoutConfig::PercentLayoutConfig()
{
	int i;
	for(i = 0; i<4; i++)
		refs[i] = NULL;

	for(i = 0; i<8; i++)
		params[i] = -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool PercentLayoutConfig::PreParse(ConfigParser &theParser)
{
	ComponentConfig* aConfig = (ComponentConfig*)GetParent();
	target = aConfig->GetChildComponent(GetName());
	if(target==NULL)
	{
		theParser.AbortRead("Component not found: " + GetName());
		return false;
	}
	else
		return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool PercentLayoutConfig::HandleInstruction(const std::string &theInstruction)
{
	int refNum = -1, paramNum = -1;
	
	if(theInstruction=="REF") refNum = 0;
	else if(theInstruction=="LEFT") refNum = 0;
	else if(theInstruction=="RIGHT") refNum = 1;
	else if(theInstruction=="TOP") refNum = 2;
	else if(theInstruction=="BOTTOM") refNum = 3;

	else if(theInstruction=="PX") paramNum = 0;
	else if(theInstruction=="PY") paramNum = 1;
	else if(theInstruction=="PW") paramNum = 2;
	else if(theInstruction=="PH") paramNum = 3;
	else if(theInstruction=="LP") paramNum = 4;
	else if(theInstruction=="TP") paramNum = 5;
	else if(theInstruction=="RP") paramNum = 6;
	else if(theInstruction=="BP") paramNum = 7;
	else
		return false;

	if(refNum>=0)
		refs[refNum] = ReadComponent();
	else if(paramNum>=0)
		params[paramNum] = ReadInt();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ColorSchemeConfig : public ComponentConfig
{
public:
	Component *target;
	ColorSchemePtr mColorScheme;
	bool mSetGlobal;

	ColorSchemeConfig();
	virtual bool HandleInstruction(const std::string &theInstruction);
	virtual bool PreParse(ConfigParser &theParser);
	virtual bool PostParse(ConfigParser &theParser) { return true; }
};
typedef SmartPtr<ColorSchemeConfig> ColorSchemeConfigPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ColorSchemeConfig::ColorSchemeConfig()
{
	mColorScheme = new ColorScheme;
	mSetGlobal = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ColorSchemeConfig::PreParse(ConfigParser &theParser)
{
	ComponentConfig* aConfig = (ComponentConfig*)GetParent();
	Component* tempComponent;
	ColorScheme* colorScheme;
	while(aConfig)
	{
		tempComponent = aConfig->GetComponent();
		if(tempComponent)
		{
			colorScheme = tempComponent->GetOwnColorScheme();
			if(colorScheme)
			{
				mColorScheme->CopyFrom(colorScheme);
				return true;
			}
		}
		aConfig = (ComponentConfig*)aConfig->GetParent();
	}

	mColorScheme->CopyFrom(WindowManager::GetDefaultWindowManager()->GetColorScheme());
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ColorSchemeConfig::HandleInstruction(const std::string &theInstruction)
{
	StandardColor colorNum = StandardColor_3DDarkShadow;

	if(theInstruction=="3DDARKSHADOW") colorNum = StandardColor_3DDarkShadow;
	else if(theInstruction=="3DFACE") colorNum = StandardColor_3DFace;
	else if(theInstruction=="3DHILIGHT") colorNum = StandardColor_3DHilight;
	else if(theInstruction=="3DSHADOW") colorNum = StandardColor_3DShadow;
	else if(theInstruction=="SCROLLBAR") colorNum = StandardColor_Scrollbar;
	else if(theInstruction=="BUTTONTEXT") colorNum = StandardColor_ButtonText;
	else if(theInstruction=="GRAYTEXT") colorNum = StandardColor_GrayText;
	else if(theInstruction=="HILIGHT") colorNum = StandardColor_Hilight;
	else if(theInstruction=="HILIGHTTEXT") colorNum = StandardColor_HilightText;
	else if(theInstruction=="TOOLTIPBACK") colorNum = StandardColor_ToolTipBack;
	else if(theInstruction=="TOOLTIPTEXT") colorNum = StandardColor_ToolTipText;
	else if(theInstruction=="MENUBACK") colorNum = StandardColor_MenuBack;
	else if(theInstruction=="MENUTEXT") colorNum = StandardColor_MenuText;
	else if(theInstruction=="BACK") colorNum = StandardColor_Back;
	else if(theInstruction=="TEXT") colorNum = StandardColor_Text;
	else if(theInstruction=="LINK") colorNum = StandardColor_Link;
	else if(theInstruction=="LINKDOWN") colorNum = StandardColor_LinkDown;
	else if(theInstruction=="SETGLOBAL") 
	{
		mSetGlobal = ReadBool();
		return true;
	}
	else if(theInstruction=="COPYSYSTEM")
	{
		WindowManager::GetDefaultWindowManager()->GetSystemColorScheme(mColorScheme);
		return true;
	}
	else
		return false;

	mColorScheme->SetStandardColor(colorNum,ReadColor());
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ContainerConfig::HandleChildGroup(ConfigParser &theParser, const std::string& theGroupType)
{
	if(theGroupType=="PERCENTLAYOUT")
	{
		PercentLayoutConfigPtr aLayout = new PercentLayoutConfig;
		if(ParseChild(theParser,aLayout,false))
		{
			int i;
			for(i=0; i<4; i++)
			{
				if(aLayout->refs[i]!=NULL)
					break;
			}

			if(i==4)
			{
//				theParser.AbortRead("Invalid PercentLayout: Need at least one ref.");
//				return true;
				aLayout->refs[0] = GetComponent(); // this
			}


			PercentLayoutParam aParam(aLayout->refs[0], aLayout->refs[1], aLayout->refs[2], aLayout->refs[3],
				aLayout->params[0], aLayout->params[1], aLayout->params[2], aLayout->params[3],
				aLayout->params[4], aLayout->params[5], aLayout->params[6], aLayout->params[7]);

			mContainer->AddChildLayout(new PercentLayout(aLayout->target,aParam),mCurLayoutId);
		}
		
		return true;

	}
	else if(theGroupType=="COLORSCHEME")
	{
		ColorSchemeConfigPtr aConfig = new ColorSchemeConfig;
		if(ParseChild(theParser,aConfig,false))
		{
			if(aConfig->mSetGlobal)
				WindowManager::GetDefaultWindowManager()->GetColorScheme()->CopyFrom(aConfig->mColorScheme);
			else
				mContainer->SetColorScheme(aConfig->mColorScheme);
		}

		return true;
	}
	else
		return ComponentConfig::HandleChildGroup(theParser,theGroupType);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ContainerConfig::HandleAdd()
{
	while(true)
	{
		Component *aComp = ReadComponent();
		mContainer->AddChild(aComp);
		if(EndOfString())
			break;

		EnsureComma();
	}


	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ContainerConfig::HandleAddLayoutPoint()
{
	while(true)
	{
		LayoutPoint *aPoint = SafeConfigGetComponent<LayoutPoint*>(ReadComponent(),"LayoutPoint");
		mContainer->AddLayoutPoint(aPoint);
		if(EndOfString())
			break;

		EnsureComma();
	}


	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ContainerConfig::HandleLayout(bool chain, bool repeat)
{
	Component *target = ReadComponent(); 
	int instructions = 0;
	Component *ref;

	if(!chain && !repeat)
	{
		EnsureComma();
		while(true)
		{
			GUIString aStr = ReadString(true);
			if(aStr.empty())
				break;

			if(aStr.compareNoCase("SameWidth")==0) instructions |= CLI_SameWidth;
			else if(aStr.compareNoCase("SameHeight")==0) instructions |= CLI_SameHeight;
			else if(aStr.compareNoCase("Above")==0) instructions |= CLI_Above;
			else if(aStr.compareNoCase("Below")==0) instructions |= CLI_Below;
			else if(aStr.compareNoCase("Right")==0) instructions |= CLI_Right;
			else if(aStr.compareNoCase("Left")==0) instructions |= CLI_Left;
			else if(aStr.compareNoCase("SameLeft")==0) instructions |= CLI_SameLeft;
			else if(aStr.compareNoCase("SameRight")==0) instructions |= CLI_SameRight;
			else if(aStr.compareNoCase("SameTop")==0) instructions |= CLI_SameTop;
			else if(aStr.compareNoCase("SameBottom")==0) instructions |= CLI_SameBottom;
			else if(aStr.compareNoCase("GrowToRight")==0) instructions |= CLI_GrowToRight;
			else if(aStr.compareNoCase("GrowToLeft")==0) instructions |= CLI_GrowToLeft;
			else if(aStr.compareNoCase("GrowToTop")==0) instructions |= CLI_GrowToTop;
			else if(aStr.compareNoCase("GrowToBottom")==0) instructions |= CLI_GrowToBottom;
			else if(aStr.compareNoCase("SameSize")==0) instructions |= CLI_SameSize;	
			else 
				throw ConfigObjectException("Unknown layout instruction: " + (std::string)aStr);
		}
	}
	else
		instructions = mLayoutInstruction;

	if(chain || (repeat && EndOfString()))
	{
		if(mLayoutRef==NULL)
			throw ConfigObjectException("Invalid LayoutChain");

		ref = mLayoutRef;
	}
	else
	{
		EnsureComma();
		ref = ReadComponent();
	}

	if(!EndOfString())
		EnsureComma();

	for(int i=0; i<4; i++)
	{
		if(EndOfString())
			break;

		mLayoutParams[i] = ReadInt();
		if(!EndOfString())
			EnsureComma();
	}

	if((!chain && !repeat) || i!=0)
	{
		for(; i<4; i++)
			mLayoutParams[i] = 0;
	}

	mContainer->AddChildLayout(target,instructions,ref,mLayoutParams[0],mLayoutParams[1],
		mLayoutParams[2],mLayoutParams[3],mCurLayoutId);

	if(repeat)
		mLayoutRef = ref;
	else
		mLayoutRef = target;

	mLayoutInstruction = instructions;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ContainerConfig::HandleSameSizeLayout(bool sameWidth, bool sameHeight)
{	
	SameSizeLayoutPtr aLayout = new SameSizeLayout(sameWidth, sameHeight);

	while(true)
	{	
		Component *aComp = ReadComponent();
		aLayout->Add(aComp);
		if(EndOfString())
			break;
		else
			EnsureComma();
	}

	mContainer->AddChildLayout(aLayout,mCurLayoutId);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ContainerConfig::HandleCenterLayout(bool horz, bool vert)
{
	Component *aComp = ReadComponent();
	CenterLayoutPtr aLayout = new CenterLayout(aComp,horz,vert);
	while(!EndOfString())
	{	
		EnsureComma();
		aComp = ReadComponent();
		aLayout->Add(aComp);
	}

	mContainer->AddChildLayout(aLayout,mCurLayoutId);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ContainerConfig::HandleClearRightLayout()
{
	Component *aComp = ReadComponent(); EnsureComma();
	int aPad = ReadInt(); EnsureComma();

	ClearRightLayoutPtr aLayout = new ClearRightLayout(aComp,aPad);
	while(true)
	{
		aComp = ReadComponent();
		aLayout->Add(aComp);
		if(EndOfString())
			break;

		EnsureComma();
	}

	mContainer->AddChildLayout(aLayout,mCurLayoutId);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ContainerConfig::HandleSpaceEvenlyLayout(bool horz)
{
	Component *aSpaceBox = ReadComponent(); EnsureComma();
	int aLeftPad = ReadInt(); EnsureComma();
	int aRightPad = ReadInt(); EnsureComma();

	SpaceEvenlyLayoutPtr aLayout = new SpaceEvenlyLayout(aSpaceBox,aLeftPad,aRightPad,horz);
	while(!EndOfString())
	{
		Component *aComponent = ReadComponent();
		aLayout->Add(aComponent);
		if(EndOfString())
			break;

		EnsureComma();
	}

	mContainer->AddChildLayout(aLayout,mCurLayoutId);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ContainerConfig::HandleNoOverlapLayout()
{
	Component *target = ReadComponent();
	EnsureComma();

	int instructions = 0;
	while(true)
	{
		GUIString aStr = ReadString(true);
		if(aStr.empty())
			break;

		if(aStr.compareNoCase("Above")==0) instructions |= CLI_Above;
		else if(aStr.compareNoCase("Below")==0) instructions |= CLI_Below;
		else if(aStr.compareNoCase("Right")==0) instructions |= CLI_Right;
		else if(aStr.compareNoCase("Left")==0) instructions |= CLI_Left;
		else 
			throw ConfigObjectException("Unknown no overlap instruction: " + (std::string)aStr);
	}

	EnsureComma();
	Component *ref = ReadComponent(); EnsureComma();
	int horzPad, vertPad;
	horzPad = ReadInt(); EnsureComma();
	vertPad = ReadInt(); 

	NoOverlapLayoutPtr aLayout = new NoOverlapLayout(target,instructions,ref,horzPad,vertPad);
	while(!EndOfString())
	{
		EnsureComma();
		Component *aComponent = ReadComponent();
		aLayout->Add(aComponent);
	}

	mContainer->AddChildLayout(aLayout);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ContainerConfig::HandleSizeAtLeastLayout()
{
	Component *target = ReadComponent(); EnsureComma();
	int aHorzPad = ReadInt(); EnsureComma();
	int aVertPad = ReadInt(); 

	SizeAtLeastLayoutPtr aLayout = new SizeAtLeastLayout(target,aHorzPad, aVertPad);
	while(!EndOfString())
	{
		EnsureComma();
		aLayout->Add(ReadComponent());		
	}
	mContainer->AddChildLayout(aLayout);
	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ContainerConfig::HandleNewLayoutPoint()
{
	while(true)
	{
		ComponentConfigPtr aConfig = new ComponentConfig;
		LayoutPointPtr aPoint = new LayoutPoint;
		aConfig->SetComponent(aPoint);
		aConfig->SetParent(this);
		mConfigMap[ReadString()] = aConfig;
		mContainer->AddLayoutPoint(aPoint);

		if(EndOfString())
			break;

		EnsureComma();
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ContainerConfig::HandleInstruction(const std::string& theInstruction)
{
	if(theInstruction=="ADD")
		return HandleAdd();
	else if(theInstruction=="ADDLAYOUTPOINT")
		return HandleAddLayoutPoint();
	else if(theInstruction=="NEWLAYOUTPOINT")
		return HandleNewLayoutPoint();
	else if(theInstruction=="LAYOUT")
		return HandleLayout();
	else if(theInstruction=="SETLAYOUTID")
		mCurLayoutId = ReadInt();
	else if(theInstruction=="ENABLELAYOUT")
	{
		bool enable = ReadBool();
		while(true)
		{
			EnsureComma();
			int anId = ReadInt();
			mContainer->EnableLayout(anId,enable);
			if(EndOfString())
				break;
		}
	}
	else if(theInstruction=="RECALCLAYOUT")
		mContainer->RecalcLayout();
	else if(theInstruction=="CHAINLAYOUT")
		return HandleLayout(true);
	else if(theInstruction=="REPEATLAYOUT")
		return HandleLayout(false,true);
	else if(theInstruction=="SAMESIZELAYOUT")
		return HandleSameSizeLayout(true,true);
	else if(theInstruction=="SAMEWIDTHLAYOUT")
		return HandleSameSizeLayout(true,false);
	else if(theInstruction=="SAMEHEIGHTLAYOUT")
		return HandleSameSizeLayout(false,true);
	else if(theInstruction=="CENTERLAYOUT")
		return HandleCenterLayout(true,true);
	else if(theInstruction=="HORZCENTERLAYOUT")
		return HandleCenterLayout(true,false);
	else if(theInstruction=="VERTCENTERLAYOUT")
		return HandleCenterLayout(false,true);
	else if(theInstruction=="HORZSPACEEVENLYLAYOUT")
		return HandleSpaceEvenlyLayout(true);
	else if(theInstruction=="VERTSPACEEVENLYLAYOUT")
		return HandleSpaceEvenlyLayout(false);
	else if(theInstruction=="CLEARRIGHTLAYOUT")
		return HandleClearRightLayout();
	else if(theInstruction=="NOOVERLAPLAYOUT")
		return HandleNoOverlapLayout();
	else if(theInstruction=="SIZEATLEASTLAYOUT")
		return HandleSizeAtLeastLayout();
	else if(theInstruction=="ISOLATETABFOCUS")
		mContainer->SetIsolateTabFocus(ReadBool());
	else if(theInstruction=="ONLYCHILDINPUT")
		mContainer->SetOnlyChildInput(ReadBool());
	else
		return ComponentConfig::HandleInstruction(theInstruction);

	return true;
}
