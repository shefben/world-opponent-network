#ifndef __WON_MultiListBoxCONFIG__
#define __WON_MultiListBoxCONFIG__
#include "ListBoxConfig.h"
#include "WONGUI/MultiListBox.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MultiListAreaConfig : public ListAreaConfig
{
protected:
	MultiListArea* mMultiListArea;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new MultiListArea; }
	static ComponentConfig* CfgFactory() { return new MultiListAreaConfig; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MultiListHeaderConfig : public ContainerConfig
{
protected:
	MultiListHeader* mHeader;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { mHeader = (MultiListHeader*)theComponent; ContainerConfig::SetComponent(theComponent); }

	static Component* CompFactory() { return new MultiListHeader; }
	static ComponentConfig* CfgFactory() { return new MultiListHeaderConfig; }
};

}; // namespace WONAPI

#endif