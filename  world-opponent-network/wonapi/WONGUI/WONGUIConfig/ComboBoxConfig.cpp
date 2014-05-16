#include "ComboBoxConfig.h"
#include "InputBoxConfig.h"
#include "ListBoxConfig.h"


using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ComboBoxConfig::ComboBoxConfig()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComboBoxConfig::SetComponent(Component *theComponent)
{
	ContainerConfig::SetComponent(theComponent);
	mComboBox = (ComboBox*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComboBoxConfig::GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, Component* &theComponent)
{
	if(theName=="LISTAREA")
	{
		theConfig = ListAreaConfig::CfgFactory();
		theComponent = mComboBox->GetListArea();
	}
	else if(theName=="INPUTBOX")
	{
		theConfig = InputBoxConfig::CfgFactory();
		theComponent = mComboBox->GetInputBox();
	}
	else 
		return ContainerConfig::GetPredefinedComponent(theName,theConfig,theComponent);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComboBoxConfig::HandleInstruction(const std::string& theInstruction)
{
	if(theInstruction=="LISTBOXWIDTH")
		mComboBox->SetListBoxWidth(ReadInt());
	else if(theInstruction=="LISTBOXHEIGHT")
		mComboBox->SetListBoxHeight(ReadInt());
	else if(theInstruction=="LISTBOXSIZE")
	{
		int w = ReadInt();
		int h = ReadInt();
		mComboBox->SetListBoxDimensions(w,h);
	}
	else if(theInstruction=="TEXT")
		mComboBox->SetText(ReadString());
	else if(theInstruction=="TEXTITEM")
	{
		while(!EndOfString())
		{
			mComboBox->InsertString(ReadString());
			if(!EndOfString())
				EnsureComma();
		}
	}
	else if(theInstruction=="SELITEM")
		mComboBox->SetSelItem(ReadInt());
	else if(theInstruction=="NOTYPE")
		mComboBox->SetNoType(ReadBool());
	else if(theInstruction=="MAXCHARS")
		mComboBox->GetInputBox()->SetMaxChars(ReadInt());
	else if(theInstruction=="SHOWBUTTONWHENDISABLED")
		mComboBox->SetShowButtonWhenDisabled(ReadBool());
	else
		return ContainerConfig::HandleInstruction(theInstruction);

	return true;
}

