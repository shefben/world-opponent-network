
#ifndef __GetVersionListOP_H__
#define __GetVersionListOP_H__


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "WONDB/DBProxyOp.h"					// Base Class
#include "WONMisc/PatchTypes.h"


namespace WONAPI 
{


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// GetVersionList - GetVersionListReply
// Request a list of valid versions using various filter methods
// ·	ProductName - string 
// ·	ConfigName	- string	(optional)
// ·	State		- byte		(Invalid, Inactive, Active) (optional)
// .    VersionData - see PatchTypes.h
//
///////////////////////////////////////////////////////////////////////////////
class GetVersionListOp: public DBProxyOp
{
	// Attributes
protected:
	std::string			mProductName;
	std::string			mConfigName;
	VersionState		mStateFilter;
	VersionDataList		mVersionDataList;

	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)
	// Operations
public:

	// Accessors
	void SetProductName		(const std::string& theProductName)		{ mProductName		= theProductName;	}
	void SetConfigName		(const std::string& theConfigName)		{ mConfigName		= theConfigName;	}
	void SetStateFilter		(VersionState		theStateFilter)		{ mStateFilter		= theStateFilter;	}

	const std::string& GetProductName()		{ return mProductName;	}
	const std::string& GetConfigName()		{ return mConfigName;	}
	VersionState	   GetStateFilter()		{ return mStateFilter;	}
	const VersionDataList&	GetVersionDataList()	{ return mVersionDataList; }

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void	  RunHook();				// Send - For Extended Pack

	// Constructor
public:
	GetVersionListOp(const std::string& productName, ServerContext* theContext);	// Requires gamename and list of patch servers
	GetVersionListOp(const std::string& productName, const IPAddr &theAddr);		// or addr of a single server

	// Destructor
protected:
	virtual ~GetVersionListOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<GetVersionListOp> GetVersionListOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
} // namespace WONAPI


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __GetVersionListOP_H__
