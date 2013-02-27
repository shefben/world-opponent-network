#ifndef __WON_SETPROFILEOP_H__
#define __WON_SETPROFILEOP_H__
#include "WONShared.h"

#include "WONServer/ServerRequestOp.h"
#include "ProfileData.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SetProfileOp : public ServerRequestOp
{
private:
	ProfileDataPtr mProfileData;
	unsigned long mAPIErrorCode;
	std::string mErrorString;
	std::wstring mNewUsername;
	std::wstring mNewPassword;

	void Init();

protected:
	virtual WONStatus GetNextRequest();
	virtual WONStatus CheckResponse();

public:
	SetProfileOp(ServerContext *theProfileContext);
	SetProfileOp(const IPAddr &theAddr);

	void SetProfileData(ProfileData *theData) { mProfileData = theData; }

	void AssociateNewUsernamePassword(const std::wstring &theUsername, const std::wstring &thePassword);
	void ClearNewUsernamePassword(); 
};

typedef SmartPtr<SetProfileOp> SetProfileOpPtr;


}; // namespace WONAPI

#endif
