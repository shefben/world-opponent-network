#include "TabCtrlConfig.h"
#include "ButtonConfig.h"
#include "ScrollAreaConfig.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TabCtrlConfig::TabCtrlConfig()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrlConfig::SetComponent(Component *theComponent)
{
	ContainerConfig::SetComponent(theComponent);
	mTabCtrl = (TabCtrl*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TabCtrlConfig::HandleInstruction(const std::string& theInstruction)
{
	if(theInstruction=="ADDTAB")
	{
		Button *aTabButton = SafeConfigGetComponent<Button*>(ReadComponent(),"Button"); EnsureComma();
		Container *aTabContainer = SafeConfigGetComponent<Container*>(ReadComponent(),"Container");
		mTabCtrl->AddTab(aTabButton,aTabContainer);
	}
	else if(theInstruction=="TABBARONRIGHT")
		mTabCtrl->SetTabBarOnRight(ReadBool());
	else if(theInstruction=="ARROWSONLEFT")
		mTabCtrl->SetArrowsOnLeft(ReadBool());
	else if(theInstruction=="TABBARHEIGHT")
		mTabCtrl->SetTabBarHeight(ReadInt());
	else if(theInstruction=="TABOVERLAP")
		mTabCtrl->SetTabOverlap(ReadInt());
	else if(theInstruction=="TABBAROVERLAP")
		mTabCtrl->SetTabBarOverlap(ReadInt());
	else if(theInstruction=="TABVERTOFFSET")
		mTabCtrl->SetTabVertOffset(ReadInt());
	else if(theInstruction=="TABHORZOFFSET")
		mTabCtrl->SetTabHorzOffset(ReadInt());
	else
		return ContainerConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TabCtrlConfig::GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, Component* &theComponent)
{
	if(theName=="CONTAINERLAYOUT")
	{
		theConfig = new ComponentConfig;
		theComponent = mTabCtrl->GetContainerLayout();
	}
	else 
		return ContainerConfig::GetPredefinedComponent(theName,theConfig,theComponent);

	return true;
}
