///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifndef __PATCHTYPES_H__
#define __PATCHTYPES_H__

#include "WONCommon/SmartPtr.h"
#include "WONCommon/WriteBuffer.h"
#include "WONCommon/ReadBuffer.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace WONAPI
{


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum VersionState
{
	VersionState_Invalid = 0,
	VersionState_Inactive = 1,
	VersionState_Active = 2,
	VersionState_BadState = 3
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef std::list<std::string>	VersionTypeList;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class VersionData: public RefCount
{
private:
	std::string		mConfigName;
	std::string		mVersion;
	std::string		mDescriptionURL;
	VersionState	mState;

	VersionTypeList	mVersionTypeList;

public:
	enum
	{
		WriteFlag_AppendState			= 1,
		WriteFlag_AppendDescripURL		= 2
	};

	// Accessors
	void SetConfigName		(const std::string& theConfigName)		{ mConfigName		= theConfigName;	}
	void SetVersion			(const std::string& theVersion)			{ mVersion			= theVersion;		}
	void SetDescriptionURL	(const std::string& theDescriptionURL)	{ mDescriptionURL	= theDescriptionURL;}
	void SetState			(const VersionState theState)			{ mState			= theState;			}

	const std::string& GetConfigName()			const { return mConfigName;		 }
	const std::string& GetVersion()				const { return mVersion;		 }
	const std::string& GetDescriptionURL()		const { return mDescriptionURL;	 }
	const VersionState GetState()				const { return mState;			 }
	const VersionTypeList& GetVersionTypeList() const { return mVersionTypeList; }

	void AddVersionType		(const std::string& theVersionType);

	// Serialization
	void ReadFromBuffer(ReadBuffer& theBuffer);
	void WriteToBuffer(WriteBuffer& theWriteBuffer, DWORD theFlags = 0);

	bool operator==(const VersionData& theVersionData) const;
	bool operator!=(const VersionData& theVersionData) const;
	bool operator< (const VersionData& theVersionData) const;	// uses mConfigName,mVersion,this

public:
	VersionData();

protected:
	virtual ~VersionData();
};



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// PatchData - Contains relevant information on a patch
// ·	ConfigName - string (optional)
// ·	FromVersion - string
// ·	ToVersion - string
// ·	DisplayName - string 
// ·	NetAddress - string
// ·	Checksum - unsigned long
// ·	PatchSize - unsigned long
// ·	HostName - string
// ·	HostURL - string (optional)
// ·	HostText - string (optional)
// ·	HostImageURL - string (optional)
// ·	MustVisitHost - Boolean (optional, default = false)
// ·	IsActive - Boolean (optional -default = false)
//
class PatchData: public RefCount
{
private:
	std::string		mConfigName;
	std::string		mFromVersion;
	std::string		mToVersion;
	std::string		mDisplayName;
	std::string		mNetAddress;		// patch url
	unsigned long	mCheckSum;
	unsigned long	mPatchSize;
	std::string		mHostName;
	std::string		mHostURL;
	std::string		mHostText;
	std::string		mHostImageURL;
	bool			mMustVisitHost;
	bool			mIsActive;



public:
	// Accessors
	void SetConfigName	(const std::string& theConfigName)	{ mConfigName = theConfigName;		}
	void SetFromVersion	(const std::string& theFromVersion)	{ mFromVersion = theFromVersion;	}
	void SetToVersion	(const std::string& theToVersion)	{ mToVersion = theToVersion;		}
	void SetDisplayName	(const std::string& theDisplayName)	{ mDisplayName = theDisplayName;	}
	void SetNetAddress	(const std::string& theNetAddress)	{ mNetAddress = theNetAddress;		}		// patch url
	void SetCheckSum	(unsigned long theCheckSum)			{ mCheckSum = theCheckSum;			}
	void SetPatchSize	(unsigned long thePatchSize)		{ mPatchSize = thePatchSize;		}
	void SetHostName	(const std::string& theHostName)	{ mHostName = theHostName;			}
	void SetHostURL		(const std::string& theHostURL)		{ mHostURL = theHostURL;			}
	void SetHostText	(const std::string& theHostText)	{ mHostText = theHostText;			}
	void SetHostImageURL(const std::string& theHostImageURL){ mHostImageURL = theHostImageURL;	}
	void SetMustVisitHost(bool theMustVisitHost)			{ mMustVisitHost = theMustVisitHost;}
	void SetIsActive	(bool theIsActive)					{ mIsActive = theIsActive;			}

	const std::string&  GetConfigName()		const	{ return mConfigName;	}
	const std::string&  GetFromVersion()	const	{ return mFromVersion;	}
	const std::string&  GetToVersion()		const	{ return mToVersion;	}
	const std::string&  GetDisplayName()	const	{ return mDisplayName;	}
	const std::string&  GetNetAddress()		const	{ return mNetAddress;	}		// patch url
	const unsigned long GetCheckSum()		const	{ return mCheckSum;		}
	const unsigned long GetPatchSize()		const	{ return mPatchSize;	}
	const std::string&  GetHostName()		const	{ return mHostName;		}
	const std::string&  GetHostURL()		const	{ return mHostURL;		}
	const std::string&  GetHostText()		const	{ return mHostText;		}
	const std::string&  GetHostImageURL()	const	{ return mHostImageURL; }
	bool  GetMustVisitHost()				const	{ return mMustVisitHost;}
	bool  GetIsActive()						const	{ return mIsActive;		}


	// GetPatchListOp, CheckValidVersionOp
	void ReadFromBuffer(ReadBuffer& theReadBuffer, bool isGetPatchListOp = false);

	// AddPatchOp
	void WriteToBuffer(WriteBuffer& theWriteBuffer, bool bUpdateOnly=false);

	// Comparison
	bool operator==(const PatchData& thePatchData) const;
	bool operator!=(const PatchData& thePatchData) const;
	bool operator< (const PatchData& thePatchData) const;

public:
	PatchData();

protected:
	virtual ~PatchData();

};


///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
typedef SmartPtr<VersionData>		VersionDataPtr;	
typedef std::list<VersionDataPtr>	VersionDataList;
typedef SmartPtr<PatchData>			PatchDataPtr;
typedef std::list<PatchDataPtr>		PatchDataList;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
} // namespace WONAPI


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // __PATCHTYPES_H__