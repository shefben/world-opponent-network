#include "WONAPI.h"
#include "ProfileData.h"
#include "WONCommon/WriteBuffer.h"
#include "WONCommon/ReadBuffer.h"
#include "WONCommon/StringUtil.h"

using namespace WONAPI;
using namespace std;

struct CountryCode
{
	unsigned short mCode;
	const char *mCountry;
};

static CountryCode gCountries[] = 
{
{  0, "UnSpecified" },
{  1, "UNITED STATES OF AMERICA" },
{ 37, "AFGHANISTAN" },
{ 38, "ALBANIA" },
{ 39, "ALGERIA" },
{ 40, "AMERICAN SAMOA" },
{ 41, "ANDORRA" },
{ 42, "ANGOLA" },
{ 43, "ANGUILLA" },
{ 44, "ANTARCTICA" },
{ 45, "ANTIGUA AND BARBUDA" },
{ 46, "ARGENTINA" },
{ 47, "ARMENIA" },
{ 48, "ARUBA" },
{ 49, "AUSTRALIA" },
{ 50, "AUSTRIA" },
{ 51, "AZERBAIJAN" },
{ 52, "BAHAMAS" },
{ 53, "BAHRAIN" },
{ 54, "BANGLADESH" },
{ 55, "BARBADOS" },
{ 56, "BELARUS" },
{ 57, "BELGIUM" },
{ 58, "BELIZE" },
{ 59, "BENIN" },
{ 60, "BERMUDA" },
{ 61, "BHUTAN" },
{ 62, "BOLIVIA" },
{ 63, "BOSNIA AND HERZEGOWINA" },
{ 64, "BOTSWANA" },
{ 65, "BOUVET ISLAND" },
{ 66, "BRAZIL" },
{ 67, "BRITISH INDIAN OCEAN TERRITORY" },
{ 68, "BRUNEI DARUSSALAM" },
{ 69, "BULGARIA" },
{ 70, "BURKINA FASO" },
{ 71, "BURUNDI" },
{ 72, "CAMBODIA" },
{ 73, "CAMEROON" },
{ 74, "CANADA" },
{ 75, "CAPE VERDE" },
{ 76, "CAYMAN ISLANDS" },
{ 77, "CENTRAL AFRICAN REPUBLIC" },
{ 78, "CHAD" },
{ 79, "CHILE" },
{ 80, "CHINA" },
{ 81, "CHRISTMAS ISLAND" },
{ 82, "COCOS (KEELING) ISLANDS" },
{ 83, "COLOMBIA" },
{ 84, "COMOROS" },
{ 85, "CONGO" },
{ 86, "COOK ISLANDS" },
{ 87, "COSTA RICA" },
{ 88, "COTE D'IVOIRE" },
{ 89, "CROATIA (local name: Hrvatska)" },
{ 90, "CUBA" },
{ 91, "CYPRUS" },
{ 92, "CZECH REPUBLIC" },
{ 93, "DENMARK" },
{ 94, "DJIBOUTI" },
{ 95, "DOMINICA" },
{ 96, "DOMINICAN REPUBLIC" },
{ 97, "EAST TIMOR" },
{ 98, "ECUADOR" },
{ 99, "EGYPT" },
{100, "EL SALVADOR" },
{101, "EQUATORIAL GUINEA" },
{102, "ERITREA" },
{103, "ESTONIA" },
{104, "ETHIOPIA" },
{105, "FALKLAND ISLANDS (MALVINAS)" },
{106, "FAROE ISLANDS" },
{107, "FIJI" },
{108, "FINLAND" },
{109, "FRANCE" },
{110, "FRANCE, METROPOLITAN" },
{111, "FRENCH GUIANA" },
{112, "FRENCH POLYNESIA" },
{113, "FRENCH SOUTHERN TERRITORIES" },
{114, "GABON" },
{115, "GAMBIA" },
{116, "GEORGIA" },
{117, "GERMANY" },
{118, "GHANA" },
{119, "GIBRALTAR" },
{120, "GREECE" },
{121, "GREENLAND" },
{122, "GRENADA" },
{123, "GUADELOUPE" },
{124, "GUAM" },
{125, "GUATEMALA" },
{126, "GUINEA" },
{127, "GUINEA-BISSAU" },
{128, "GUYANA" },
{129, "HAITI" },
{130, "HEARD AND MC DONALD ISLANDS" },
{131, "HONDURAS" },
{132, "HONG KONG" },
{133, "HUNGARY" },
{134, "ICELAND" },
{135, "INDIA" },
{136, "INDONESIA" },
{137, "IRAN (ISLAMIC REPUBLIC OF)" },
{138, "IRAQ" },
{139, "IRELAND" },
{140, "ISRAEL" },
{141, "ITALY" },
{142, "JAMAICA" },
{143, "JAPAN" },
{144, "JORDAN" },
{145, "KAZAKHSTAN" },
{146, "KENYA" },
{147, "KIRIBATI" },
{148, "KOREA, DEMOCRATIC PEOPLE'S REPUBLIC OF" },
{149, "KOREA, REPUBLIC OF" },
{150, "KUWAIT" },
{151, "KYRGYZSTAN" },
{152, "LAO PEOPLE'S DEMOCRATIC REPUBLIC" },
{153, "LATVIA" },
{154, "LEBANON" },
{155, "LESOTHO" },
{156, "LIBERIA" },
{157, "LIBYAN ARAB JAMAHIRIYA" },
{158, "LIECHTENSTEIN" },
{159, "LITHUANIA" },
{160, "LUXEMBOURG" },
{161, "MACAU" },
{162, "MACEDONIA, THE FORMER YUGOSLAV REPUBLIC OF" },
{163, "MADAGASCAR" },
{164, "MALAWI" },
{165, "MALAYSIA" },
{166, "MALDIVES" },
{167, "MALI" },
{168, "MALTA" },
{169, "MARSHALL ISLANDS" },
{170, "MARTINIQUE" },
{171, "MAURITANIA" },
{172, "MAURITIUS" },
{173, "MAYOTTE" },
{174, "MEXICO" },
{175, "MICRONESIA, FEDERATED STATES OF" },
{176, "MOLDOVA, REPUBLIC OF" },
{177, "MONACO" },
{178, "MONGOLIA" },
{179, "MONTSERRAT" },
{180, "MOROCCO" },
{181, "MOZAMBIQUE" },
{182, "MYANMAR" },
{183, "NAMIBIA" },
{184, "NAURU" },
{185, "NEPAL" },
{186, "NETHERLANDS" },
{187, "NETHERLANDS ANTILLES" },
{188, "NEW CALEDONIA" },
{189, "NEW ZEALAND" },
{190, "NICARAGUA" },
{191, "NIGER" },
{192, "NIGERIA" },
{193, "NIUE" },
{194, "NORFOLK ISLAND" },
{195, "NORTHERN MARIANA ISLANDS" },
{196, "NORWAY" },
{197, "OMAN" },
{198, "PAKISTAN" },
{199, "PALAU" },
{200, "PANAMA" },
{201, "PAPUA NEW GUINEA" },
{202, "PARAGUAY" },
{203, "PERU" },
{204, "PHILIPPINES" },
{205, "PITCAIRN" },
{206, "POLAND" },
{207, "PORTUGAL" },
{208, "PUERTO RICO" },
{209, "QATAR" },
{210, "REUNION" },
{211, "ROMANIA" },
{212, "RUSSIAN FEDERATION" },
{213, "RWANDA" },
{214, "SAINT KITTS AND NEVIS" },
{215, "SAINT LUCIA" },
{216, "SAINT VINCENT AND THE GRENADINES" },
{217, "SAMOA" },
{218, "SAN MARINO" },
{219, "SAO TOME AND PRINCIPE" },
{220, "SAUDI ARABIA" },
{221, "SENEGAL" },
{222, "SEYCHELLES" },
{223, "SIERRA LEONE" },
{224, "SINGAPORE" },
{225, "SLOVAKIA (Slovak Republic)" },
{226, "SLOVENIA" },
{227, "SOLOMON ISLANDS" },
{228, "SOMALIA" },
{229, "SOUTH AFRICA" },
{230, "SOUTH GEORGIA AND THE SOUTH SANDWICH ISLANDS" },
{231, "SPAIN" },
{232, "SRI LANKA" },
{233, "ST. HELENA" },
{234, "ST. PIERRE AND MIQUELON" },
{235, "SUDAN" },
{236, "SURINAME" },
{237, "SVALBARD AND JAN MAYEN ISLANDS" },
{238, "SWAZILAND" },
{239, "SWEDEN" },
{240, "SWITZERLAND" },
{241, "SYRIAN ARAB REPUBLIC" },
{242, "TAIWAN, PROVINCE OF CHINA" },
{243, "TAJIKISTAN" },
{244, "TANZANIA, UNITED REPUBLIC OF" },
{245, "THAILAND" },
{246, "TOGO" },
{247, "TOKELAU" },
{248, "TONGA" },
{249, "TRINIDAD AND TOBAGO" },
{250, "TUNISIA" },
{251, "TURKEY" },
{252, "TURKMENISTAN" },
{253, "TURKS AND CAICOS ISLANDS" },
{254, "TUVALU" },
{255, "UGANDA" },
{256, "UKRAINE" },
{257, "UNITED ARAB EMIRATES" },
{258, "UNITED KINGDOM" },
{260, "UNITED STATES MINOR OUTLYING ISLANDS" },
{261, "URUGUAY" },
{262, "UZBEKISTAN" },
{263, "VANUATU" },
{264, "VATICAN CITY STATE (HOLY SEE)" },
{265, "VENEZUELA" },
{266, "VIET NAM" },
{267, "VIRGIN ISLANDS (BRITISH)" },
{268, "VIRGIN ISLANDS (U.S.)" },
{269, "WALLIS AND FUTUNA ISLANDS" },
{270, "WESTERN SAHARA" },
{271, "YEMEN" },
{272, "YUGOSLAVIA" },
{273, "ZAIRE" },
{274, "ZAMBIA" },
{275, "ZIMBABWE" }
};

struct AgeRangeCode
{
	unsigned short mCode;
	const char *mAgeRange;
};

static AgeRangeCode gAgeRanges[] =
{
	{2, "UNDER 13"},
	{3, "13-17"},
	{4, "18-24"},
	{5, "25-34"},
	{6, "35-44"},
	{7, "45-54"},
	{8, "55+"}
};

struct StateAbbrevCode
{
	const wchar_t* mStateAbbrv;
	const char* mStateName;
};

static StateAbbrevCode gStateAbbrevs[] =
{
	{ L"AL","ALABAMA"},
	{ L"AK","ALASKA"},
	{ L"AZ","ARIZONA"},
	{ L"AR","ARKANSAS"},
	{ L"CA","CALIFORNIA"},
	{ L"CO","COLORADO"},
	{ L"CT","CONNECTICUT"},
	{ L"DE","DELAWARE"},
	{ L"DC","DISTRICT OF COLUMBIA"},
	{ L"FL","FLORIDA"},
	{ L"GA","GEORGIA"},
	{ L"GU","GUAM "},
	{ L"HI","HAWAII"},
	{ L"ID","IDAHO"},
	{ L"IL","ILLINOIS"},
	{ L"IN","INDIANA"},
	{ L"IA","IOWA"},
	{ L"KS","KANSAS"},
	{ L"KY","KENTUCKY"},
	{ L"LA","LOUISIANA"},
	{ L"ME","MAINE"},
	{ L"MD","MARYLAND"},
	{ L"MA","MASSACHUSETTS"},
	{ L"MI","MICHIGAN"},
	{ L"MN","MINNESOTA"},
	{ L"MS","MISSISSIPPI"},
	{ L"MO","MISSOURI"},
	{ L"MT","MONTANA"},
	{ L"NE","NEBRASKA"},
	{ L"NV","NEVADA"},
	{ L"NH","NEW HAMPSHIRE"},
	{ L"NJ","NEW JERSEY"},
	{ L"NM","NEW MEXICO"},
	{ L"NY","NEW YORK"},
	{ L"NC","NORTH CAROLINA"},
	{ L"ND","NORTH DAKOTA"},
	{ L"OH","OHIO"},
	{ L"OK","OKLAHOMA"},
	{ L"OR","OREGON"},
	{ L"PA","PENNSYLVANIA"},
	{ L"PR","PUERTO RICO"},
	{ L"RI","RHODE ISLAND"},
	{ L"SC","SOUTH CAROLINA"},
	{ L"SD","SOUTH DAKOTA"},
	{ L"TN","TENNESSEE"},
	{ L"TX","TEXAS"},
	{ L"UT","UTAH"},
	{ L"VT","VERMONT"},
	{ L"VI","VIRGIN ISLANDS"},
	{ L"VA","VIRGINIA"},
	{ L"WA","WASHINGTON"},
	{ L"WV","WEST VIRGINIA"},
	{ L"WI","WISCONSIN"},
	{ L"WY","WYOMING"}
};


bool ProfileData::mLoadedData = false;
ProfileCountryMap ProfileData::mCountryMap;
ProfileAgeRangeMap ProfileData::mAgeRangeMap;
ProfileStateAbbrevMap ProfileData::mStateAbbrevMap;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void StripEndWhitespace(const char *theStr, std::string &strOut)
{
	strOut.erase();
	while((*theStr)!='\0')
	{
		if(isspace(*theStr))
			theStr++;
		else
			break;
	}

	strOut = theStr;
	for(int i=strOut.length()-1; i>=0; i--)
	{
		if(isspace(strOut[i]))
			strOut.resize(strOut.length()-1);
		else
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::LoadData()
{
	if(mLoadedData)
		return;

	mLoadedData = true;

	ProfileCountryMap::iterator aCountryItr = mCountryMap.begin();
	FILE *aFile = fopen( (WONAPICore::GetDefaultFileDirectory() + "_woncountries.txt").c_str(),"r");
	if(aFile==NULL)
	{
		int aNumCountries = sizeof(gCountries)/sizeof(CountryCode);
		for(int i=0; i<aNumCountries; i++)
			aCountryItr = mCountryMap.insert(aCountryItr, ProfileCountryMap::value_type(gCountries[i].mCode, gCountries[i].mCountry));
	}
	else
	{
		char aCountry[1024];
		unsigned short aCode;

		while(fscanf(aFile,"%d%[^\n]",&aCode, aCountry)==2)
		{
			string aCountryStr;
			StripEndWhitespace(aCountry, aCountryStr);
			aCountryItr = mCountryMap.insert(aCountryItr,ProfileCountryMap::value_type(aCode,aCountryStr));
		}

		fclose(aFile);
	}

	ProfileAgeRangeMap::iterator anAgeRangeItr = mAgeRangeMap.begin();
	aFile = fopen( (WONAPICore::GetDefaultFileDirectory() + "_wonagerange.txt").c_str(),"r");
	if(aFile==NULL)
	{
		int aNumAgeRanges = sizeof(gAgeRanges)/sizeof(AgeRangeCode);
		for(int i=0; i<aNumAgeRanges; i++)
			anAgeRangeItr = mAgeRangeMap.insert(anAgeRangeItr, ProfileAgeRangeMap::value_type(gAgeRanges[i].mCode, gAgeRanges[i].mAgeRange));
		
	}
	else
	{
		char anAgeRange[1024];
		unsigned short aCode;

		while(fscanf(aFile,"%d%[^\n]",&aCode, anAgeRange)==2)
		{
			string anAgeRangeStr;
			StripEndWhitespace(anAgeRange, anAgeRangeStr);
			anAgeRangeItr = mAgeRangeMap.insert(anAgeRangeItr,ProfileAgeRangeMap::value_type(aCode,anAgeRangeStr));
		}

		fclose(aFile);
	}

	ProfileStateAbbrevMap::iterator aStateAbbrevItr = mStateAbbrevMap.begin();
	aFile = fopen( (WONAPICore::GetDefaultFileDirectory() + "_wonstateabbrev.txt").c_str(),"r");
	if(aFile==NULL)
	{
		int nNumStateAbbrevs = sizeof(gStateAbbrevs)/sizeof(StateAbbrevCode);
		for(int i=0; i<nNumStateAbbrevs; i++)
			aStateAbbrevItr = mStateAbbrevMap.insert(aStateAbbrevItr, ProfileStateAbbrevMap::value_type(gStateAbbrevs[i].mStateAbbrv,gStateAbbrevs[i].mStateName));
	}
	else
	{
		char aState[1024];
		wchar_t aStateAbbrev[2];

		while(fscanf(aFile,"%s%[^\n]", aStateAbbrev, aState)==2)
		{
			string aStateStr;
			StripEndWhitespace(aState,aStateStr);
			aStateAbbrevItr = mStateAbbrevMap.insert(aStateAbbrevItr,ProfileStateAbbrevMap::value_type(aStateAbbrev,aStateStr));
		}

		fclose(aFile);
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const ProfileAgeRangeMap& ProfileData::GetAgeRangeMap()
{
	LoadData();
	return mAgeRangeMap;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const ProfileCountryMap& ProfileData::GetCountryMap()
{
	LoadData();
	return mCountryMap;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const ProfileStateAbbrevMap& ProfileData::GetStateAbbrevMap()
{
	LoadData();
	return mStateAbbrevMap;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ProfileData::ProfileData()
	: mCustomActionId(0),
	  mBirthMonth(0),
	  mBirthDay(0),
	  mBirthYear(0)
{
	LoadData();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ProfileData::~ProfileData()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const ProfileFieldInfo& ProfileData::GetField(ProfileField theField) const
{
	static ProfileFieldInfo anInvalid;
		
	ProfileFieldMap::const_iterator anItr = mFieldMap.find(theField);
	if(anItr==mFieldMap.end())
		return anInvalid;
	else
		return anItr->second;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::wstring ProfileData::GetWString(ProfileField theField) const
{
	wstring aStr;
	const ByteBuffer* aBuf = GetField(theField).mData;

	try
	{
		if(aBuf!=NULL)
		{
			ReadBuffer aReadBuf(aBuf->data(),aBuf->length());
			aReadBuf.ReadRawWString(aStr,aBuf->length()/2);
		}

	}
	catch(ReadBufferException&)
	{
	}

	return aStr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned short ProfileData::GetShort(ProfileField theField) const
{
	const ByteBuffer* aBuf = GetField(theField).mData;
	unsigned short aVal = 0;
	try
	{
		if(aBuf!=NULL)
		{
			ReadBuffer aReadBuf(aBuf->data(),aBuf->length());
			aVal = aReadBuf.ReadShort();
		}
	}
	catch(ReadBufferException&)
	{
	}

	return aVal;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned char ProfileData::GetByte(ProfileField theField) const
{
	const ByteBuffer* aBuf = GetField(theField).mData;
	unsigned char aChar = 0;
	if(aBuf!=NULL)
		aChar = aBuf->data()[0];

	return aChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned short ProfileData::GetAgeRange() const
{
	return GetShort(ProfileField_AgeRange);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned short ProfileData::GetCountry() const
{
	return GetShort(ProfileField_Country);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::wstring ProfileData::GetStateAbbrev() const
{
	const ByteBuffer* aBuf = GetField(ProfileField_State).mData;
	std::wstring theStateAbbrev;

	if (aBuf!=NULL)
	{
		theStateAbbrev += aBuf->data()[0];
		theStateAbbrev += aBuf->data()[2];
	}

	return theStateAbbrev;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ProfileGender ProfileData::GetGender() const
{
	unsigned char aChar = GetByte(ProfileField_Gender);
	if(aChar==0)
		return ProfileGender_Female;
	else
		return ProfileGender_Male;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::SetAgeRange(unsigned short theAgeRangeCode)
{
	SetByte(ProfileField_AgeRange,theAgeRangeCode);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::SetCountry(unsigned short theCountryCode)
{
	SetShort(ProfileField_Country, theCountryCode);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::SetState(std::wstring theStateAbbrev)
{
	Set(ProfileField_State, theStateAbbrev);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::SetSource(unsigned short theSourceCode)
{
	SetShort(ProfileField_Source,theSourceCode);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::SetGender(ProfileGender theGender)
{
	WriteBuffer aBuf;
	if(theGender==ProfileGender_Male)
		SetByte(ProfileField_Gender, 1);
	else
		SetByte(ProfileField_Gender, 0);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::SetByte(ProfileField theField, unsigned char theByte)
{
	WriteBuffer aBuf;
	aBuf.AppendByte(theByte);
	mFieldMap[theField] = ProfileFieldInfo(aBuf.ToByteBuffer());	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::SetShort(ProfileField theField, unsigned short theShort)
{
	WriteBuffer aBuf;
	aBuf.AppendShort(theShort);
	mFieldMap[theField] = ProfileFieldInfo(aBuf.ToByteBuffer());	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::Set(ProfileField theField, const std::wstring &theVal)
{
	WriteBuffer aBuf;
	aBuf.AppendWString(theVal,0);
	mFieldMap[theField] = ProfileFieldInfo(aBuf.ToByteBuffer());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::Clear(ProfileField theField)
{
	mFieldMap.erase(theField);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::Clear()
{
	mNewsletterSubMap.clear();
	mFieldMap.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::UnpackGetReply(ReadBuffer &theMsg)
{
	unsigned short aNumFields = theMsg.ReadShort();
	for(int i=0; i<aNumFields; i++)
	{
		ProfileField aField = (ProfileField)theMsg.ReadLong();
		ProfileFieldInfo &anInfo = mFieldMap[aField];
		anInfo.mData = theMsg.ReadBuf(2);
		anInfo.mErrorStatus = (WONStatus)theMsg.ReadShort();
		theMsg.ReadString(anInfo.mErrorString);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::UnpackSetReply(ReadBuffer &theMsg)
{
	unsigned short aNumFields = theMsg.ReadShort();
	for(int i=0; i<aNumFields; i++)
	{
		ProfileField aField = (ProfileField)theMsg.ReadLong();
		ProfileFieldInfo &anInfo = mFieldMap[aField];
		anInfo.mErrorStatus = (WONStatus)theMsg.ReadShort();
		theMsg.ReadString(anInfo.mErrorString);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::AddNewsletterSub(unsigned short theId, bool subscribe)
{
	mNewsletterSubMap[theId] = subscribe;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ProfileData::GetNewsletterSub(unsigned short theId)
{
	if (mNewsletterSubMap.find(theId) == mNewsletterSubMap.end())
		return false;
	else
		return mNewsletterSubMap[theId];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::ClearNewsletterSub(unsigned short theId)
{
	mNewsletterSubMap.erase(theId);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::ClearNewsletterSubs()
{
	mNewsletterSubMap.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::SetCustomAction(unsigned short theActionId, const ByteBuffer *theData)
{
	mCustomActionId = theActionId;
	mCustomActionData = theData;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::ClearCustomAction()
{
	mCustomActionId = 0;
	mCustomActionData = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::PackRequestData(WriteBuffer &theMsg, bool isSet)
{
	unsigned short aNumFields = 0;
	int aNumFieldsPos = theMsg.length();
	theMsg.AppendShort(0); // skip num fields for now

	// Append the newsletter subs
	ProfileNewsletterSubMap::iterator anItr = mNewsletterSubMap.begin();
	while(anItr!=mNewsletterSubMap.end())
	{
		theMsg.AppendShort(ProfileField_NewsLetterSub);

		theMsg.AppendShort( isSet ? 3:2 ); // 3 bytes to follow
		theMsg.AppendShort(anItr->first);

		if (isSet)
			theMsg.AppendBool(anItr->second);

		aNumFields++;
		++anItr;
	}
	
	// Append the custom action
	if(mCustomActionId!=0)
	{
		theMsg.AppendShort(ProfileField_CustomAction);
		int aBufSize = 0;
		if(mCustomActionData.get()!=NULL)
			aBufSize = mCustomActionData->length();

		theMsg.AppendShort(2 + aBufSize); // action id + custom data
		theMsg.AppendShort(mCustomActionId);
		theMsg.AppendBuffer(mCustomActionData,0);

		aNumFields++;
	}

	// Append the birthdate
	theMsg.AppendShort(ProfileField_BirthDate);
	theMsg.AppendShort(4); // action id + sizeof(long)
	theMsg.AppendByte(mBirthMonth);
	theMsg.AppendByte(mBirthDay);
	theMsg.AppendShort(mBirthYear);
	aNumFields++;

	// Closer
	theMsg.SetShort(aNumFieldsPos, aNumFields);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::UnpackGetData(ReadBuffer &theMsg)
{
	unsigned short aNumFields	= 0;
	unsigned short aFieldType	= 0;
	unsigned short aFieldLength = 0;
	int aNumFieldsPos = theMsg.length();

	try
	{
		// Number of items to follow
		aNumFields = theMsg.ReadShort();

		for (int field_num = 0; field_num < aNumFields; ++field_num)
		{
			aFieldType = theMsg.ReadShort();
			aFieldLength = theMsg.ReadShort();

			switch(aFieldType)
			{
			case ProfileField_NewsLetterSub:
				{
					unsigned short aNewsLetterSubType = theMsg.ReadShort();
					mNewsletterSubMap[aNewsLetterSubType]  = theMsg.ReadBool();
				}
				break;
			case ProfileField_CustomAction:
				{
					unsigned short aCustomActionID = theMsg.ReadShort();
					mCustomActionData = new ByteBuffer( (const char*)theMsg.ReadBytes(aFieldLength - sizeof(unsigned short)), aFieldLength-sizeof(unsigned short));
				}
				break;
			case ProfileField_BirthDate:
				{
					mBirthMonth = theMsg.ReadByte();
					mBirthDay   = theMsg.ReadByte();
					mBirthYear  = theMsg.ReadShort();
				}
				break;
			}
		}
	}
	catch (ReadBufferException&)
	{
	}

}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProfileData::SetBirthDate(const unsigned char theMonth, const unsigned char theDay, const unsigned short theYear)
{
	mBirthMonth = theMonth;
	mBirthDay   = theDay;
	mBirthYear  = theYear;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned long ProfileData::GetBirthDate()
{
	return (mBirthMonth) | (mBirthDay<<8) | (mBirthYear<<16);
}

