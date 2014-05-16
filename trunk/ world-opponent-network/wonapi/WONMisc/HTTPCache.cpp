#include "HTTPCache.h"
#include "URL.h"
#include "WONAPI.h"

#include "WONCommon/WONFile.h"

using namespace WONAPI;
using namespace std;

static int CACHE_VERSION = 1;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTTPCacheEntry::HTTPCacheEntry()
{
	mIsValid = false;
	mId = 0;
	mExpirationTime = 0;
	mLastAccessTime = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTTPCacheEntry::ReadFromFile(FileReader &theReader)
{
	mId = theReader.ReadLong();
	mIsValid = theReader.ReadByte()!=0;
	mExpirationTime = theReader.ReadLong();
	mLastAccessTime = theReader.ReadLong();

	theReader.ReadString(mRemoteLocation);
	theReader.ReadString(mFileName);
	theReader.ReadString(mFileType);
	theReader.ReadString(mEntityTag);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTTPCacheEntry::WriteToFile(FileWriter &theWriter)
{
	theWriter.WriteLong(mId);
	theWriter.WriteByte(mIsValid?1:0);
	theWriter.WriteLong(mExpirationTime);
	theWriter.WriteLong(mLastAccessTime);

	theWriter.WriteString(mRemoteLocation);
	theWriter.WriteString(mFileName);
	theWriter.WriteString(mFileType);
	theWriter.WriteString(mEntityTag);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTTPCacheEntry::IsValid()
{
	return mIsValid;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static HTTPCachePtr gGlobalCache;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTTPCache::HTTPCache()
{
	mCreatedDirectory = false;
	mNextFreeId = 1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTTPCache::HTTPCache(const std::string &thePath, bool useFullPath)
{	
	SetCachePath(thePath, useFullPath);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTTPCache::~HTTPCache()
{
	WriteCacheMap();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTTPCache* HTTPCache::GetGlobalCache()
{
	if(gGlobalCache.get()==NULL)
		gGlobalCache = new HTTPCache(WONAPICore::GetDefaultFileDirectory() +  "_wonHTTPCache");

	return gGlobalCache;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTTPCache::SetGlobalCache(HTTPCache* theCache)
{
	gGlobalCache = theCache;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTTPCache::ReadCacheMap()
{
	mCacheIdMap.clear();
	mCacheLocationMap.clear();
	mNextFreeId = 1;

	FileReader aReader;
	std::string aPath = mCachePath + "cachemap.dat";
	if(!aReader.Open(aPath.c_str()))
		return;

	try
	{
		int aVersion = aReader.ReadLong();
		if(aVersion!=CACHE_VERSION)
			return;

		int aNumEntries = aReader.ReadLong();
		for(int i=0; i<aNumEntries; i++)
		{
			HTTPCacheEntryPtr anEntry = new HTTPCacheEntry;
			anEntry->ReadFromFile(aReader);
			anEntry->mLocalPath = mCachePath + anEntry->mFileName;

			mCacheIdMap[anEntry->mId] = anEntry;
			mCacheLocationMap[anEntry->mRemoteLocation] = anEntry;
		}
	}
	catch(FileReaderException&)
	{
	}
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTTPCache::WriteCacheMap()
{
	FileWriter aWriter;
	std::string aPath = mCachePath + "cachemap.dat";
	if(!aWriter.Open(aPath.c_str()))
		return;

	time_t aTimeNow = time(NULL);
	try
	{
		aWriter.WriteLong(CACHE_VERSION);

		int aSize = mCacheIdMap.size();
		int aPos = aWriter.pos();
		aWriter.WriteLong(aSize);

		int aNumEntries = 0;
		CacheIdMap::iterator anItr = mCacheIdMap.begin();
		while(anItr!=mCacheIdMap.end())
		{
			HTTPCacheEntry *anEntry = anItr->second;
			if(anEntry->IsValid() && aTimeNow-anEntry->mLastAccessTime<3600*24*14) // two week expiration
			{
				anEntry->WriteToFile(aWriter);
				aNumEntries++;
			}
			else
			{
				WONFile aFile(anEntry->GetLocalPath());
				aFile.Remove();
			}
			++anItr;
		}

		aWriter.SetPos(aPos);
		aWriter.WriteLong(aNumEntries);
	}
	catch(FileWriterException&)
	{
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTTPCache::SetCachePath(const std::string &thePath, bool useFullPath)
{	
	WONFile aPath(thePath);
	if(useFullPath)
		mCachePath = aPath.GetFullPath();
	else
		mCachePath = thePath;

	mCreatedDirectory = false;

//	aPath.CreateAsDirectory();
	if(!mCachePath.empty())
	{
		char aLastChar = mCachePath[mCachePath.length()-1];
		if(aLastChar!='\\' && aLastChar!='/')
			mCachePath += '/';
	}
	ReadCacheMap();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static const std::string CalcHash(const std::string& theLocation)
{
	unsigned char aHashArray[12]= {0,0,0,0,0,0,0,0,0,0,0,0};
	char aHashStr[25];
	
	for (int i = 0; i < theLocation.length(); i++)
		aHashArray[i % 12] += theLocation[i];

	sprintf(aHashStr, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", 
		aHashArray[0], aHashArray[1], aHashArray[2], aHashArray[3],
		aHashArray[4], aHashArray[5], aHashArray[6], aHashArray[7],
		aHashArray[8], aHashArray[9], aHashArray[10], aHashArray[11]);
		
	return aHashStr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTTPCache::GetFreeId()
{	
	CacheIdMap::iterator anItr = mCacheIdMap.find(mNextFreeId);
	while(anItr!=mCacheIdMap.end())
	{
		if(mNextFreeId!=anItr->first)
			break;

		++anItr;
		++mNextFreeId;
	}

	return mNextFreeId;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTTPCacheEntry* HTTPCache::GetEntry(const std::string &theLocation)
{
	if(!mCreatedDirectory)
	{
		WONFile aPath = mCachePath;
		aPath.CreateAsDirectory();
		mCreatedDirectory = true;
	}

	CacheLocationMap::iterator anItr = mCacheLocationMap.insert(CacheLocationMap::value_type(theLocation,NULL)).first;
	HTTPCacheEntry *anEntry = anItr->second;
	if(anEntry==NULL)
	{
		anEntry = anItr->second = new HTTPCacheEntry;
		anEntry->mRemoteLocation = theLocation;
		anEntry->mId = GetFreeId();

		string aType;
		int aSlashPos = theLocation.find_last_of('/');
		if(aSlashPos==string::npos)
			aSlashPos = theLocation.find_last_of('\\');
		if(aSlashPos!=string::npos)
		{
			int aDotPos = theLocation.find_last_of('.');
			if(aDotPos>aSlashPos && aDotPos+10>=theLocation.length())
				aType = theLocation.substr(aDotPos+1);
			else
				aType = "dat";
		}
		else
			aType = "dat";

		char aBuf[50];
		sprintf(aBuf,"%06d.%s",anEntry->mId,aType.c_str());
		anEntry->mFileName = aBuf;
		anEntry->mLocalPath = mCachePath + anEntry->mFileName;
		WONFile aFile(anEntry->mLocalPath);
		aFile.Remove(); // make sure some other entry isn't already there.

		mCacheIdMap[anEntry->mId] = anEntry;
	}

	anEntry->mLastAccessTime = time(NULL);
	return anEntry;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTTPCache::UpdateEntry(HTTPCacheEntry *theEntry)
{
/*	if(!theEntry->mIsValid)
	{
		WONFile aFile(theEntry->mLocalPath);
		aFile.Remove();
		mCacheIdMap.erase(theEntry->mId);
		mCacheLocationMap.erase(theEntry->mRemoteLocation);
	}*/
}
