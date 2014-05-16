
#ifndef __INSERTBANNEDKEYOP_H__
#define __INSERTBANNEDKEYOP_H__

#include "WONDB/DBProxyOp.h"

namespace WONAPI 
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// This Op is called by the client to insert a set of banned keys into the database
class InsertBannedKeyOp: public DBProxyOp
{
	// Attributes
public:
	InsertBannedKeyOp(ServerContext* theContext);

	std::wstring mCommunityName;    // [in]
	std::string  mProductName;	    // [in]	
	time_t       mBannedUntil;	    // [in]

	std::list<std::string> mKeyList;// [in]
	std::list<short>  mStatusList;  // [out]

	// Overrides
protected:
	enum MSGTYPE { MSGTYPE = 20 };				// DO NOT MODIFY (MUST MATCH WITH SERVER)
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void RunHook();						// Send - For Extended Pack
	virtual void Reset();

	// Destructor
protected:
	virtual ~InsertBannedKeyOp() {}		// Safety Lock -- Enforces SmartPtr Use.
};
typedef SmartPtr<InsertBannedKeyOp> InsertBannedKeyOpPtr;		// Smart Ptr Template

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// This Op is called by the client to query a set of keys out of the database
class QueryBannedKeyOp: public DBProxyOp
{
	// Attributes
public:
	QueryBannedKeyOp(ServerContext* theContext);

	std::wstring mCommunityName;   // [in]
	std::string  mProductName;	   // [in]	

	typedef std::pair<std::string, time_t> BANNED_KEY_PAIR;
	typedef std::list<BANNED_KEY_PAIR>     BANNED_KEY_LIST;
	BANNED_KEY_LIST mKeyList;      // [out]

	// Overrides
protected:
	enum MSGTYPE { MSGTYPE = 32 };				// DO NOT MODIFY (MUST MATCH WITH SERVER)
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void RunHook();						// Send - For Extended Pack
	virtual void Reset(){}

	// Destructor
protected:
	virtual ~QueryBannedKeyOp() {}		// Safety Lock -- Enforces SmartPtr Use.
};
typedef SmartPtr<QueryBannedKeyOp> QueryBannedKeyOpPtr;		// Smart Ptr Template

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// This Op is called by the client to query a set of keys out of the database
class QueryListBannedKeyOp: public DBProxyOp
{
	// Attributes
public:
	QueryListBannedKeyOp(ServerContext* theContext);

	std::wstring mCommunityName;     // [in]
	std::string  mProductName;	     // [in]	
	std::list<std::string> mKeyList; // [in]

	typedef std::pair<time_t, short>   BANNED_KEY_PAIR;
	typedef std::list<BANNED_KEY_PAIR> BANNED_KEY_LIST;
	BANNED_KEY_LIST mKeyDataList;    // [out]

	// Overrides
protected:
	enum MSGTYPE { MSGTYPE = 34 };				// DO NOT MODIFY (MUST MATCH WITH SERVER)
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void RunHook();						// Send - For Extended Pack
	virtual void Reset(){}

	// Destructor
protected:
	virtual ~QueryListBannedKeyOp() {}		// Safety Lock -- Enforces SmartPtr Use.
};
typedef SmartPtr<QueryListBannedKeyOp> QueryListBannedKeyOpPtr;		// Smart Ptr Template

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// This Op is called by the client to delete a set of banned keys out of the database
class RemoveBannedKeyOp: public DBProxyOp
{
	// Attributes
public:
	RemoveBannedKeyOp(ServerContext* theContext);

	std::wstring mCommunityName;     // [in]
	std::string  mProductName;	     // [in]	

	std::list<std::string> mKeyList; // [in]
	std::list<short>  mStatusList;   // [out]

	// Overrides
protected:
	enum MSGTYPE { MSGTYPE = 30 };				// DO NOT MODIFY (MUST MATCH WITH SERVER)
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void RunHook();						// Send - For Extended Pack
	virtual void Reset();

	// Destructor
protected:
	virtual ~RemoveBannedKeyOp() {}		// Safety Lock -- Enforces SmartPtr Use.
};
typedef SmartPtr<RemoveBannedKeyOp> RemoveBannedKeyOpPtr;		// Smart Ptr Template

} // WONAPI

#endif // #ifndef __INSERTBANNEDKEYOP_H__
