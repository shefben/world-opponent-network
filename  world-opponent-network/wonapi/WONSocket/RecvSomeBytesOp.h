#ifndef __WON_RECVSOMEBYTESOP_H__
#define __WON_RECVSOMEBYTESOP_H__


#include "SocketOp.h"

namespace WONAPI
{

class RecvSomeBytesOp : public SocketOp
{
protected:
	char *mCurRecv;
	ByteBufferPtr mRecvBytes;
	unsigned short mMaxBytes;
	bool mDoDelay;
	DWORD mStartTime;

	virtual ~RecvSomeBytesOp();

	virtual WONStatus Start();
	virtual WONStatus Continue();
	virtual SocketOp* Duplicate() { return new RecvSomeBytesOp(mMaxBytes,mSocket,mDoDelay); }

public:
	static DWORD mInitDelay;

public:
	RecvSomeBytesOp(unsigned short theMaxBytes = 1024, AsyncSocket *theSocket = NULL, bool doDelay = false);

	const ByteBuffer* GetBytes() const { return mRecvBytes; }

};

}; // namespace WONAPI

#endif