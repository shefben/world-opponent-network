#include "DialogConfig.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DialogConfig::SetComponent(Component *theComponent)
{
	ContainerConfig::SetComponent(theComponent);
	mDialog = (Dialog*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool DialogConfig::HandleInstruction(const std::string& theInstruction)
{
	if(theInstruction=="ALLOWDRAG")
		mDialog->SetAllowDrag(ReadBool());
	else if(theInstruction=="KEEPINROOT")
		mDialog->SetKeepInRoot(ReadBool());
	else if(theInstruction=="ENDONESCAPE")
		mDialog->SetEndOnEscape(ReadInt());
	else if(theInstruction=="DONTENDONESCAPE")
		mDialog->SetDontEndOnEscape();
	else if(theInstruction=="DEFAULTBUTTON")
		mDialog->SetDefaultButton(SafeConfigGetComponent<Button*>(ReadComponent(),"Button"));
	else
		return ContainerConfig::HandleInstruction(theInstruction);

	return true;
}
