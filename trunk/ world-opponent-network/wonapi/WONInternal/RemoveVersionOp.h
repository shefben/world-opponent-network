
#ifndef __RemoveVersionOP_H__
#define __RemoveVersionOP_H__


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "WONDB/DBProxyOp.h"					// Base Class


namespace WONAPI 
{


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// RemoveVersion - StatusReply
// Request to add a version to the database, will be added with state of Inactive.
// ·	ProductName - string 
// ·	ConfigName - string (optional)
// ·	Version - string
// ·	VersionType - list of strings (optional)
// ·	DescriptionURL - string (optional)
//
///////////////////////////////////////////////////////////////////////////////
class RemoveVersionOp: public DBProxyOp
{
	// Attributes
protected:
	std::string mProductName;
	std::string mConfigName;
	std::string mVersion;

	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)
	// Operations
public:

	// Accessors
	void SetProductName		(const std::string& theProductName)		{ mProductName		= theProductName;	}
	void SetConfigName		(const std::string& theConfigName)		{ mConfigName		= theConfigName;	}
	void SetVersion			(const std::string& theVersion)			{ mVersion			= theVersion;		}	

	const std::string& GetProductName()		{ return mProductName;		}
	const std::string& GetConfigName()		{ return mConfigName;		}
	const std::string& GetVersion()			{ return mVersion;			}

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void	  RunHook();				// Send - For Extended Pack

	// Constructor
public:
	RemoveVersionOp(ServerContext* theContext);			// Requires List of DBProxy servers
	RemoveVersionOp(const IPAddr &theAddr);				// or addr of a single server

	// Destructor
protected:
	virtual ~RemoveVersionOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<RemoveVersionOp> RemoveVersionOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
} // namespace WONAPI


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __RemoveVersionOP_H__
