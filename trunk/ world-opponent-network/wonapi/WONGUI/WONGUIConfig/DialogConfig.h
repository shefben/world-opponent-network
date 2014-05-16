#ifndef __WON_DIALOGCONFIG_H__
#define __WON_DIALOGCONFIG_H__

#include "ContainerConfig.h"
#include "WONGUI/Dialog.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class DialogConfig : public ContainerConfig
{
protected:
	Dialog* mDialog;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new Dialog; }
	static ComponentConfig* CfgFactory() { return new DialogConfig; }
};
typedef SmartPtr<DialogConfig> DialogConfigPtr;

}; // namespace WONAPI

#endif