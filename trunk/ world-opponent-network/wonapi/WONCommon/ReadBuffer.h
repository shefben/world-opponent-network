#ifndef __WON_READBUFFER_H__
#define __WON_READBUFFER_H__
#include "WONShared.h"

#include "ByteBuffer.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class ReadBuffer 
{
protected:
	int mReadPos;
	const char *mData;
	unsigned long mDataLen;

	unsigned char mCurBit;
	unsigned char mCurByte;

	unsigned long ReadLength(unsigned char theLengthFieldSize);

public:
	ReadBuffer();
	ReadBuffer(const void *theData, unsigned long theDataLen);
	void SetData(const void *theData, unsigned long theDataLen);

	const char* data() const { return mData; }
	unsigned long length() const { return mDataLen; }

	// Read
	const void* ReadBytes(unsigned long theAmount);
	bool ReadBool();
	bool ReadBit();
	char ReadByte();
	short ReadShort();
	long ReadLong();
	__int64 ReadInt64();
	void ReadRawString(std::string &theStr, unsigned long theNumChars);
	void ReadRawWString(std::wstring &theWStr, unsigned long theNumChars);
	void ReadString(std::string &theStr, unsigned char theLengthFieldSize = 2);
	void ReadWString(std::wstring &theWStr, unsigned char theLengthFieldSize = 2);
	ByteBufferPtr ReadBuf(unsigned char theLengthFieldSize);

	void Rewind();
	int pos() { return mReadPos; }
	void SetPos(int thePos) { mReadPos = thePos; }

	int Available() { return length() - pos(); }
	bool HasMoreBytes() { return pos() < (int)length(); }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class ReadBufferException : public std::exception
{
public:
	std::string what;
	ReadBufferException() : exception() { }
	ReadBufferException(const std::string &theWhat) { what = theWhat; }
};

}; // namespace WONAPI

#endif
