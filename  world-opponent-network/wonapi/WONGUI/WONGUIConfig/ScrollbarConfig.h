#ifndef __WON_ScrollbarCONFIG__
#define __WON_ScrollbarCONFIG__
#include "ContainerConfig.h"
#include "WONGUI/Scrollbar.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ScrollbarConfig : public ContainerConfig
{
protected:
	Scrollbar* mScrollbar;

	virtual bool HandleInstruction(const std::string &theInstruction);
	virtual bool GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, Component* &theComponent);

public:
	ScrollbarConfig();
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new Scrollbar; }
	static ComponentConfig* CfgFactory() { return new ScrollbarConfig; }
};

}; // namespace WONAPI

#endif