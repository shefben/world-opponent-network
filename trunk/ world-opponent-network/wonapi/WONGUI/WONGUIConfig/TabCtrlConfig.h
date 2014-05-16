#ifndef __WON_TabCtrlCONFIG__
#define __WON_TabCtrlCONFIG__
#include "ContainerConfig.h"
#include "WONGUI/TabCtrl.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TabCtrlConfig : public ContainerConfig
{
protected:
	TabCtrl* mTabCtrl;

	virtual bool HandleInstruction(const std::string &theInstruction);
	virtual bool GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, Component* &theComponent);

public:
	TabCtrlConfig();
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new TabCtrl; }
	static ComponentConfig* CfgFactory() { return new TabCtrlConfig; }
};

}; // namespace WONAPI

#endif