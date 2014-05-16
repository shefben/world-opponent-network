#include "BrowserComponentConfig.h"
#include "ResourceConfig.h"
#include "ScrollAreaConfig.h"
#include "WONGUI/HTMLDocumentGen.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponentConfig::SetComponent(Component *theComponent)
{
	ContainerConfig::SetComponent(theComponent);
	mBrowserComponent = (BrowserComponent*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool BrowserComponentConfig::GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, Component* &theComponent)
{
	if(theName=="HTMLVIEW")
	{
		theConfig = new ScrollAreaConfig;
		theComponent = mBrowserComponent->GetHTMLView();
	}
	else
		return ContainerConfig::GetPredefinedComponent(theName,theConfig,theComponent);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool BrowserComponentConfig::HandleInstruction(const std::string& theInstruction)
{
	if(theInstruction=="TRANSPARENT")
	{
		bool transparent = ReadBool();
		mBrowserComponent->GetHTMLView()->SetComponentFlags(ComponentFlag_GrabBG,transparent);
		if(transparent)
			mBrowserComponent->SetBackground(COLOR_INVALID);
		else
			mBrowserComponent->RemoveBackground();
	}
	else if(theInstruction=="BACKGROUND")
		mBrowserComponent->SetBackground(ReadBackground());
	else if(theInstruction=="SCROLLER")
		mBrowserComponent->SetScroller(SafeConfigGetComponent<Scroller*>(ReadComponent(),"Scroller"));
	else if(theInstruction=="FONTSIZE")
	{
		HTMLFontSize aSize;
		GUIString aSizeStr = ReadString();
		if(aSizeStr.compareNoCase("Smallest")==0) aSize = HTMLFontSize_Smallest;
		else if(aSizeStr.compareNoCase("Small")==0) aSize = HTMLFontSize_Small;
		else if(aSizeStr.compareNoCase("Medium")==0) aSize = HTMLFontSize_Medium;
		else if(aSizeStr.compareNoCase("Large")==0) aSize = HTMLFontSize_Large;
		else if(aSizeStr.compareNoCase("Largest")==0) aSize = HTMLFontSize_Largest;
		else
			throw ConfigObjectException("Invalid font size: " + (std::string)aSizeStr);

		mBrowserComponent->SetFontSize(aSize);
	}
	else
		return ContainerConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponentConfig::Init(ResourceConfigTable *theTable)
{
	theTable->AddAllocator("BrowserComponent",CfgFactory,CompFactory); 
}
