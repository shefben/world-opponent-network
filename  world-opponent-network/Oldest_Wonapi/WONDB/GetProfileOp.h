#ifndef __WON_GetProfileOP_H__
#define __WON_GetProfileOP_H__
#include "WONShared.h"

#include "WONServer/ServerRequestOp.h"
#include "ProfileData.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GetProfileOp : public ServerRequestOp
{
private:
	ProfileFieldSet mFieldSet;
	ProfileDataPtr mProfileData;
	unsigned long mAPIErrorCode;
	std::string mErrorString;

	void Init();

protected:
	virtual WONStatus GetNextRequest();
	virtual WONStatus CheckResponse();
	virtual void Reset();


public:
	GetProfileOp(ServerContext *theProfileContext);
	GetProfileOp(const IPAddr &theAddr);

	void AddField(ProfileField theField) { mFieldSet.insert(theField); }
	void AddAllFields();
	void RemoveField(ProfileField theField) { mFieldSet.erase(theField); }
	void ClearFields() { mFieldSet.clear(); }

	ProfileData* GetProfileData() { return mProfileData; }
};

typedef SmartPtr<GetProfileOp> GetProfileOpPtr;

}; // namespace WONAPI

#endif
