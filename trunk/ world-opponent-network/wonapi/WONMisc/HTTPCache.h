#ifndef __WON_HTTPCACHE_H__
#define __WON_HTTPCACHE_H__

#include "WONCommon/SmartPtr.h"
#include "WONCommon/FileReader.h"
#include "WONCommon/FileWriter.h"
#include "WONMisc/URL.h"
#include <map>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class HTTPCacheEntry : public RefCount
{
protected:
	int mId;
	bool mIsValid;

	std::string mRemoteLocation;
	std::string mLocalPath;
	std::string mFileName;
	std::string mFileType;
	std::string mEntityTag;
	time_t mExpirationTime;
	time_t mLastAccessTime;
	friend class HTTPCache;

public:
	void ReadFromFile(FileReader &theReader);
	void WriteToFile(FileWriter &theWriter);

public:
	HTTPCacheEntry();
	bool IsValid();
	void SetValid(bool isValid) { mIsValid = isValid; }

	const std::string& GetRemoteLocation() { return mRemoteLocation; }
	const std::string& GetLocalPath() { return mLocalPath; }

	void SetExpirationTime(time_t theTime) { mExpirationTime = theTime; }
	void SetEntityTag(const std::string &theTag) { mEntityTag = theTag; }

	time_t GetExpirationTime() { return mExpirationTime; }
	const std::string& GetEntityTag() { return mEntityTag; }
};
typedef SmartPtr<HTTPCacheEntry> HTTPCacheEntryPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class HTTPCache : public RefCount
{
protected:
	std::string mCachePath;
	bool mCreatedDirectory;

	typedef std::map<std::string,HTTPCacheEntryPtr> CacheLocationMap;
	typedef std::map<int,HTTPCacheEntryPtr> CacheIdMap;
	CacheLocationMap mCacheLocationMap;
	CacheIdMap mCacheIdMap;
	int mNextFreeId;

	void ReadCacheMap();
	void WriteCacheMap();
	int GetFreeId();

	virtual ~HTTPCache();

public:
	HTTPCache();
	HTTPCache(const std::string &thePath, bool useFullPath = false);
	void SetCachePath(const std::string &thePath, bool useFullPath);

	HTTPCacheEntry* GetEntry(const std::string &theLocation);
	void UpdateEntry(HTTPCacheEntry *theEntry);

	static HTTPCache* GetGlobalCache();
	static void SetGlobalCache(HTTPCache* theCache);
};
typedef SmartPtr<HTTPCache> HTTPCachePtr;

}; // namespace WONAPI

#endif