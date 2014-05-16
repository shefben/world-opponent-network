#ifndef __WON_TEXTBOXCONFIG__
#define __WON_TEXTBOXCONFIG__
#include "ScrollAreaConfig.h"
#include "WONGUI/TextBox.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextAreaConfig : public ScrollAreaConfig
{
protected:
	TextArea* mTextArea;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new TextArea; }
	static ComponentConfig* CfgFactory() { return new TextAreaConfig; }
};

}; // namespace WONAPI

#endif