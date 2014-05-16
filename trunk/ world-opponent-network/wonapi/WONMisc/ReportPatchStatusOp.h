

#ifndef __ReportPatchStatusOp_H__
#define __ReportPatchStatusOp_H__


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "WONDB/DBProxyOp.h"					// Base Class


namespace WONAPI 
{


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// ReportPatchStatus - StatusReply
// Report the status of a requested patch download.
// ·	ProductName - string 
// ·	ConfigName - string (optional)
// ·	FromVersion - string
// ·	ToVersion - string
// ·	NetAddress - string
// .    PatchStatus - BYTE
//
///////////////////////////////////////////////////////////////////////////////
class ReportPatchStatusOp: public DBProxyOp
{
	// Attributes
protected:
	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)

	std::string		mProductName;
	std::string		mConfigName;
	std::string		mFromVersion;
	std::string		mToVersion;
	std::string		mNetAddress;		// patch url
	BYTE            mPatchStatus;

	AsyncSocketPtr mUDPSocket;

	// Operations
public:

	// Accessors
	void SetProductName	(const std::string& theProductName)	{ mProductName	= theProductName;	}
	void SetConfigName	(const std::string& theConfigName)	{ mConfigName	= theConfigName;	}
	void SetFromVersion	(const std::string& theFromVersion)	{ mFromVersion	= theFromVersion;	}
	void SetToVersion	(const std::string& theToVersion)	{ mToVersion	= theToVersion;		}
	void SetNetAddress	(const std::string& theNetAddress)	{ mNetAddress	= theNetAddress;	}
	void SetPatchStatus	(BYTE thePatchStatus)				{ mPatchStatus	= thePatchStatus;	}
	void SetUDPSocket(AsyncSocket *theSocket)				{ mUDPSocket	= theSocket;		}
	
	const std::string&  GetProductName()		{ return mProductName;	}
	const std::string&  GetConfigName()			{ return mConfigName;	}
	const std::string&  GetFromVersion()		{ return mFromVersion;	}
	const std::string&  GetToVersion()			{ return mToVersion;	}
	const std::string&  GetNetAddress()			{ return mNetAddress;	}		// patch url
	BYTE  GetPatchStatus()						{ return mPatchStatus;	}		

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void	  RunHook();				// Send - For Extended Pack

	// Constructor
public:
	ReportPatchStatusOp(const IPAddr &theAddr);	// Address of a single DBProxy server

	// Destructor
protected:
	virtual ~ReportPatchStatusOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<ReportPatchStatusOp> ReportPatchStatusOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
} // namespace WONAPI


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __ReportPatchStatusOp_H__
