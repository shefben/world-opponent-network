

#ifndef __RemovePatchOP_H__
#define __RemovePatchOP_H__


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "WONDB/DBProxyOp.h"					// Base Class


namespace WONAPI 
{


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// RemovePatch - StatusReply
// Request to remove a patch.
// ·	ProductName - string 
// ·	ConfigName - string (optional)
// ·	FromVersion - string
// ·	ToVersion - string
// ·	NetAddress - string
//
///////////////////////////////////////////////////////////////////////////////
class RemovePatchOp: public DBProxyOp
{
	// Attributes
protected:
	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)

	std::string		mProductName;
	std::string		mConfigName;
	std::string		mFromVersion;
	std::string		mToVersion;
	std::string		mNetAddress;		// patch url

	// Operations
public:

	// Accessors
	void SetProductName	(const std::string& theProductName)	{ mProductName	= theProductName;	}
	void SetConfigName	(const std::string& theConfigName)	{ mConfigName	= theConfigName;	}
	void SetFromVersion	(const std::string& theFromVersion)	{ mFromVersion	= theFromVersion;	}
	void SetToVersion	(const std::string& theToVersion)	{ mToVersion	= theToVersion;		}
	void SetNetAddress	(const std::string& theNetAddress)	{ mNetAddress	= theNetAddress;	}
	
	const std::string&  GetProductName()		{ return mProductName;	}
	const std::string&  GetConfigName()			{ return mConfigName;	}
	const std::string&  GetFromVersion()		{ return mFromVersion;	}
	const std::string&  GetToVersion()			{ return mToVersion;	}
	const std::string&  GetNetAddress()			{ return mNetAddress;	}		// patch url

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void	  RunHook();				// Send - For Extended Pack

	// Constructor
public:
	RemovePatchOp(ServerContext* theContext);			// Requires List of DBProxy servers
	RemovePatchOp(const IPAddr &theAddr);				// or addr of a single server

	// Destructor
protected:
	virtual ~RemovePatchOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<RemovePatchOp> RemovePatchOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
} // namespace WONAPI


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __RemovePatchOP_H__
