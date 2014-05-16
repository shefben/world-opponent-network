

#ifndef __ActivatePatchOP_H__
#define __ActivatePatchOP_H__


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "WONDB/DBProxyOp.h"					// Base Class


namespace WONAPI 
{


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// ActivatePatch - StatusReply
// Used to Activate (make public) or De-Activate a patch
// ·	ProductName - string 
// ·	ConfigName - string (optional)
// ·	FromVersion - string
// ·	ToVersion - string
// ·	NetAddress - string
// ·	IsActive - Boolean (optional -default = false)
//
///////////////////////////////////////////////////////////////////////////////
class ActivatePatchOp: public DBProxyOp
{
	// Attributes
protected:
	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)

	std::string		mProductName;
	std::string		mConfigName;
	std::string		mFromVersion;
	std::string		mToVersion;
	std::string		mNetAddress;		// patch url
	BOOL			mIsActive;

	// Operations
public:


	// Accessors
	void SetProductName(const std::string& theProductName)	{ mProductName = theProductName;	}
	void SetConfigName(const std::string& theConfigName)	{ mConfigName = theConfigName;		}
	void SetFromVersion(const std::string& theFromVersion)	{ mFromVersion = theFromVersion;	}
	void SetToVersion(const std::string& theToVersion)		{ mToVersion = theToVersion;		}
	void SetNetAddress(const std::string& theNetAddress)	{ mNetAddress = theNetAddress;		}		// patch url
	void SetIsActive(BOOL theIsActive)						{ mIsActive = theIsActive;			}

	const std::string&  GetProductName()		{ return mProductName;	}
	const std::string&  GetConfigName()			{ return mConfigName;	}
	const std::string&  GetFromVersion()		{ return mFromVersion;	}
	const std::string&  GetToVersion()			{ return mToVersion;	}
	const std::string&  GetNetAddress()			{ return mNetAddress;	}		// patch url
	BOOL  GetIsActive()							{ return mIsActive;		}

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void	  RunHook();				// Send - For Extended Pack

	// Constructor
public:
	ActivatePatchOp(ServerContext* theContext);			// Requires List of DBProxy servers
	ActivatePatchOp(const IPAddr &theAddr);				// or addr of a single server

	// Destructor
protected:
	virtual ~ActivatePatchOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<ActivatePatchOp> ActivatePatchOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
} // namespace WONAPI


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __ActivatePatchOP_H__
