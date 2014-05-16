//----------------------------------------------------------------------------------
// ResourceManager.h
//----------------------------------------------------------------------------------
#ifndef __ResourceManager_H__
#define __ResourceManager_H__

#include "UpdateResource.h"

namespace WONAPI
{

//----------------------------------------------------------------------------------
// InfoDlg.
//----------------------------------------------------------------------------------
class ResourceManager
{
public:
	ResourceManager(void);
	~ResourceManager(void);

	void AppendInfoString(GUIString& sInfo, UpdateResourceId nId);
	GUIString BuildInfoString(UpdateResourceId nFirstId);
	GUIString GetFinishedString(const GUIString& sInfo);

	void Init(void);
};
typedef SmartPtr<ResourceManager> ResourceManagerPtr;

};


#endif
