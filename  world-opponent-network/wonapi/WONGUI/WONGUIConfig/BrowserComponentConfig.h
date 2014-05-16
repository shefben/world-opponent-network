#ifndef __WON_BrowserComponentCONFIG__
#define __WON_BrowserComponentCONFIG__
#include "ContainerConfig.h"
#include "WONGUI/BrowserComponent.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class BrowserComponentConfig : public ContainerConfig
{
protected:
	BrowserComponent* mBrowserComponent;

	virtual bool HandleInstruction(const std::string &theInstruction);
	virtual bool GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, Component* &theComponent);

public:
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new BrowserComponent; }
	static ComponentConfig* CfgFactory() { return new BrowserComponentConfig; }

	static void Init(ResourceConfigTable *theTable);
};

}; // namespace WONAPI

#endif