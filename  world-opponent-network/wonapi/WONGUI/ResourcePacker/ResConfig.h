#ifndef __WON_RESCONFIG_H__
#define __WON_RESCONFIG_H__

#include "WONCommon/StringUtil.h"
#include "WONConfig/ConfigObject.h"
#include "WONGUI/ResourceCollection.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ResConfig : public ConfigObject
{
protected:
	ResourceCollectionWriter &mResourceCollection;
	typedef std::map<std::string,ResourceType,StringLessNoCase> ResourceTypeMap;
	ResourceTypeMap mTypeMap;

	virtual bool HandleKeyVal(ConfigParser &theParser, const std::string& theKey, StringParser &theVal);

public:
	ResConfig(ResourceCollectionWriter &theWriter);
};
typedef SmartPtr<ResConfig> ResConfigPtr;

};

#endif