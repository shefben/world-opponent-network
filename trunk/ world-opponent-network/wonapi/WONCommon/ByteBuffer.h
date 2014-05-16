#ifndef __WON_BYTEBUFFER_H__
#define __WON_BYTEBUFFER_H__
#include "WONShared.h"

#include <string>
#include <assert.h>
#include "SmartPtr.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ByteBuffer : public RefCount
{
protected:
	char *mData;
	unsigned long mDataLen;

public:
	ByteBuffer();
	ByteBuffer(const void *theData, unsigned long theLen, bool takeOwnership=false);
	explicit ByteBuffer(const char *theStr);

	const char* data() const { return this?mData:NULL; }
	unsigned long length() const { return this?mDataLen:0; }

protected:
	virtual ~ByteBuffer();
};

typedef ConstSmartPtr<ByteBuffer> ByteBufferPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class OffsetByteBuffer : public ByteBuffer
{
protected:
	char *mBeginData;

	virtual ~OffsetByteBuffer();

public:
	OffsetByteBuffer(void *theBeginData, unsigned long theOffset, unsigned long theDataLen); // takes ownership
};
typedef ConstSmartPtr<OffsetByteBuffer> OffsetByteBufferPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

}; // namespace WONAPI

#endif
