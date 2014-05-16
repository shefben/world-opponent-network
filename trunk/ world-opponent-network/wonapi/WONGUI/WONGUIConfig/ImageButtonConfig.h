#ifndef __WON_IMAGEBUTTONCONFIG__
#define __WON_IMAGEBUTTONCONFIG__
#include "ComponentConfig.h"
#include "WONGUI/ImageButton.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ImageButtonConfig : public ComponentConfig
{
protected:
	ImageButton* mImageButton;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	ImageButtonConfig();
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new ImageButton; }
	static ComponentConfig* CfgFactory() { return new ImageButtonConfig; }
};

}; // namespace WONAPI

#endif