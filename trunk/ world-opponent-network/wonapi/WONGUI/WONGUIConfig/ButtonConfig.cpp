#include "ButtonConfig.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ButtonConfig::ButtonConfig()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ButtonConfig::SetComponent(Component *theComponent)
{
	ComponentConfig::SetComponent(theComponent);
	mButton = (Button*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ButtonConfig::HandleButtonFlags()
{
	while(true)
	{
		bool add = true;
		int aFlag = 0;
		GUIString aStr = ReadFlag(&add);
		if(aStr.empty())
			break;

		if(aStr.compareNoCase("InvalidateOnMouseOver")==0) aFlag = ButtonFlag_InvalidateOnMouseOver;
		else if(aStr.compareNoCase("RepeatFire")==0) aFlag = ButtonFlag_RepeatFire;
		else if(aStr.compareNoCase("Checked")==0) aFlag = ButtonFlag_Checked;
		else if(aStr.compareNoCase("Radio")==0) aFlag = ButtonFlag_Radio;
		else if(aStr.compareNoCase("ActivateDown")==0) aFlag = ButtonFlag_ActivateDown;
		else if(aStr.compareNoCase("WantDefault")==0) aFlag = ButtonFlag_WantDefault;
		else if(aStr.compareNoCase("EndDialog")==0) aFlag = ButtonFlag_EndDialog;
		else if(aStr.compareNoCase("NoActivate")==0) aFlag = ButtonFlag_NoActivate;
		else if(aStr.compareNoCase("WantDefault")==0) aFlag = ButtonFlag_WantDefault;
		else 
			throw ConfigObjectException("Unknown button flag" + (std::string)aStr);

		mButton->SetButtonFlags(aFlag,add);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ButtonConfig::HandleFitText()
{
	int maxWidth = mButton->Width(), maxHeight = mButton->Height();
	GUIString anOldText = mButton->GetText();
	while(true)
	{
		GUIString aStr = ReadString();
		mButton->SetText(aStr);
		int width = 0,height = 0;
		mButton->GetDesiredSize(width,height);

		if(width>maxWidth)
			maxWidth = width;
		if(height>maxHeight)
			maxHeight = height;

		if(!EndOfString())
			EnsureComma();
		else
			break;
	}

	mButton->SetText(anOldText);
	mButton->SetSize(maxWidth,maxHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ButtonConfig::HandleInstruction(const std::string& theInstruction)
{
	if(theInstruction=="BUTTONFLAGS")
		HandleButtonFlags();
	else if(theInstruction=="TEXT")
		mButton->SetText(ReadString());
	else if(theInstruction=="SETCHECK")
		mButton->SetCheck(ReadBool());
	else if(theInstruction=="CLICKSOUND")
		mButton->SetClickSound(ReadSound());
	else if(theInstruction=="FITTEXT")
		HandleFitText();
	else
		return ComponentConfig::HandleInstruction(theInstruction);

	return true;
}

