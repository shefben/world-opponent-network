
#ifndef __ADDVERSIONOP_H__
#define __ADDVERSIONOP_H__


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "WONDB/DBProxyOp.h"					// Base Class
#include "WONMisc/PatchTypes.h"

namespace WONAPI 
{


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// AddVersion - StatusReply
// Request to add a version to the database, will be added with state of Inactive.
// ·	ProductName - string 
// ·	ConfigName - string (optional)
// ·	Version - string
// ·	VersionType - list of strings (optional)
// ·	DescriptionURL - string (optional)
//
///////////////////////////////////////////////////////////////////////////////
class AddVersionOp: public DBProxyOp
{
	// Attributes
protected:
	std::string		mProductName;
	VersionDataPtr	mVersionData;				// see PatchTypes.h"

	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)
	// Operations
public:

	// Accessors
	void SetProductName		(const std::string& theProductName)		{ mProductName		= theProductName;					}
	void SetConfigName		(const std::string& theConfigName)		{ mVersionData->SetConfigName		(theConfigName);	}
	void SetVersion			(const std::string& theVersion)			{ mVersionData->SetVersion			(theVersion);		}
	void SetDescriptionURL	(const std::string& theDescriptionURL)	{ mVersionData->SetDescriptionURL	(theDescriptionURL);}
	void AddVersionType		(const std::string& theVersionType)		{ mVersionData->AddVersionType		(theVersionType);	}

	const std::string& GetProductName()		{ return mProductName;		}
	const std::string& GetConfigName()		{ return mVersionData->GetConfigName();		}
	const std::string& GetVersion()			{ return mVersionData->GetVersion();		}
	const std::string& GetDescriptionURL()	{ return mVersionData->GetDescriptionURL();	}
	const VersionTypeList& GetVersionTypeList() { return mVersionData->GetVersionTypeList(); }

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void	  RunHook();				// Send - For Extended Pack

	// Constructor
public:
	AddVersionOp(ServerContext* theContext);	// Requires gamename and list of patch servers
	AddVersionOp(const IPAddr &theAddr);		// or addr of a single server

	// Destructor
protected:
	virtual ~AddVersionOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<AddVersionOp> AddVersionOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
} // namespace WONAPI


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __ADDVERSIONOP_H__
