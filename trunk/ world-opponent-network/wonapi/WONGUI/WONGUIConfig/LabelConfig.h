#ifndef __WON_LABELCONFIG__
#define __WON_LABELCONFIG__
#include "ComponentConfig.h"
#include "WONGUI/Label.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LabelConfig : public ComponentConfig
{
protected:
	Label* mLabel;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	LabelConfig();
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new Label; }
	static ComponentConfig* CfgFactory() { return new LabelConfig; }
};

}; // namespace WONAPI

#endif