#ifndef __WON_BUTTONCONFIG__
#define __WON_BUTTONCONFIG__
#include "ComponentConfig.h"
#include "WONGUI/Button.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ButtonConfig : public ComponentConfig
{
protected:
	Button* mButton;

	virtual bool HandleInstruction(const std::string &theInstruction);
	void HandleFitText();
	void HandleButtonFlags();

public:
	ButtonConfig();
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new Button; }
	static ComponentConfig* CfgFactory() { return new ButtonConfig; }
};

}; // namespace WONAPI

#endif