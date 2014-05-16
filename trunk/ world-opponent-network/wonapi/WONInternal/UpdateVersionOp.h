
#ifndef __UPDATEVERSIONOP_H__
#define __UPDATEVERSIONOP_H__


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "WONDB/DBProxyOp.h"					// Base Class
#include "WONMisc/PatchTypes.h"

namespace WONAPI 
{


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// SetVersionState - StatusReply
// Request to change the state of an existing version.
// ·	ProductName - string 
// ·	ConfigName - string (optional)
// ·	Version - string
// ·	State - byte (Invalid / Inactive / Active)
//
///////////////////////////////////////////////////////////////////////////////
class UpdateVersionOp: public DBProxyOp
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
	void SetState	(VersionState theVersionState)					{ mVersionData->SetState			(theVersionState);	}

	const std::string&		GetProductName()	{ return mProductName;						}
	const std::string&		GetConfigName()		{ return mVersionData->GetConfigName();		}
	const std::string&		GetVersion()		{ return mVersionData->GetVersion();		}
	const std::string&		GetDescriptionURL()	{ return mVersionData->GetDescriptionURL();	}
	const VersionTypeList&	GetVersionTypeList(){ return mVersionData->GetVersionTypeList();}
	VersionState			GetState()			{ return mVersionData->GetState();			}

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void	  RunHook();				// Send - For Extended Pack

	// Constructor
public:
	UpdateVersionOp(ServerContext* theContext);	// Requires gamename and list of patch servers
	UpdateVersionOp(const IPAddr &theAddr);		// or addr of a single server

	// Destructor
protected:
	virtual ~UpdateVersionOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<UpdateVersionOp> UpdateVersionOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
} // namespace WONAPI


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __UPDATEVERSIONOP_H__
