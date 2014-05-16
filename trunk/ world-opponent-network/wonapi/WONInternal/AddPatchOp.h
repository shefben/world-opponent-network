

#ifndef __AddPatchOP_H__
#define __AddPatchOP_H__


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "WONDB/DBProxyOp.h"					// Base Class
#include "WONMisc/PatchTypes.h"

namespace WONAPI 
{


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// AddPatch - StatusReply
// Request to add a patch to the database, will be added as valid.  Its active status is based on the specified parameter
// ·	ProductName - string 
// .	PatchData	- see PatchTypes.h
//
///////////////////////////////////////////////////////////////////////////////
class AddPatchOp: public DBProxyOp
{
	// Attributes
protected:
	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)

	std::string		mProductName;
	PatchDataPtr	mPatchData;
	bool			mIsUpdate;


	// Operations
public:

	// Accessors
	void SetPatchData	(PatchData* thePatchData)			{ mPatchData = thePatchData;					}
	void SetProductName (const std::string& theProductName)	{ mProductName = theProductName;				}
	void SetConfigName	(const std::string& theConfigName)	{ mPatchData->SetConfigName(theConfigName);		}
	void SetFromVersion	(const std::string& theFromVersion)	{ mPatchData->SetFromVersion(theFromVersion);	}
	void SetToVersion	(const std::string& theToVersion)	{ mPatchData->SetToVersion(theToVersion);		}
	void SetDisplayName	(const std::string& theDisplayName)	{ mPatchData->SetDisplayName(theDisplayName);	}
	void SetNetAddress	(const std::string& theNetAddress)	{ mPatchData->SetNetAddress(theNetAddress);		}
	void SetCheckSum	(unsigned long theCheckSum)			{ mPatchData->SetCheckSum(theCheckSum);			}
	void SetPatchSize	(unsigned long thePatchSize)		{ mPatchData->SetPatchSize(thePatchSize);		}
	void SetHostName	(const std::string& theHostName)	{ mPatchData->SetHostName(theHostName);			}
	void SetHostURL		(const std::string& theHostURL)		{ mPatchData->SetHostURL(theHostURL);			}
	void SetHostText	(const std::string& theHostText)	{ mPatchData->SetHostText(theHostText);			}
	void SetHostImageURL(const std::string& theHostImageURL){ mPatchData->SetHostImageURL(theHostImageURL);	}
	void SetMustVisitHost(bool theMustVisitHost)			{ mPatchData->SetMustVisitHost(theMustVisitHost);}
	void SetIsActive	(bool theIsActive)					{ mPatchData->SetIsActive(theIsActive);			}
	void SetIsUpdate	(bool theIsUpdate)					{ mIsUpdate = theIsUpdate; }

	PatchDataPtr		GetPatchData()			{ return mPatchData;					}
	const std::string&  GetProductName()		{ return mProductName;					}
	const std::string&  GetConfigName()			{ return mPatchData->GetConfigName();	}
	const std::string&  GetFromVersion()		{ return mPatchData->GetFromVersion();	}
	const std::string&  GetToVersion()			{ return mPatchData->GetToVersion();	}
	const std::string&  GetDisplayName()		{ return mPatchData->GetDisplayName();	}
	const std::string&  GetNetAddress()			{ return mPatchData->GetNetAddress();	}
	const unsigned long GetCheckSum()			{ return mPatchData->GetCheckSum();		}
	const unsigned long GetPatchSize()			{ return mPatchData->GetPatchSize();	}
	const std::string&  GetHostName()			{ return mPatchData->GetHostName();		}
	const std::string&  GetHostURL()			{ return mPatchData->GetHostURL();		}
	const std::string&  GetHostText()			{ return mPatchData->GetHostText();		}
	const std::string&  GetHostImageURL()		{ return mPatchData->GetHostImageURL(); }
	bool  GetMustVisitHost()					{ return mPatchData->GetMustVisitHost();}
	bool  GetIsActive()							{ return mPatchData->GetIsActive();		}
	bool  GetIsUpdate()							{ return mIsUpdate;						}

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void	  RunHook();				// Send - For Extended Pack

	// Constructor
public:
	AddPatchOp(ServerContext* theContext);			// Requires List of DBProxy servers
	AddPatchOp(const IPAddr &theAddr);				// or addr of a single server

	// Destructor
protected:
	virtual ~AddPatchOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<AddPatchOp> AddPatchOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
} // namespace WONAPI


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __AddPatchOP_H__
