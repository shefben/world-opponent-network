#ifndef __WON_FONTCONFIG_H__
#define __WON_FONTCONFIG_H__

#include "ComponentConfig.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FontConfig_Init(ResourceConfigTable *theTable);

class FontConfig : public ComponentConfig
{
protected:
	FontPtr mFont;
	FontDescriptor mDescriptor;
	bool mNeedRecalcFont;
	
	virtual bool GetNamedIntValue(const std::string &theName, int &theValue);
	virtual bool HandleInstruction(const std::string &theInstruction);
	virtual bool PostParse(ConfigParser &theParser);

public:
	FontConfig();

	Font* GetFont();
	void SetFont(Font *theFont);

	static ComponentConfig* CfgFactory() { return new FontConfig; }
	static Component* CompFactory() { return NULL; }
};
typedef SmartPtr<FontConfig> FontConfigPtr;

} // namespace WONAPI

#endif