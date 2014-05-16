#ifndef __GETACCOUNTFROMCDKEYOP_H__
#define __GETACCOUNTFROMCDKEYOP_H__
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "WONDB/DBProxyOp.h"
#include "WONDB/ProfileData.h"
#include <list>

namespace WONAPI 
{


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// This Op is called by the client to retrieve account information
// using a valid product cd key and community name to identify the account
class GetAccountFromCDKeyOp: public DBProxyOp
{
	// Attributes
protected:
	std::string		mCDKey;		// [in]
	unsigned long	mCommunity; // [in]

	std::wstring	mUserName;	// [out]
	std::wstring	mPassword;	// [out]
	std::string		mEmail;		// [out]
	std::string		mLastLogin; // [out]
	std::string		mActivated; // [out]
	

	ProfileDataPtr	mProfileData;

	// Overrides
protected:
	enum MSGTYPE { MSGTYPE = 10 };				// DO NOT MODIFY (MUST MATCH WITH SERVER)
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void RunHook();						// Send - For Extended Pack
	virtual void Reset();

public:
	void SetCDKey(const char* theCDKey);

	std::wstring& GetUserName() { return mUserName; }
	std::wstring& GetPassword() { return mPassword; }
	std::string&  GetEmail()	{ return mEmail; }
	std::string&  GetLastLogin(){ return mLastLogin; }
	std::string&  GetActivated(){ return mActivated; }
	std::string&  GetCDKey()	{ return mCDKey; }

	void SetProfileData(ProfileData* theData) { mProfileData = theData; }
	ProfileData* GetProfileData() { return mProfileData.get(); }
	// Constructor
public:
	GetAccountFromCDKeyOp(ServerContext* theContext, std::string theCDKey, unsigned long theCommunity);

	// Destructor
protected:
	virtual ~GetAccountFromCDKeyOp() {}		// Safety Lock -- Enforces SmartPtr Use.


};
typedef SmartPtr<GetAccountFromCDKeyOp> GetAccountFromCDKeyOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
} // WONAPI


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __GETACCOUNTFROMCDKEYOP_H__
