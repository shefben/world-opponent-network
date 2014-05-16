#include "PatchTypes.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
VersionData::VersionData() : mConfigName(""), mVersion(""), mDescriptionURL(""), mState(VersionState_Invalid) 
{ 
	mVersionTypeList.clear(); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
VersionData::~VersionData() 
{ 
	mVersionTypeList.clear(); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void VersionData::AddVersionType(const std::string& theVersionType)
{ 
	// Parse for any comma seperated strings
	int lastPos = 0;
	int aCommaPos = theVersionType.find(',');
	std::string versionString;

	for (; aCommaPos != -1; aCommaPos = theVersionType.find(',',lastPos))
	{
		versionString = theVersionType.substr(lastPos, aCommaPos-lastPos);
		lastPos = aCommaPos+1;
		mVersionTypeList.push_back(versionString);
	}
	
	versionString = theVersionType.substr(lastPos, aCommaPos-lastPos);
	mVersionTypeList.push_back(versionString);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void VersionData::ReadFromBuffer(ReadBuffer& theBuffer)
{
	theBuffer.ReadString(mConfigName);
	theBuffer.ReadString(mVersion);
	
	int numVersionTypeStrings = theBuffer.ReadByte();
	std::string versionTypeString;

	for (int k=0; k < numVersionTypeStrings; ++k)
	{
		theBuffer.ReadString(versionTypeString);
		AddVersionType(versionTypeString);
	}
	theBuffer.ReadString(mDescriptionURL);
	mState = (VersionState)theBuffer.ReadByte();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void VersionData::WriteToBuffer(WriteBuffer& theWriteBuffer, DWORD theFlags)
{
	theWriteBuffer.AppendString(mConfigName);
	theWriteBuffer.AppendString(mVersion);
	theWriteBuffer.AppendByte(mVersionTypeList.size());

	if (mVersionTypeList.size() != 0)
	{
		VersionTypeList::const_iterator anItr = mVersionTypeList.begin();
		for (; anItr != mVersionTypeList.end(); ++anItr)
		{
			theWriteBuffer.AppendString(*anItr);
		}
	}

	if (theFlags & WriteFlag_AppendDescripURL) 
		theWriteBuffer.AppendString(mDescriptionURL);

	if (theFlags & WriteFlag_AppendState)
		theWriteBuffer.AppendByte(mState);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool VersionData::operator==(const VersionData& theVersion) const
{
	if (mVersion == theVersion.GetVersion() &&
		mConfigName == theVersion.GetConfigName() &&
		mDescriptionURL == theVersion.GetDescriptionURL() &&
		mState == theVersion.GetState() &&
		mVersionTypeList == theVersion.GetVersionTypeList())
	{
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool VersionData::operator!=(const VersionData& theVersion) const
{
	if (mVersion		 != theVersion.GetVersion()			||
		mConfigName		 != theVersion.GetConfigName()		||
		mDescriptionURL  != theVersion.GetDescriptionURL()	||
		mState			 != theVersion.GetState()			||
		mVersionTypeList != theVersion.GetVersionTypeList()		)
	{
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PatchData::PatchData() : mConfigName(""), mFromVersion(""), mToVersion(""), mDisplayName(""),
		mNetAddress(""), mCheckSum(0), mPatchSize(0), mHostName(""), mHostURL(""),
		mHostText(""), mHostImageURL(""), mMustVisitHost(FALSE), mIsActive(FALSE) 
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PatchData::~PatchData() 
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PatchData::ReadFromBuffer(ReadBuffer& theReadBuffer, bool isGetPatchListOp)
{
	if (isGetPatchListOp)
	{
		theReadBuffer.ReadString(mConfigName);
		theReadBuffer.ReadString(mFromVersion);
		theReadBuffer.ReadString(mToVersion);
		mIsActive = theReadBuffer.ReadBool();
	}

	theReadBuffer.ReadString(mDisplayName);
	theReadBuffer.ReadString(mNetAddress);		// patch url
	mCheckSum = theReadBuffer.ReadLong();
	mPatchSize = theReadBuffer.ReadLong();
	theReadBuffer.ReadString(mHostName);
	theReadBuffer.ReadString(mHostURL);
	theReadBuffer.ReadString(mHostText);
	theReadBuffer.ReadString(mHostImageURL);
	mMustVisitHost = theReadBuffer.ReadBool();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PatchData::WriteToBuffer(WriteBuffer& theWriteBuffer, bool bUpdateOnly)
{
	theWriteBuffer.AppendString(mConfigName);
	theWriteBuffer.AppendString(mFromVersion);
	theWriteBuffer.AppendString(mToVersion);
	theWriteBuffer.AppendByte(bUpdateOnly);
	theWriteBuffer.AppendString(mDisplayName);
	theWriteBuffer.AppendString(mNetAddress);		// patch url
	theWriteBuffer.AppendLong(mCheckSum);
	theWriteBuffer.AppendLong(mPatchSize);
	theWriteBuffer.AppendString(mHostName);
	theWriteBuffer.AppendString(mHostURL);
	theWriteBuffer.AppendString(mHostText);
	theWriteBuffer.AppendString(mHostImageURL);
	theWriteBuffer.AppendByte(mMustVisitHost);
	theWriteBuffer.AppendByte(mIsActive);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool PatchData::operator==(const PatchData& thePatchData) const
{
	if (mConfigName		== thePatchData.mConfigName		&&
		mFromVersion	== thePatchData.mFromVersion	&&
		mToVersion		== thePatchData.mToVersion		&&
		mNetAddress		== thePatchData.mNetAddress		&&
		mDisplayName	== thePatchData.mDisplayName	&&
		mCheckSum		== thePatchData.mCheckSum		&&
		mPatchSize		== thePatchData.mPatchSize		&&
		mHostName		== thePatchData.mHostName		&&
		mHostURL		== thePatchData.mHostURL		&&
		mHostText		== thePatchData.mHostText		&&
		mHostImageURL	== thePatchData.mHostImageURL	&&
		mMustVisitHost	== thePatchData.mMustVisitHost	&&
		mIsActive		== thePatchData.mIsActive			)
		return true;
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool PatchData::operator!=(const PatchData& thePatchData) const
{
	if (mConfigName		!= thePatchData.mConfigName		||
		mFromVersion	!= thePatchData.mFromVersion	||
		mToVersion		!= thePatchData.mToVersion		||
		mNetAddress		!= thePatchData.mNetAddress		||
		mDisplayName	!= thePatchData.mDisplayName	||
		mCheckSum		!= thePatchData.mCheckSum		||
		mPatchSize		!= thePatchData.mPatchSize		||
		mHostName		!= thePatchData.mHostName		||
		mHostURL		!= thePatchData.mHostURL		||
		mHostText		!= thePatchData.mHostText		||
		mHostImageURL	!= thePatchData.mHostImageURL	||
		mMustVisitHost	!= thePatchData.mMustVisitHost	||
		mIsActive		!= thePatchData.mIsActive			)
		return true;
	else
		return false;

}