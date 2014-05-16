#include "MSComponentConfig.h"
#include "ResourceConfig.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::MSComponentConfig_Init(ResourceConfigTable *theTable)
{
	theTable->AddAllocator("MSButton",MSButtonConfig::CfgFactory,MSButtonConfig::ButtonFactory);
	theTable->AddAllocator("MSCheckbox",MSButtonConfig::CfgFactory,MSButtonConfig::CheckboxFactory);
	theTable->AddAllocator("MSRadioButton",MSButtonConfig::CfgFactory,MSButtonConfig::RadioFactory);
	theTable->AddAllocator("MSLabel",MSButtonConfig::CfgFactory,MSButtonConfig::LabelFactory);
	theTable->AddAllocator("MSInputBox",MSInputBoxConfig::CfgFactory,MSInputBoxConfig::CompFactory);
	theTable->AddAllocator("MSComboBox",MSComboBoxConfig::CfgFactory,MSComboBoxConfig::CompFactory);
	theTable->AddAllocator("MSArrowButton",MSArrowButtonConfig::CfgFactory,MSArrowButtonConfig::CompFactory);
	theTable->AddAllocator("MSCloseButton",MSCloseButtonConfig::CfgFactory,MSCloseButtonConfig::CompFactory);
	theTable->AddAllocator("MSTabButton",MSTabButtonConfig::CfgFactory,MSTabButtonConfig::CompFactory);
	theTable->AddAllocator("MSHeaderButton",MSHeaderButtonConfig::CfgFactory,MSHeaderButtonConfig::CompFactory);
	theTable->AddAllocator("MSScrollbar",MSScrollbarConfig::CfgFactory,MSScrollbarConfig::CompFactory);
	theTable->AddAllocator("MSScrollContainer",MSScrollContainerConfig::CfgFactory,MSScrollContainerConfig::CompFactory);
	theTable->AddAllocator("MSSeperator",MSSeperatorConfig::CfgFactory,MSSeperatorConfig::CompFactory);
	theTable->AddAllocator("MS3DFrame",MS3DFrameConfig::CfgFactory,MS3DFrameConfig::CompFactory);
	theTable->AddAllocator("MSEtchedFrame",MSEtchedFrameConfig::CfgFactory,MSEtchedFrameConfig::CompFactory);
	theTable->AddAllocator("MSEtchedFrameGroup",MSEtchedFrameGroupConfig::CfgFactory,MSEtchedFrameGroupConfig::CompFactory);
	theTable->AddAllocator("MSDlgBack",MSDlgBackConfig::CfgFactory,MSDlgBackConfig::CompFactory);
	theTable->AddAllocator("MSTabCtrl",MSTabCtrlConfig::CfgFactory,MSTabCtrlConfig::CompFactory);
	theTable->AddAllocator("MSListCtrl",MSListCtrlConfig::CfgFactory,MSListCtrlConfig::CompFactory);
	theTable->AddAllocator("MSHeaderControl",MSHeaderControlConfig::CfgFactory,MSHeaderControlConfig::CompFactory);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSButtonConfig::MSButtonConfig()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSButtonConfig::SetComponent(Component *theComponent)
{
	ButtonConfig::SetComponent(theComponent);
	mButton = (MSButtonBase*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSButtonConfig::HandleInstruction(const std::string& theInstruction)
{
	if(theInstruction=="FONT")
		mButton->SetFont(ReadFont());
	else if(theInstruction=="TEXTCOLOR")
		mButton->SetTextColor(ReadColor());
	else if(theInstruction=="VERTALIGN")
		mButton->SetVertAlign(ReadVertAlign());
	else if(theInstruction=="HORZALIGN")
		mButton->SetHorzAlign(ReadHorzAlign());
	else if(theInstruction=="TRANSPARENT")
		mButton->SetTransparent(ReadBool());
	else if(theInstruction=="ADJUSTHEIGHTTOFITTEXT")
		mButton->SetAdjustHeightToFitText(ReadBool());
	else if(theInstruction=="WRAP")
		mButton->SetWrap(ReadBool());
	else if(theInstruction=="INCREMENTHORZTEXTPAD")
	{
		int left = ReadInt(); EnsureComma();
		int right = ReadInt();
		mButton->IncrementHorzTextPad(left,right);
	}
	else if(theInstruction=="INCREMENTVERTEXTPAD")
	{
		int top = ReadInt(); EnsureComma();
		int bottom = ReadInt();
		mButton->IncrementVertTextPad(top,bottom);
	}
	else
		return ButtonConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSArrowButtonConfig::SetComponent(Component *theComponent)
{
	ButtonConfig::SetComponent(theComponent);
	mArrowButton = (MSArrowButton*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSArrowButtonConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="DIRECTION")
	{
		GUIString aStr = ReadString();
		if(aStr.compareNoCase("Up")==0)
			mArrowButton->SetDirection(Direction_Up);
		else if(aStr.compareNoCase("Down")==0)
			mArrowButton->SetDirection(Direction_Down);
		else if(aStr.compareNoCase("Left")==0)
			mArrowButton->SetDirection(Direction_Left);
		else if(aStr.compareNoCase("Right")==0)
			mArrowButton->SetDirection(Direction_Right);
	}
	else
		return ButtonConfig::HandleInstruction(theInstruction);

	return true;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSTabButtonConfig::SetComponent(Component *theComponent)
{
	MSButtonConfig::SetComponent(theComponent);
	mTabButton = (MSTabButton*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSTabButtonConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="LOWERAMOUNT")
		mTabButton->SetLowerAmount(ReadInt());
	else
		return MSButtonConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSComboBoxConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="DRAWFRAME")
		mComboBox->SetDrawFrame(ReadBool());
	else
		return ComboBoxConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSScrollbarConfig::SetComponent(Component *theComponent)
{
	ScrollbarConfig::SetComponent(theComponent);
	mScrollbar = (MSScrollbar*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSScrollbarConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="TRANSPARENT")
		mScrollbar->SetTransparent(ReadBool());
	else
		return ScrollbarConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MS3DFrameConfig::SetComponent(Component *theComponent)
{
	ComponentConfig::SetComponent(theComponent);
	mFrame = (MS3DFrame*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MS3DFrameConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="FRAMETYPE")
		mFrame->SetFrameType(ReadInt());
	else if(theInstruction=="UP")
		mFrame->SetIsUp(ReadBool());
	else
		return ComponentConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSEtchedFrameGroupConfig::SetComponent(Component *theComponent)
{
	ContainerConfig::SetComponent(theComponent);
	mFrame = (MSEtchedFrameGroup*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSEtchedFrameGroupConfig::GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, 
													 Component* &theComponent)
{
	if(theName=="LABEL")
	{
		theConfig = LabelConfig::CfgFactory();
		theComponent = mFrame->GetLabel();
	}
	else
		return ContainerConfig::GetPredefinedComponent(theName,theConfig,theComponent);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSEtchedFrameGroupConfig::HandleInstruction(const std::string &theInstruction)
{

	if(theInstruction=="TITLE")
		mFrame->SetLabelText(ReadString());
	else if(theInstruction=="CONTAINEDCOMPONENT")
		mFrame->AddContainedComponent(ReadComponent());
	else
		return ContainerConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSTabCtrlConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="TRANSPARENT")
		mTabCtrl->SetTransparent(ReadBool());
	else if(theInstruction=="ADDSTANDARDTAB")
	{
		GUIString aName = ReadString(); EnsureComma();
		Container *aTabContainer = SafeConfigGetComponent<Container*>(ReadComponent(),"Container");
		mTabCtrl->AddStandardTab(aName,aTabContainer);
	}
	else
		return TabCtrlConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSScrollContainerConfig::SetComponent(Component *theComponent)
{
	ScrollbarScrollerConfig::SetComponent(theComponent);
	mScrollContainer = (MSScrollContainer*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSScrollContainerConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="DRAWFRAME")
		mScrollContainer->SetDrawFrame(ReadBool());
	else if(theInstruction=="DRAW3DFRAME")
		mScrollContainer->SetDraw3DFrame(ReadBool());
	else if(theInstruction=="TRANSPARENT")
		mScrollContainer->SetTransparent(ReadBool());
	else
		return ScrollbarScrollerConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSScrollContainerConfig::GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, 
													 Component* &theComponent)
{
	if(theName=="HORZSCROLLBAR")
	{
		theConfig = new MSScrollbarConfig;
		theComponent = mScrollContainer->GetScrollbar(false);
	}
	else if(theName=="VERTSCROLLBAR")
	{
		theConfig = new MSScrollbarConfig;
		theComponent = mScrollContainer->GetScrollbar(true);
	}
	else
		return ScrollbarScrollerConfig::GetPredefinedComponent(theName,theConfig,theComponent);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSHeaderControlConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="SETSTANDARDCOLUMN")
	{
		int aColumn = ReadInt(); EnsureComma();
		GUIString aTitle = ReadString();
		mHeader->SetColumn(aColumn,aTitle);
	}
	else
		return MultiListHeaderConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSListCtrlConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="HEADERHEIGHT")
		mListCtrl->SetHeaderHeight(ReadInt());
	else
		return MSScrollContainerConfig::HandleInstruction(theInstruction);

	return true;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSListCtrlConfig::GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, Component* &theComponent)
{
	if(theName=="HEADER")
	{
		theComponent = mListCtrl->GetHeader();
		theConfig = new MSHeaderControlConfig;
	}
	else if(theName=="MULTILISTAREA")
	{
		theComponent = mListCtrl->GetListArea();
		theConfig = new MultiListAreaConfig;
	}
	else
		return MSScrollContainerConfig::GetPredefinedComponent(theName,theConfig,theComponent);

	return true;
}

