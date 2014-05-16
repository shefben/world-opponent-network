
#ifndef KeySubscribeOp_H_
#define KeySubscribeOp_H_

#include "WONDB/DBProxyOp.h"
#include <list>

namespace WONAPI 
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class BaseKeySubscribeOp: public DBProxyOp
{
	// Attributes
public:
	BaseKeySubscribeOp(ServerContext* theContext);

	std::wstring           mCommunityName; // [in]
	std::string            mProductName;   // [in]
	std::list<std::string> mKeyList;       // [in]

	std::list<short>       mStatusList;	  // [out]

	// Overrides
protected:
	void Pack(WriteBuffer& theBufferR);
	WONStatus Unpack(ReadBuffer& theBufferR);

	// Destructor
protected:
	virtual ~BaseKeySubscribeOp() {}		// Safety Lock -- Enforces SmartPtr Use.
};
typedef SmartPtr<BaseKeySubscribeOp> BaseKeySubscribeOpPtr;		// Smart Ptr Template

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// This Op is called by the client to insert a list of keys into the database
class InsertKeySubscribeOp: public BaseKeySubscribeOp
{
	// Attributes
public:
	InsertKeySubscribeOp(ServerContext* theContext);

	time_t        mAllocateDate;
	unsigned long mAllocateDuration;
	time_t        mActivateTime;
	unsigned long mActivateDuration;

	// Overrides
protected:
	enum MSGTYPE { MSGTYPE = 1 };				// DO NOT MODIFY (MUST MATCH WITH SERVER)
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void RunHook();						// Send - For Extended Pack
	virtual void Reset(){}

	// Destructor
protected:
	virtual ~InsertKeySubscribeOp() {}		// Safety Lock -- Enforces SmartPtr Use.
};
typedef SmartPtr<InsertKeySubscribeOp> InsertKeySubscribeOpPtr;		// Smart Ptr Template

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// This Op is called by the client to insert a list of keys into the database
class UpdateKeySubscribeOp: public BaseKeySubscribeOp
{
	// Attributes
public:
	UpdateKeySubscribeOp(ServerContext* theContext);

	// AllocateDate     0x0001
	// AllocateDuration 0x0002
	// ActivateTime     0x0004
	// ActivateDuration 0x0008
	// ActivateUserSeq  0x0010

	void SetAllocateDate	(time_t theAllocateDate)	 { mAllocateDate	 = theAllocateDate;		mUpdateFields |= 0x0001; }
	void SetAllocateDuration(time_t theAllocateDuration) { mAllocateDuration = theAllocateDuration;	mUpdateFields |= 0x0002; }
	void SetActivateTime	(time_t theActivateTime)	 { mActivateTime	 = theActivateTime;		mUpdateFields |= 0x0004; }
	void SetActivateDuration(time_t theActivateDuration) { mActivateDuration = theActivateDuration;	mUpdateFields |= 0x0008; }
	void SetActivateUserSeq	(time_t theActivateUserSeq)	 { mActivateUserSeq	 = theActivateUserSeq;	mUpdateFields |= 0x0010; }

	void UnsetAllocateDate		(void) { mAllocateDate		= -1;	mUpdateFields &= ~0x0001; }
	void UnsetAllocateDuration	(void) { mAllocateDuration	=  0;	mUpdateFields &= ~0x0002; }
	void UnsetActivateTime		(void) { mActivateTime		= -1;	mUpdateFields &= ~0x0004; }
	void UnsetActivateDuration	(void) { mActivateDuration	=  0;	mUpdateFields &= ~0x0008; }
	void UnsetActivateUserSeq	(void) { mActivateUserSeq	=  0;	mUpdateFields &= ~0x0010; }

	bool IsUpdateAllocDate	      (void) { return (mUpdateFields & 0x0001 != 0); }
	bool IsUpdateAllocDuration    (void) { return (mUpdateFields & 0x0002 != 0); }
	bool IsUpdateActivateDate     (void) { return (mUpdateFields & 0x0004 != 0); }
	bool IsUpdateActivateDuration (void) { return (mUpdateFields & 0x0008 != 0); }
	bool IsUpdateActivateUserSeq  (void) { return (mUpdateFields & 0x0010 != 0); }

	time_t        GetAllocDate	      (void) { return mAllocateDate; }
	unsigned long GetAllocDuration    (void) { return mAllocateDuration; }
	time_t        GetActivateDate     (void) { return mActivateTime; }
	unsigned long GetActivateDuration (void) { return mActivateDuration; }
	unsigned long GetActivateUserSeq  (void) { return mActivateUserSeq; }

protected:
	unsigned short mUpdateFields;
	time_t         mAllocateDate;
	unsigned long  mAllocateDuration;
	time_t         mActivateTime;
	unsigned long  mActivateDuration;
	unsigned long  mActivateUserSeq;

	// Overrides
protected:
	enum MSGTYPE { MSGTYPE = 5 };				// DO NOT MODIFY (MUST MATCH WITH SERVER)
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void RunHook();						// Send - For Extended Pack
	virtual void Reset(){}

	// Destructor
protected:
	virtual ~UpdateKeySubscribeOp() {}		// Safety Lock -- Enforces SmartPtr Use.
};
typedef SmartPtr<UpdateKeySubscribeOp> UpdateKeySubscribeOpPtr;		// Smart Ptr Template

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// This Op is called by the client to insert a list of keys into the database
class RemoveKeySubscribeOp: public BaseKeySubscribeOp
{
	// Attributes
public:
	RemoveKeySubscribeOp(ServerContext* theContext);

	// Overrides
protected:
	enum MSGTYPE { MSGTYPE = 7 };				// DO NOT MODIFY (MUST MATCH WITH SERVER)
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void RunHook();						// Send - For Extended Pack
	virtual void Reset(){}

	// Destructor
protected:
	virtual ~RemoveKeySubscribeOp() {}		// Safety Lock -- Enforces SmartPtr Use.
};
typedef SmartPtr<RemoveKeySubscribeOp> RemoveKeySubscribeOpPtr;		// Smart Ptr Template

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class QueryKeySubscribeOp: public DBProxyOp
{
	// Attributes
public:
	QueryKeySubscribeOp(ServerContext* theContext);

	std::wstring           mCommunityName; // [in]
	std::string            mProductName;   // [in]
	std::list<std::string> mKeyList;       // [in]

	struct KEY_DATA
	{
		WONStatus     mStatus;
		time_t        mAllocateDate;
		unsigned long mAllocateDuration;
		time_t        mActivateTime;
		unsigned long mActivateDuration;
		unsigned long mActivateUserSeq;
	};
	typedef std::list<KEY_DATA> KEY_DATA_LIST;
	KEY_DATA_LIST mKeyDataList;            // [out]

	// Overrides
protected:
	enum MSGTYPE { MSGTYPE = 3 };				// DO NOT MODIFY (MUST MATCH WITH SERVER)
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void RunHook();						// Send - For Extended Pack
	virtual void Reset(){}

	// Destructor
protected:
	virtual ~QueryKeySubscribeOp() {}		// Safety Lock -- Enforces SmartPtr Use.
};
typedef SmartPtr<QueryKeySubscribeOp> QueryKeySubscribeOpPtr;		// Smart Ptr Template

};

#endif