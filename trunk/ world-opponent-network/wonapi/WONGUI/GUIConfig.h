#ifndef __WON_GUICONFIG_H__
#define __WON_GUICONFIG_H__
#include "WONCommon/SmartPtr.h"
#include "WONGUI/GUITypes.h"

#include <string>

namespace WONAPI
{

class ComponentConfig;
class Component;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct ComponentConfigInit : public ComponentInit
{
	ComponentConfig *mConfig;

	ComponentConfigInit(ComponentConfig *theConfig);
};
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Component* ComponentConfig_GetChildComponent(ComponentConfig *theConfig, const char *theName);
ComponentConfig* ComponentConfig_GetChildConfig(ComponentConfig *theConfig, const char *theName);
void ComponentConfig_ThrowError(const std::string &theDescription, const char *theFileName, int theLineNum);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template<class T>
void WONComponentConfig_GetImpl(ComponentConfig *theConfig, SmartPtr<T> &theComp, const char *theName, const char *theFile, int theLineNum)
{
	T *aComponent = dynamic_cast<T*>(ComponentConfig_GetChildComponent(theConfig,theName));
	if(aComponent==NULL)
		ComponentConfig_ThrowError(theName,theFile,theLineNum);
	
	theComp = aComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template<class T>
void WONComponentConfig_GetWithCtrlIdImpl(ComponentConfig *theConfig, SmartPtr<T> &theComp, const char *theName, int theControlId, const char *theFile, int theLineNum)
{
	WONComponentConfig_GetImpl(theConfig, theComp,theName,theFile,theLineNum);
	theComp->SetControlId(theControlId);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ComponentConfig* WONComponentConfig_GetConfigImpl(ComponentConfig *theConfig, const char *theName, const char *theFile, int theLineNum);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#define WONComponentConfig_Get(theConfig, theComp, theName)	\
	WONComponentConfig_GetImpl(theConfig, theComp, theName, __FILE__, __LINE__)

#define WONComponentConfig_GetWithCtrlId(theConfig, theComp, theName, theControlId)	\
	WONComponentConfig_GetWithCtrlIdImpl(theConfig, theComp, theName, theControlId, __FILE__, __LINE__)

#define WONComponentConfig_GetConfig(theConfig, theName) \
	WONComponentConfig_GetConfigImpl(theConfig, theName, __FILE__, __LINE__)

#define WONComponentConfig_Error(theError) \
	ComponentConfig_ThrowError(theError, __FILE__, __LINE__)


} // namespace WONAPI


#endif