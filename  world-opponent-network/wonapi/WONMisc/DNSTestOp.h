///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifndef __WON_DNSTestOP_H__
#define __WON_DNSTestOP_H__


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "WONShared.h"
#include "WONSocket/IPAddr.h"
#include "WONCommon/AsyncOp.h"
#include "WONCommon/Thread.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace WONAPI
{


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class DNSTestOp: public AsyncOp
{
	friend class DNSTestThread;

	// Attributes
protected:
	std::string	mHostString;	// pre-lookup
	IPAddr		mHostAddr;		// post lookup

	// Operations
public:
	const std::string&	GetHostString() { return mHostString; }
	const IPAddr&		GetHostAddr()	{ return mHostAddr; }

protected:
	virtual void RunHook();				// Send

	// Constructor
public:
	DNSTestOp(const std::string &theHost);

protected:
	~DNSTestOp();		// Enforce SmartPtr use

};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef SmartPtr<DNSTestOp> DNSTestOpPtr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class DNSTestThread: public Thread
{
public:
	DNSTestOpPtr mOp;

public:
	DNSTestThread(DNSTestOp* theOp) : Thread(), mOp(theOp) {}
	virtual void ThreadFunc(void);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
}; // namespace WONAPI


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif
