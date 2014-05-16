#include "WONGUI/GUIConfig.h"

#include "ComponentConfig.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ComponentConfigInit::ComponentConfigInit(ComponentConfig *theConfig) : 
	ComponentInit(ComponentInitType_ComponentConfig), mConfig(theConfig) 
{
	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Component* WONAPI::ComponentConfig_GetChildComponent(ComponentConfig *theConfig, const char *theName)
{
	return theConfig->GetChildComponent(theName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ComponentConfig* WONAPI::ComponentConfig_GetChildConfig(ComponentConfig *theConfig, const char *theName)
{
	return theConfig->GetChildConfig(theName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ComponentConfig* WONAPI::WONComponentConfig_GetConfigImpl(ComponentConfig *theConfig, const char *theName, const char *theFile, int theLineNum)
{
	ComponentConfig *aConfig = 	ComponentConfig_GetChildConfig(theConfig,theName);
	if(aConfig==NULL)
		ComponentConfig_ThrowError(theName,theFile,theLineNum);

	return aConfig;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::ComponentConfig_ThrowError(const std::string &theDescription, const char *theFileName, int theLineNum)
{
	char aBuf[512];
	sprintf(aBuf,"InitComponent error: %s (%s:%d)",theDescription.c_str(), theFileName, theLineNum);

	throw ConfigObjectException(aBuf);
}
