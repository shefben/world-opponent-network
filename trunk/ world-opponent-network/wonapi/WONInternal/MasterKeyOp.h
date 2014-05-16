
#ifndef __INSERTMASTERKEYOP_H__
#define __INSERTMASTERKEYOP_H__

#include "WONDB/DBProxyOp.h"
#include <list>

namespace WONAPI 
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// This Op is called by the client to insert a master key (bfk) into the database
class InsertMasterKeyOp: public DBProxyOp
{
	// Attributes
public:
	InsertMasterKeyOp(ServerContext* theContext);

	std::wstring   mCommunityName;	// [in]
	std::string    mProductName;    // [in]
	unsigned short mSequenceNumber;	// [in]	
	bool           mIsBeta;			// [in]
	time_t         mActivateTime;	// [in]
	time_t         mDeactivateTime;	// [in]
	BYTE           mKeyLength;		// [in]
	BYTE           mKeyP[20];		// [in]
	unsigned long  mCDKeyFlags;     // [in]

	// Overrides
protected:
	enum MSGTYPE { MSGTYPE = 18 };				// DO NOT MODIFY (MUST MATCH WITH SERVER)
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void RunHook();						// Send - For Extended Pack
	virtual void Reset(){}

	// Destructor
protected:
	virtual ~InsertMasterKeyOp() {}		// Safety Lock -- Enforces SmartPtr Use.
};
typedef SmartPtr<InsertMasterKeyOp> InsertMasterKeyOpPtr;		// Smart Ptr Template

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// This Op is called by the client to update a master key (bfk) into the database
class UpdateMasterKeyOp: public DBProxyOp
{
	// Attributes
public:
	UpdateMasterKeyOp(ServerContext* theContext);

	std::wstring   mCommunityName;	// [in]
	std::string    mProductName;    // [in]
	unsigned short mSequenceNumber;	// [in]	
	bool           mIsBeta;			// [in]
	time_t         mActivateTime;	// [in]
	time_t         mDeactivateTime;	// [in]
	unsigned long  mCDKeyFlags;     // [in]

	// Overrides
protected:
	enum MSGTYPE { MSGTYPE = 22 };				// DO NOT MODIFY (MUST MATCH WITH SERVER)
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void RunHook();						// Send - For Extended Pack
	virtual void Reset(){}

	// Destructor
protected:
	virtual ~UpdateMasterKeyOp() {}		// Safety Lock -- Enforces SmartPtr Use.
};
typedef SmartPtr<UpdateMasterKeyOp> UpdateMasterKeyOpPtr;		// Smart Ptr Template

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// This Op is called by the client to delete a master key (bfk) into the database
class DeleteMasterKeyOp: public DBProxyOp
{
	// Attributes
public:
	DeleteMasterKeyOp(ServerContext* theContext);

	std::wstring   mCommunityName;	// [in]
	std::string    mProductName;    // [in]
	unsigned short mSequenceNumber;	// [in]	

	// Overrides
protected:
	enum MSGTYPE { MSGTYPE = 24 };				// DO NOT MODIFY (MUST MATCH WITH SERVER)
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void RunHook();						// Send - For Extended Pack
	virtual void Reset(){}

	// Destructor
protected:
	virtual ~DeleteMasterKeyOp() {}		// Safety Lock -- Enforces SmartPtr Use.
};
typedef SmartPtr<DeleteMasterKeyOp> DeleteMasterKeyOpPtr;		// Smart Ptr Template

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// This Op is called by the client to query master key(s) (bfk) from the database
class QueryMasterKeyOp: public DBProxyOp
{
	// Attributes
public:
	QueryMasterKeyOp(ServerContext* theContext);

	std::wstring   mCommunityName;	// [in]
	std::string    mProductName;    // [in]

	struct MasterKey
	{
		unsigned short mSequenceNumber;
		time_t         mActivateDate;
		time_t         mDeactivateDate;
		bool           mIsBeta;
		unsigned long  mCDKeyFlags;
	};

	typedef std::list<MasterKey>			MASTER_KEY_LIST;
	typedef MASTER_KEY_LIST::const_iterator MASTER_KEY_LIST_CITR;
	MASTER_KEY_LIST mMasterKeyList;	// [out]	

	// Overrides
protected:
	enum MSGTYPE { MSGTYPE = 26 };				// DO NOT MODIFY (MUST MATCH WITH SERVER)
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void RunHook();						// Send - For Extended Pack
	virtual void Reset(){}

	// Destructor
protected:
	virtual ~QueryMasterKeyOp() {}		// Safety Lock -- Enforces SmartPtr Use.
};
typedef SmartPtr<QueryMasterKeyOp> QueryMasterKeyOpPtr;		// Smart Ptr Template
typedef ConstSmartPtr<QueryMasterKeyOp> ConstQueryMasterKeyOpPtr;		// Smart Ptr Template

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// This Op is called by the client to verify CDKeys from the auth server
class AuthCheckKeyOp: public DBProxyOp
{
	// Attributes
public:
	AuthCheckKeyOp(ServerContext* theContext);

	std::wstring   mCommunityName;	// [in]
	std::string    mProductName;	// [in]

	typedef std::list<std::string>	 KEY_LIST;
	typedef KEY_LIST::const_iterator KEY_LIST_CITR;
	KEY_LIST mKeyList;      	    // [in]	

	typedef std::list<short>		        KEY_STATUS_LIST;
	typedef KEY_STATUS_LIST::const_iterator KEY_STATUS_LIST_CITR;
	KEY_STATUS_LIST mKeyStatusList; // [out]	

	// Overrides
protected:
	enum MSGTYPE { MSGTYPE = 28 };				// DO NOT MODIFY (MUST MATCH WITH SERVER)
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void RunHook();						// Send - For Extended Pack
	virtual void Reset(){}

	// Destructor
protected:
	virtual ~AuthCheckKeyOp() {}		// Safety Lock -- Enforces SmartPtr Use.
};
typedef SmartPtr<AuthCheckKeyOp> AuthCheckKeyOpPtr;		// Smart Ptr Template
typedef ConstSmartPtr<AuthCheckKeyOp> ConstAuthCheckKeyOpPtr;		// Smart Ptr Template

} // WONAPI

#endif // #ifndef __INSERTMASTERKEYOP_H__
