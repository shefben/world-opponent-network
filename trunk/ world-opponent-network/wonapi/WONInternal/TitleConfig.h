
#ifndef _TITLE_CONFIG_H_
#define _TITLE_CONFIG_H_

#include "WONAPI.h"
#include "WONConfig/ConfigObject.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TitleConfig : public ConfigObject
{
public:
	TitleConfig(bool alreadyInTitle = false);

	static ConfigObject* ConfigFactory() { return new TitleConfig(); }
	virtual bool HandleChildGroup(ConfigParser &theParser, const std::string& theTitleType);
	virtual bool PreParse(ConfigParser &theParser);
	virtual bool PostParse(ConfigParser &theParser);
	std::string ToString(bool includeName) const;

	bool mAlreadyInTitle;

	std::string mNetwork;        // Title's production network string (used in <net>.mX.sierra.com)
	unsigned short mNetPort;     // Title's network port for master query
	bool mForceCDKey;            // Force CDKey generation for this title, even if no communities require keys

	std::string mImagePath;      // Path for binary images and key files for communties.
	std::string mHashString;     // String used in lightweight key check for communities
	std::string mKeyPrefix;      // String used searching for key files (key files have this prefix.)

	bool mRequireCrc;            // Require CRC check for this community
	bool mAllowBetaKey;          // Allow Beta keys for this community
	bool mRequireCDKey;          // Require validity of keys be checked, and restrict multiple logins,...

	bool mFailToGuest;           // Convert to guest login in case of DB failure
	bool mAllowGuest;            // Allow guest login on this community.

	bool mConcurrentUserLogin;   // Allow same userID to be logged in multiple times.  (Conflicts with Guest stuff.)
	bool mConcurrentCDKeyLogin;  // Allow same CD key to be logged in multiple times.

	bool mGetUserData;           // Tribes2 legacy (Not used)
	bool mGetForgeableUserData;  // Tribes2 legacy (Not used)

	bool mCDKeyForInvite;        // A valid CD Key gets user into an invite only community.
	bool mSaveSessionKey;        // Save the session key and expiration date in the database. Tribes2 legacy (Not used)
	bool mGetKeyID;              // Get unique key id and place in certificate.
	bool mASCIIAccountNames;     // Account names only allow space thru tilde (32 thru 126)

	std::wstring  mAdminCommunityName;	// Community Name used for Administration. (Patch Server etc)
};
typedef WONAPI::SmartPtr<TitleConfig> TitleConfigPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class CommunityConfig : public TitleConfig
{
public:
	enum NameLogin
	{
		NameLogin_None		= 0,
		NameLogin_Optional	= 1,
		NameLogin_Required	= 2,
		NameLogin_Unknown	= 3
	};

	CommunityConfig(bool alreadyInCommunity = false);
	CommunityConfig(const std::wstring& theCommunityName, unsigned long theCommunityID); // For dummy versions in authentication

	static ConfigObject* ConfigFactory() { return new CommunityConfig(); }
	virtual bool HandleChildGroup(ConfigParser &theParser, const std::string& theCommunityType);
	virtual bool PreParse(ConfigParser &theParser);
	virtual bool PostParse(ConfigParser &theParser);
	std::string ToString(void) const;

	bool mAlreadyInCommunity;

	int          mCommunityID;
	std::wstring mCommunityName;
	std::string  mNameLoginStr;
	NameLogin    mNameLogin;		 // Name and password requirements on this community. (Required, Optional, None)

	typedef std::map<long, long>                     TRUST_LEVEL_LIFESPAN_MAP;
	typedef TRUST_LEVEL_LIFESPAN_MAP::iterator       TRUST_LEVEL_LIFESPAN_MAP_ITR;
	typedef TRUST_LEVEL_LIFESPAN_MAP::const_iterator TRUST_LEVEL_LIFESPAN_MAP_CITR;
	TRUST_LEVEL_LIFESPAN_MAP mTrustLevelLifespanMap;

	WONTypes::IntListList mCertificateLifeSpanList;
};
typedef WONAPI::SmartPtr<CommunityConfig> CommunityConfigPtr;
typedef WONAPI::ConstSmartPtr<CommunityConfig> ConstCommunityConfigPtr;

};

#endif