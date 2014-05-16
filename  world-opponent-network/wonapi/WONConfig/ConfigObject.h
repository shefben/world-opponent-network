#ifndef __WON_CONFIGOBJECT_H__
#define __WON_CONFIGOBJECT_H__

#include "WONCommon/SmartPtr.h"
#include "WONCommon/StringParser.h"
#include "ConfigParser.h"
#include "WONTypes.h"
#include "ConfigValue.h"
#include <list>

namespace WONAPI
{

class ConfigObject;
typedef SmartPtr<ConfigObject> ConfigObjectPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ConfigObject : public RefCount
{
protected:
	typedef std::list<ConfigObjectPtr> ChildList;
	ChildList mChildList;
	ChildList::iterator mChildItr;
	std::string mObjectName;
	std::string mInheritedName;
	ConfigObject *mParent;

	typedef ConfigObject*(*ConfigObjectFactory)();
	struct GroupType
	{
		ConfigObjectFactory mFactory;
		ConfigObjectPtr *mObjectPtr;
		ConfigObjectPtr mObject;

		GroupType(ConfigObjectFactory theFactory = NULL, ConfigObjectPtr *theObject = NULL) : mFactory(theFactory), mObjectPtr(theObject) { }
		GroupType(ConfigObject *theObject) : mFactory(NULL), mObjectPtr(NULL), mObject(theObject) { }
	};


	typedef std::map<std::string,ConfigValueBasePtr,StringLessNoCase> KeyValMap;
	KeyValMap mKeyValMap;

	typedef std::map<std::string,GroupType,StringLessNoCase> KeyGroupMap;
	KeyGroupMap mKeyGroupMap;


protected:
	bool ReadGroupStart(ConfigParser &theParser, std::string &theGroupName, std::string &theInheritedName);

	bool HandleChildGroup(ConfigParser &theParser, KeyGroupMap::iterator &theItr);
	bool HandleKeyVal(ConfigParser &theParser, StringParser &theVal, KeyValMap::iterator &theItr);

	virtual bool HandleChildGroup(ConfigParser &theParser, const std::string& theGroupType);
	virtual bool HandleKeyVal(ConfigParser &theParser, const std::string& theKey, StringParser &theVal);

	virtual bool PreParse(ConfigParser &theParser);
	virtual bool PostParse(ConfigParser &theParser);

	void RegisterKeyGroup(const char *theKey, ConfigObjectFactory theFactory, ConfigObjectPtr *theGroup = NULL);
	void RegisterKeyGroup(const char *theKey, ConfigObject *theGroup);
	void RegisterKeyValue(const char *theKey, ConfigValueBase *theValue);
	
	void RegisterBoolValue(const char *theKey, bool &theBool, bool theDefVal = false);

	void RegisterInt64Value(const char *theKey, __int64 &theInt64, __int64 theDefVal = 0);
	void RegisterIntValue(const char *theKey, int &theInt, int theDefVal = 0);
	void RegisterLongValue(const char *theKey, long &theLong, long theDefVal = 0);
	void RegisterShortValue(const char *theKey, short &theShort, short theDefVal = 0);
	void RegisterByteValue(const char *theKey, char &theByte, char theDefVal = 0);
	void RegisterUInt64Value(const char *theKey, unsigned __int64 &theInt64, unsigned __int64 theDefVal = 0);
	void RegisterULongValue(const char *theKey, unsigned long &theLong, unsigned long theDefVal = 0);
	void RegisterUShortValue(const char *theKey, unsigned short &theShort, unsigned short theDefVal = 0);
	void RegisterUByteValue(const char *theKey, unsigned char &theByte, unsigned char theDefVal = 0);

	void RegisterStringValue(const char *theKey, std::string &theStr, const char *theDefVal = NULL);
	void RegisterWStringValue(const char *theKey, std::wstring &theStr, const wchar_t *theDefVal = NULL);

	void RegisterBoolListValue(const char *theKey, WONTypes::BoolList &theBoolList, const char *theDefVal = NULL, bool additive = false);
	void RegisterIntListValue(const char *theKey, WONTypes::IntList &theIntList, const char *theDefVal = NULL, bool additive = false);
	void RegisterStringListValue(const char *theKey, WONTypes::StringList &theStringList, const char *theDefVal = NULL, bool additive = false);
	void RegisterWStringListValue(const char *theKey, WONTypes::WStringList &theStringList, const wchar_t *theDefVal = NULL, bool additive = false);

	void RegisterBoolListListValue(const char *theKey, WONTypes::BoolListList &theBoolListList, const char *theDefVal = NULL, bool additive = false);
	void RegisterIntListListValue(const char *theKey, WONTypes::IntListList &theIntListList, const char *theDefVal = NULL, bool additive = false);
	void RegisterStringListListValue(const char *theKey, WONTypes::StringListList &theStringListList, const char *theDefVal = NULL, bool additive = false);
	void RegisterWStringListListValue(const char *theKey, WONTypes::WStringListList &theStringListList, const wchar_t *theDefVal = NULL, bool additive = false);

	bool QueryValue(KeyValMap::const_iterator theItr, std::string &theValR);

	bool IncludeFile(ConfigParser &theParser, const std::string &theFilePath, bool required = true);
	bool IncludeGroup(ConfigParser &theParser, const std::string &theGroupName, bool required = true);

public:
	ConfigObject();

	virtual void Reset(bool resetName = true);
	void Clear();
	void Rewind();
	ConfigObject* NextChild();	
	ConfigObject* GetParent() { return mParent; }
	void SetParent(ConfigObject *theParent) { mParent = theParent; }

	bool HasMoreChildren();
	int GetNumChildren() { return mChildList.size(); }

	void AddChild(ConfigObject *theChild);
	const std::string& GetName() const { return mObjectName; }
	void SetName(const std::string &theName) { mObjectName = theName; }

	void CopyKeyValsFrom(ConfigObject *theCopyFrom);

	bool ParseTopLevel(ConfigParser &theParser, const char *theFilePath = NULL); // will try to open the parser to theFilePath if not NULL
	bool ParseGroup(ConfigParser& theParser, bool checkBraces = true, bool skip = false, bool doPreAndPostParse = true, bool readGroupStart = true);
	bool SkipGroup(ConfigParser& theParser, bool checkBraces = true, bool readGroupStart = true);
	bool ParseChild(ConfigParser& theParser, ConfigObject *theChild, bool addChild = true, bool readGroupStart = true);

	virtual bool QueryValue(const std::string &theKey, std::string &theValR);
	virtual void QueryAllValues(std::string &theValues);

	enum SetValueResult { SetValue_Success, SetValue_NotFound, SetValue_ErrorSetting };
	virtual SetValueResult SetValue(const std::string &theKey, StringParser &theParser);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ConfigObjectException : public std::exception
{
public:
	std::string what;
	bool mAbort;

	ConfigObjectException() : exception(), mAbort(true) { }
	ConfigObjectException(const std::string &theWhat, bool abort = true) { what = theWhat; mAbort = abort; }
};

}; // namespace WONAPI

#endif
