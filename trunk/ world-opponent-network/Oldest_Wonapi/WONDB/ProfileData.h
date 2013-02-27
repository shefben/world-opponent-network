#ifndef __WON_PROFILEDATA_H__
#define __WON_PROFILEDATA_H__
#include "WONShared.h"
#include "WONStatus.h"
#include "WONCommon/SmartPtr.h"
#include "WONCommon/ByteBuffer.h"
#include "WONCommon/ReadBuffer.h"
#include "WONCommon/BiMap.h"
#include <map>
#include <set>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum ProfileField
{
	ProfileField_NewsLetterSub						= 1,
	ProfileField_CustomAction						= 2, // perform custom DB action
	ProfileField_BirthDate							= 3, // birth date

// The following fields are not implemented yet in the DBProxy
	ProfileField_Gender								= 0,
//	ProfileField_BirthDate							= 1,
	ProfileField_FirstName							= 2,
	ProfileField_LastName							= 3,
	ProfileField_MiddleName							= 4,
	ProfileField_CompanyName						= 5,
	ProfileField_Address1							= 6,
	ProfileField_Address2							= 7,
	ProfileField_City								= 8,
	ProfileField_PostalCode							= 9,
	ProfileField_State								= 10,
	ProfileField_Phone1								= 11,
	ProfileField_Phone1Ext							= 12,
	ProfileField_Phone2								= 13,
	ProfileField_Phone2Ext							= 14,
	ProfileField_Email1								= 15,	
	ProfileField_Email2								= 16,
	ProfileField_CreationDate						= 17,
	ProfileField_ModifiedDate						= 18,
	ProfileField_AgeRange							= 19,
	ProfileField_Country							= 20,
	ProfileField_Source								= 21,
	ProfileField_NewsletterSubs						= 22,
	ProfileField_Community							= 25,
	ProfileField_NumFields							= 26
};

struct ProfileFieldInfo
{
	ByteBufferPtr mData;
	std::string mErrorString;
	WONStatus mErrorStatus;

	ProfileFieldInfo() : mErrorStatus(WS_None) {}
	ProfileFieldInfo(const ByteBuffer* theData) : mData(theData), mErrorStatus(WS_None) {}
};

enum ProfileGender
{
	ProfileGender_Male,
	ProfileGender_Female
};

typedef std::set<ProfileField> ProfileFieldSet;
typedef std::map<ProfileField, ProfileFieldInfo> ProfileFieldMap;
typedef std::map<unsigned short, std::string> ProfileCountryMap;
typedef std::map<unsigned short, std::string> ProfileAgeRangeMap;
typedef std::map<std::wstring, std::string> ProfileStateAbbrevMap;
typedef std::map<unsigned short, bool> ProfileNewsletterSubMap;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ProfileData : public RefCount
{
private:
	ProfileFieldMap mFieldMap;	// obsolete legacy member

	// Birthdate
	// [byte]  Month (1-12)
	// [byte]  Day   (1-31)
	// [short] Year  (4-digit 1999 NOT 99)
	unsigned char  mBirthMonth;
	unsigned char  mBirthDay;
	unsigned short mBirthYear;


	// Newsletter subscriptions
	ProfileNewsletterSubMap mNewsletterSubMap;

	// Custom actions
	unsigned short mCustomActionId;
	ByteBufferPtr mCustomActionData;

	static bool mLoadedData;
	static void LoadData();

	static ProfileCountryMap mCountryMap;
	static ProfileAgeRangeMap mAgeRangeMap;
	static ProfileStateAbbrevMap mStateAbbrevMap;

protected:
	virtual ~ProfileData();

public:	
	ProfileData();

	// Newsletter Subscriptions
	void AddNewsletterSub(unsigned short theId, bool subscribe = false);
	bool GetNewsletterSub(unsigned short theId);
	void ClearNewsletterSub(unsigned short theId);
	void ClearNewsletterSubs();

	// Custom Actions
	void SetCustomAction(unsigned short theActionId, const ByteBuffer *theActionData);
	void ClearCustomAction();
	unsigned short	  GetCustomActionType() { return mCustomActionId;   }
	const ByteBuffer* GetCustomActionData() { return mCustomActionData; }

	void SetBirthDate(const unsigned char theMonth, const unsigned char theDay, const unsigned short theYear);
	unsigned long GetBirthDate();

	void PackRequestData(WriteBuffer &theMsg, bool isSet);
	void UnpackGetData(ReadBuffer &theMsg);

// Old methods.  Not used yet in the DBProxy
public:	
	static const ProfileAgeRangeMap& GetAgeRangeMap();
	static const ProfileCountryMap& GetCountryMap();
	static const ProfileStateAbbrevMap& GetStateAbbrevMap();

	const ProfileFieldInfo& GetField(ProfileField theField) const;
	std::wstring GetWString(ProfileField theField) const;
	unsigned short GetShort(ProfileField theField) const;
	unsigned char GetByte(ProfileField theField) const;

	unsigned short GetAgeRange() const;
	unsigned short GetCountry() const;
	std::wstring GetStateAbbrev() const;
	ProfileGender GetGender() const;

	void Set(ProfileField theField, const std::wstring &theVal);
	void SetByte(ProfileField theField, unsigned char theByte);
	void SetShort(ProfileField theField, unsigned short theShort);

	void SetAgeRange(unsigned short theAgeRangeCode);
	void SetCountry(unsigned short theCountryCode);
	void SetState(std::wstring theStateAbbrev);
	void SetSource(unsigned short theSourceCode);
	void SetGender(ProfileGender theGender);

	void Clear(ProfileField theField);
	void Clear();

	const ProfileFieldMap& GetFieldMap() const { return mFieldMap; }

	void UnpackGetReply(ReadBuffer &theMsg);
	void UnpackSetReply(ReadBuffer &theMsg);
};

typedef SmartPtr<ProfileData> ProfileDataPtr;


}; // namespace WONAPI


#endif
