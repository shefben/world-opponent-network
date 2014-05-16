#ifndef __WON_TREECTRLCONFIG__
#define __WON_TREECTRLCONFIG__
#include "ListBoxConfig.h"
#include "WONGUI/TreeCtrl.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TreeAreaConfig : public ListAreaConfig
{
protected:
	TreeArea* mTreeArea;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { mTreeArea = (TreeArea*)theComponent; ListAreaConfig::SetComponent(theComponent); }

	static Component* CompFactory() { return new TreeArea; }
	static ComponentConfig* CfgFactory() { return new TreeAreaConfig; }
};


}; // namespace WONAPI

#endif