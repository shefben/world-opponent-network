#include "InputBoxConfig.h"
#include "WONGUI/ChildLayouts.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBoxConfig::SetComponent(Component *theComponent)
{
	ComponentConfig::SetComponent(theComponent);
	mInputBox = (InputBox*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool InputBoxConfig::HandleInstruction(const std::string& theInstruction)
{
	if(theInstruction=="TEXT")
		mInputBox->SetText(ReadString());
	else if(theInstruction=="PASSWORDCHAR")
		mInputBox->SetPasswordChar(ReadChar());
	else if(theInstruction=="ALLOWSELECTION")
		mInputBox->SetAllowSelection(ReadBool());
	else if(theInstruction=="MAXCHARS")
		mInputBox->SetMaxChars(ReadInt());
	else if(theInstruction=="CHARWIDTH")
		mInputBox->SetCharWidth(ReadInt());
	else if(theInstruction=="BACKGROUND")
		mInputBox->SetBackground(ReadBackground());
	else if(theInstruction=="DISABLEDBACKGROUND")
		mInputBox->SetDisabledBackground(ReadBackground());
	else if(theInstruction=="ICON")
		mInputBox->SetIcon(ReadImage());
	else if(theInstruction=="TEXTCOLOR")
		mInputBox->SetTextColor(ReadColor());
	else if(theInstruction=="DISABLEDTEXTCOLOR")
		mInputBox->SetDisabledTextColor(ReadColor());
	else if(theInstruction=="SELCOLOR")
		mInputBox->SetSelectionColor(ReadSelectionColor());
	else if(theInstruction=="SELONFOCUS")
		mInputBox->SetSelectOnFocus(ReadBool());
	else if(theInstruction=="MAXINPUTHISTORY")
		mInputBox->SetMaxInputHistory(ReadInt());
	else if(theInstruction=="USEDISABLEDCOLORS")
		mInputBox->SetUseDisabledColors(ReadBool());
	else if(theInstruction=="CURSORBLINKRATE")
	{
		int on = ReadInt(), off; 
		if(!EndOfString())
		{
			EnsureComma();
			off = ReadInt();
		}
		else
			off = on;

		mInputBox->SetCursorBlinkRate(on,off);
	}
	else
		return ComponentConfig::HandleInstruction(theInstruction);

	return true;
}
