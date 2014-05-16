#ifndef __WON_ComboBoxCONFIG__
#define __WON_ComboBoxCONFIG__
#include "ContainerConfig.h"
#include "WONGUI/ComboBox.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ComboBoxConfig : public ContainerConfig
{
protected:
	ComboBox* mComboBox;

	virtual bool HandleInstruction(const std::string &theInstruction);
	virtual bool GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, Component* &theComponent);

public:
	ComboBoxConfig();
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new ComboBox; }
	static ComponentConfig* CfgFactory() { return new ComboBoxConfig; }
};

}; // namespace WONAPI

#endif