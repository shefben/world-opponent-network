
#include "TitleConfig.h"

using namespace WONAPI;
using namespace std;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TitleConfig::TitleConfig(bool alreadyInTitle /* = false */) :
	mAlreadyInTitle(alreadyInTitle)
{
	RegisterBoolValue("ForceCDKey",mForceCDKey,false);
	RegisterBoolValue("RequireCRC",mRequireCrc,false);
	RegisterBoolValue("AllowBetaKey",mAllowBetaKey,false);
	RegisterBoolValue("RequireCDKey",mRequireCDKey,false);
	RegisterBoolValue("FailToGuestLogin",mFailToGuest,true);
	RegisterBoolValue("AllowGuestLogin",mAllowGuest,false);
	RegisterBoolValue("ConcurrentUserLogin",mConcurrentUserLogin,true);
	RegisterBoolValue("ConcurrentCDKeyLogin",mConcurrentCDKeyLogin,false);
	RegisterBoolValue("GetUserData",mGetUserData,false);
	RegisterBoolValue("GetForgeableUserData",mGetForgeableUserData,false);
	RegisterBoolValue("CDKeyForInvite",mCDKeyForInvite,false);
	RegisterBoolValue("SaveSessionKey",mSaveSessionKey,false);
	RegisterBoolValue("GetKeyID",mGetKeyID,false);
	RegisterBoolValue("ASCIIAccountNames",mASCIIAccountNames,false);

	RegisterUShortValue("NetPort",mNetPort,15111);

	RegisterStringValue("Network",mNetwork);
	RegisterStringValue("ImagePath",mImagePath);
	RegisterStringValue("HashString",mHashString);
	RegisterStringValue("KeyPrefix",mKeyPrefix);

	RegisterWStringValue("AdminCommunityName",mAdminCommunityName,L"");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TitleConfig::PreParse(ConfigParser &theParser)
{
	TitleConfig *aParent = dynamic_cast<TitleConfig*>(GetParent());
	if(aParent!=NULL)
		CopyKeyValsFrom(aParent);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TitleConfig::HandleChildGroup(ConfigParser &theParser, const std::string& theTitleType)
{
	if(ConfigObject::HandleChildGroup(theParser, theTitleType))
		return true;

	if(theTitleType=="TITLEDEF")
	{
		if(mAlreadyInTitle)
		{
			theParser.AbortRead("You can't nest Title definitions!");
			return false;
		}

		TitleConfigPtr aTitleP = new TitleConfig(true);
		return ParseChild(theParser,aTitleP);
	}
	else if(theTitleType=="COMMUNITY")
	{
		CommunityConfigPtr aCommunityConfigP = new CommunityConfig;
		return ParseChild(theParser,aCommunityConfigP);
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TitleConfig::PostParse(ConfigParser &theParser)
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
string TitleConfig::ToString(bool includeName) const
{
	string aStr;
	if(includeName)
		aStr += GetName() +"\r\n";

	aStr += string("\tNetwork: ") + (mNetwork.empty() ? string("EMPTY") : mNetwork) + "\r\n";
	char aBuffer[20];
	sprintf(aBuffer, "%d", mNetPort);
	aStr += string("\tNetPort: ") + string(aBuffer) + "\r\n";
	aStr += string("\tForceCDKey: ") + string((mForceCDKey) ? "TRUE" : "FALSE") + "\r\n";

	aStr += string("\tImagePath: ") + (mImagePath.empty() ? string("EMPTY") : mImagePath) + "\r\n";
	aStr += string("\tHashString: ") + (mHashString.empty() ? string("EMPTY") : mHashString) + "\r\n";
	aStr += string("\tKeyPrefix: ") + (mKeyPrefix.empty() ? string("EMPTY") : mKeyPrefix) + "\r\n";

	aStr += string("\tRequireCrc: ") + string((mRequireCrc) ? "TRUE" : "FALSE") + "\r\n";
	aStr += string("\tAllowBetaKey: ") + string((mAllowBetaKey) ? "TRUE" : "FALSE") + "\r\n";
	aStr += string("\tRequireCDKey: ") + string((mRequireCDKey) ? "TRUE" : "FALSE") + "\r\n";

	aStr += string("\tFailToGuest: ") + string((mFailToGuest) ? "TRUE" : "FALSE") + "\r\n";
	aStr += string("\tAllowGuest: ") + string((mAllowGuest) ? "TRUE" : "FALSE") + "\r\n";
	
	aStr += string("\tConcurrentUserLogin: ") + string((mConcurrentUserLogin) ? "TRUE" : "FALSE") + "\r\n";
	aStr += string("\tConcurrentCDKeyLogin: ") + string((mConcurrentCDKeyLogin) ? "TRUE" : "FALSE") + "\r\n";
	
	aStr += string("\tGetUserData: ") + string((mGetUserData) ? "TRUE" : "FALSE") + "\r\n";
	aStr += string("\tGetForgeableUserData: ") + string((mGetForgeableUserData) ? "TRUE" : "FALSE") + "\r\n";
	
	aStr += string("\tCDKeyForInvite: ") + string((mCDKeyForInvite) ? "TRUE" : "FALSE") + "\r\n";
	aStr += string("\tSaveSessionKey: ") + string((mSaveSessionKey) ? "TRUE" : "FALSE") + "\r\n";
	aStr += string("\tGetKeyID: ") + string((mGetKeyID) ? "TRUE" : "FALSE") +"\r\n";
	aStr += string("\tASCIIAccountNames: ") + string((mASCIIAccountNames) ? "TRUE" : "FALSE") +"\r\n";

	return aStr;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CommunityConfig::CommunityConfig(bool alreadyInCommunity /* = false */) :
	TitleConfig(),
	mAlreadyInCommunity(alreadyInCommunity)
{
	RegisterIntValue("CommunityID",mCommunityID,0);
	RegisterIntListListValue("CertificateLifeSpan",mCertificateLifeSpanList,NULL,true);
	RegisterStringValue("NameLogin",mNameLoginStr,"Required");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CommunityConfig::CommunityConfig(const wstring& theCommunityName, unsigned long theCommunityID)
{
	Reset();
	mCommunityName = theCommunityName;
	mCommunityID = theCommunityID;
	mFailToGuest = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CommunityConfig::PreParse(ConfigParser &theParser)
{
	TitleConfig *aParent = dynamic_cast<TitleConfig*>(GetParent());
	if(aParent!=NULL)
		CopyKeyValsFrom(aParent);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CommunityConfig::HandleChildGroup(ConfigParser &theParser, const std::string& theCommunityType)
{
	if(ConfigObject::HandleChildGroup(theParser, theCommunityType))
		return true;

	if(theCommunityType=="TITLEDEF")
	{
		theParser.AbortRead("You can't have a title definition within a community!");
		return false;
	}
	else if(theCommunityType=="COMMUNITY")
	{
		if(mAlreadyInCommunity)
		{
			theParser.AbortRead("You can't nest communities!");
			return false;
		}

		CommunityConfigPtr aCommutiyP = new CommunityConfig(true);
		return ParseChild(theParser,aCommutiyP);
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CommunityConfig::PostParse(ConfigParser &theParser)
{
	string aNameLoginStr = StringToUpperCase(mNameLoginStr);
	if(aNameLoginStr == "REQUIRED")
		mNameLogin = NameLogin_Required;
	else if(aNameLoginStr == "OPTIONAL")
		mNameLogin = NameLogin_Optional;
	else if(aNameLoginStr == "NONE")
		mNameLogin = NameLogin_None;
	else
		mNameLogin = NameLogin_Unknown;
	
	if(mImagePath.empty())
	{
		mImagePath = "./";

		TitleConfig* anObjectP = dynamic_cast<TitleConfig*>(GetParent());
		if(anObjectP)
			mImagePath += anObjectP->GetName();
		else
			mImagePath += GetName();
	}

	if(mHashString.empty())
	{
		TitleConfig* anObjectP = dynamic_cast<TitleConfig*>(GetParent());
		if(anObjectP)
			mHashString = anObjectP->GetName();
		else
			mHashString = GetName();
	}

	long aTrustLevel, aDuration;
	mTrustLevelLifespanMap.clear();
	for(WONTypes::IntListList::const_iterator anItr = mCertificateLifeSpanList.begin(); anItr != mCertificateLifeSpanList.end(); ++anItr)
	{
		WONTypes::IntList::const_iterator anItr2 = anItr->begin();
		if(anItr2 != anItr->end())
		{
			aTrustLevel = *anItr2;
			++anItr2;
			if(anItr2 != anItr->end())
			{
				aDuration = *anItr2;
				mTrustLevelLifespanMap[aTrustLevel] = aDuration;
			}
		}
	}

	if (mCommunityName.empty())
		mCommunityName = StringToWString(GetName());

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
string CommunityConfig::ToString(void) const
{
	string aStr = ((mCommunityName.empty()) ? GetName() : WStringToString(mCommunityName)) + "\r\n";

	char aBuffer[25];
	sprintf(aBuffer, "%d", mCommunityID);
	aStr += string("CommunityID: ") + string(aBuffer) + "\r\n";
	aStr += TitleConfig::ToString(false);

	// Trust level lifespan map
	aStr += string("\tNameLogin: ") + mNameLoginStr + "\r\n";
	aStr += "\tTrust Level Lifespans: [";
	if(mTrustLevelLifespanMap.empty())
		aStr += "EMPTY";
	else{
		bool isFirst = true;
		for(TRUST_LEVEL_LIFESPAN_MAP_CITR anItr = mTrustLevelLifespanMap.begin(); anItr != mTrustLevelLifespanMap.end(); ++anItr)
		{
			if(isFirst)
				isFirst = false;
			else
				aStr += ",";

			sprintf(aBuffer, "(%d,%d)", anItr->first, anItr->second);
			aStr += string(aBuffer);
		}
	}
	aStr += "]\r\n";

	return aStr;
}

