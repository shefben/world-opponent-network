#ifndef __GETACCOUNTFROMCDKEYEXOP_H__
#define __GETACCOUNTFROMCDKEYEXOP_H__
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "WONInternal/GetAccountFromCDKeyOp.h"

namespace WONAPI 
{


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// This Op is called by the client to retrieve account information
// using a valid product cd key and community name to identify the account
class GetAccountFromCDKeyExOp: public GetAccountFromCDKeyOp
{
	// Attributes
public:
	enum ACTIONFLAG { RESET = 0, BAN = 1 };

protected:
	/* Inherited
	std::string		mCDKey;		// [in]
	unsigned long	mCommunity; // [in]

	std::wstring	mUserName;	// [out]
	std::wstring	mPassword;	// [out]
	std::string		mEmail;		// [out]
	std::string		mLastLogin; // [out]
	std::string		mActivated; // [out]

	ProfileDataPtr	mProfileData;
	*/
	ACTIONFLAG		mActionFlag;

	// Overrides
protected:
	enum MSGTYPE { MSGTYPE = 12 };				// DO NOT MODIFY (MUST MATCH WITH SERVER)
	//virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void RunHook();						// Send - For Extended Pack
	//virtual void Reset();

public:
	/* Inherited
	void SetCDKey(const char* theCDKey);
	std::string& GetCDKey() { return mCDKey; };

	std::wstring& GetUserName() { return mUserName; }
	std::wstring& GetPassword() { return mPassword; }
	std::string&  GetEmail()	{ return mEmail; }
	std::string&  GetLastLogin(){ return mLastLogin; }
	std::string&  GetActivated(){ return mActivated; }

	void SetProfileData(ProfileData* theData) { mProfileData = theData; }
	ProfileData* GetProfileData() { return mProfileData.get(); }
	*/

	void SetActionFlag(ACTIONFLAG actionFlag) { mActionFlag = actionFlag; }
	ACTIONFLAG GetActionFlag() { return mActionFlag; }
	static const char* GetActionString(ACTIONFLAG actionFlag);

	// Constructor
public:
	GetAccountFromCDKeyExOp(ServerContext* theContext, std::string theCDKey, unsigned long theCommunity);

	// Destructor
protected:
	virtual ~GetAccountFromCDKeyExOp() {}		// Safety Lock -- Enforces SmartPtr Use.


};
typedef SmartPtr<GetAccountFromCDKeyExOp> GetAccountFromCDKeyOpExPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
} // WONAPI


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __GETACCOUNTFROMCDKEYEXOP_H__
