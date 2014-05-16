#ifndef __WON_InputBoxConfig_H__
#define __WON_InputBoxConfig_H__

#include "ComponentConfig.h"
#include "WONGUI/InputBox.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class InputBoxConfig : public ComponentConfig
{
protected:
	InputBox* mInputBox;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new InputBox; }
	static ComponentConfig* CfgFactory() { return new InputBoxConfig; }

};
typedef SmartPtr<InputBoxConfig> InputBoxConfigPtr;

}; // namespace WONAPI

#endif