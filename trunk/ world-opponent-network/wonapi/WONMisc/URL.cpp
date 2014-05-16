#include "URL.h"
#include "WONCommon/StringUtil.h"
#include "WONCommon/WONFile.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
URL::URL()
{
	mType = URLType_None;
	mIsAbsolute = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
URL::URL(const string &theURL)
{
	mIsAbsolute = false;

	int aProtocolEnd = theURL.find("://");
	if(aProtocolEnd!=string::npos)
	{
		string aProtocol = theURL.substr(0,aProtocolEnd);

		if(stricmp(aProtocol.c_str(),"HTTP")==0)
			mType = URLType_HTTP;
		else if(stricmp(aProtocol.c_str(),"FTP")==0)
			mType = URLType_FTP;
		else if(stricmp(aProtocol.c_str(),"FILE")==0)
			mType = URLType_File;
		else 
		{
			mType = URLType_None;
			return;
		}

		mIsAbsolute = true;
		aProtocolEnd+=3;
		int anAddressEnd = theURL.find_first_of('/',aProtocolEnd);
		
		mAddress = theURL.substr(aProtocolEnd,anAddressEnd-aProtocolEnd);
		if(anAddressEnd==string::npos)
			mPath = "/";
		else
			mPath = theURL.substr(anAddressEnd);
	}
	else
	{
		mType = URLType_File;
		mPath = theURL;
	}

	if(mType==URLType_File)
	{
		for(int i=0; i<mPath.length(); i++)
			if(mPath[i]=='\\')
				mPath[i] = '/';
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string URL::GetPathAsLocalFilePath() const
{
	WONFile aFile(mPath);
	return aFile.GetFilePath();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void URL::CalcRelativeTo(const URL &theBaseURL)
{
	if(mIsAbsolute)
		return;

	mIsAbsolute = theBaseURL.mIsAbsolute;
	mType = theBaseURL.mType;
	mAddress = theBaseURL.mAddress;

	if(mPath[0]=='#') // anchor position
	{
		mPath = theBaseURL.mPath + mPath;
		return;
	}

	if(!mPath.empty() && !theBaseURL.mPath.empty() && mPath[0]!='/')
	{
		// relative path
		string aRelativePath = mPath;
		mPath = theBaseURL.mPath;
		int aSlashPos = mPath.find_last_of('/');
		if(aSlashPos==string::npos)
			mPath+='/';
		else
			mPath = mPath.substr(0,aSlashPos+1);

		mPath += aRelativePath;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void URL::GetEntire(std::string &theStr)
{
	theStr.erase();
	switch(mType)
	{
		case URLType_HTTP: theStr += "http://"; break;
		case URLType_FTP: theStr += "ftp://"; break;
	}

	theStr += mAddress;
	theStr += mPath;
}
