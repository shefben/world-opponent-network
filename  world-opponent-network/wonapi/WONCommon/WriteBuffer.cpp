#include "WriteBuffer.h"
#include "LittleEndian.h"
using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::Reset()
{
	delete [] mBeginData;

	mData = mBeginData = NULL;
	mDataLen = 0;
	mCapacity = 0;
	mCurBit = 0;

//	if(mLengthFieldSize>0)
//		SkipBytes(mLengthFieldSize);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::Release()
{
	mData = mBeginData = NULL;
	Reset();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WriteBuffer::WriteBuffer(unsigned char theLengthFieldSize, bool lengthIncludesLengthFieldSize) 
{
	mData = mBeginData = NULL;
	mLengthFieldSize = theLengthFieldSize;
	mLengthIncludesLengthFieldSize = lengthIncludesLengthFieldSize;

	Reset();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WriteBuffer::~WriteBuffer()
{
	delete [] mBeginData;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::SetLengthFieldSize(unsigned char theLengthFieldSize, bool lengthIncludesLengthFieldSize)
{
/*	if(mLengthFieldSize==theLengthFieldSize && mLengthIncludesLengthFieldSize==lengthIncludesLengthFieldSize)
		return;

	int aDiff = mLengthFieldSize - theLengthFieldSize;
	if(aDiff > 0)
	{
		if(mDataLen > aDiff)
			memmove(mData,mData+aDiff,mDataLen-aDiff);
	}
	else if(aDiff < 0)
	{
		Reserve(mDataLen - aDiff);
		memmove(mData - aDiff,mData,mDataLen);
	}
	// else don't need to move memory
*/

	mLengthFieldSize = theLengthFieldSize;
	mLengthIncludesLengthFieldSize = lengthIncludesLengthFieldSize;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::CheckWrite(unsigned long theWriteLen)
{
	int aTotalLen = mDataLen + theWriteLen;
	if(aTotalLen>mCapacity)
	{
		int aSize = mCapacity;
		if(aSize<32)
			aSize = 32;

		while(aSize<aTotalLen)
			aSize<<=1;

		Reserve(aSize);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::ReserveChunk(unsigned long theNumBytesToReserve, unsigned long theMaxSize)
{
	int aTotalLen = theNumBytesToReserve;
	if(aTotalLen>mCapacity)
	{
		int aSize = mCapacity;
		if(aSize<32)
			aSize = 32;

		while(aSize<aTotalLen)
			aSize<<=1;

		if(theMaxSize>=theNumBytesToReserve && aSize>theMaxSize)
			aSize = theMaxSize;

		Reserve(aSize);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::Reserve(unsigned long theNumBytes)
{
	if(mCapacity<theNumBytes)
	{
		char *newBuffer = new char[theNumBytes+BEGIN_PAD];
		memcpy(newBuffer+BEGIN_PAD,mData,mDataLen);
		delete [] mBeginData;
		mBeginData = newBuffer;
		mData = newBuffer+BEGIN_PAD;
		mCapacity = theNumBytes;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::SetSize(unsigned long theNumBytes)
{
	Reserve(theNumBytes);
	mDataLen = theNumBytes;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::SkipBytes(unsigned long theLen)
{
	CheckWrite(theLen);
	mDataLen += theLen;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::AppendBytes(const void *theBytes, unsigned long theLen)
{
	CheckWrite(theLen);
	memcpy(mData + mDataLen, theBytes, theLen);
	mDataLen += theLen;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::AppendBit(bool theBit)
{
	if(mCurBit==0)
	{
		CheckWrite(1);
		mCurBit = 1;
		mDataLen++;

		*(mData + mDataLen - 1) = 0;
	}

	if(theBit)
		*(mData + mDataLen - 1) |= mCurBit;

	mCurBit<<=1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::AppendByte(char theByte)
{
	AppendBytes(&theByte,1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void WriteBuffer::AppendBool(bool theBool)
{
	unsigned char aVal = theBool?1:0;
	AppendBytes(&aVal,1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::AppendShort(short theShort)
{
	AppendBytes(&(theShort=ShortToLittleEndian(theShort)),2);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::AppendLong(long theLong)
{
	AppendBytes(&(theLong=LongToLittleEndian(theLong)),4);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::AppendInt64(__int64 theInt64)
{
	AppendBytes(&(theInt64=Int64ToLittleEndian(theInt64)),8);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::AppendLength(unsigned long theLen, unsigned char theLengthFieldSize)
{
	switch(theLengthFieldSize)
	{
		case 1: AppendByte(theLen); break;
		case 2: AppendShort(theLen); break;
		case 4: AppendLong(theLen); break;
		default: break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::AppendString(const std::string &theStr, unsigned char theLengthFieldSize)
{
	AppendLength(theStr.length(),theLengthFieldSize);
	AppendBytes(theStr.data(),theStr.length());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::AppendWString(const std::wstring &theWStr, unsigned char theLengthFieldSize)
{
	AppendLength(theWStr.length(),theLengthFieldSize);

	if(IsLittleEndian() && sizeof(wchar_t)==2)
		AppendBytes(theWStr.data(),theWStr.length()*2);
	else
	{
		for(int i=0; i<theWStr.length(); i++)
			AppendShort(theWStr[i]);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::AppendBuffer(const ByteBuffer* theBuffer, unsigned char theLengthFieldSize)
{
	unsigned long aLen = theBuffer?theBuffer->length():0;
	AppendLength(aLen,theLengthFieldSize);

	if(theBuffer!=NULL && aLen>0)
		AppendBytes(theBuffer->data(),aLen);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::SetPos(unsigned long thePos)
{
	Reserve(thePos);
	mDataLen = thePos;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::SetBytes(unsigned long thePos, const void *theBytes, unsigned long theLen)
{
	memcpy(mData+thePos,theBytes,theLen);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::SetByte(unsigned long thePos, char theByte)
{
	SetBytes(thePos,&theByte,1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::SetShort(unsigned long thePos, short theShort)
{
	SetBytes(thePos,&(theShort=ShortToLittleEndian(theShort)),2);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::SetLong(unsigned long thePos, long theLong)
{
	SetBytes(thePos,&(theLong=LongToLittleEndian(theLong)),4);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WriteBuffer::SetInt64(unsigned long thePos, __int64 theInt64)
{
	SetBytes(thePos,&(theInt64=Int64ToLittleEndian(theInt64)),8);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ByteBufferPtr WriteBuffer::ToByteBuffer(bool release)
{
	unsigned long aLen = mDataLen;
	char *data = mData;
	unsigned long datalen = mDataLen;

	switch(mLengthFieldSize)
	{
		case 1: 
			if(mLengthIncludesLengthFieldSize)
				aLen++;

			data = mData-1;
			datalen++;
			*data = aLen; 
			break;
		
		case 2:
			if(aLen<0xffff)
			{
				if(mLengthIncludesLengthFieldSize)
					aLen+=2;

				data = mData-2;
				datalen+=2;
				(*(unsigned short*)data) = ShortToLittleEndian(aLen); 
			}
			else // extended 2-byte
			{
				data = mData-6;
				datalen+=6;

				(*(unsigned short*)data) = ShortToLittleEndian(0xffff);
				if(mLengthIncludesLengthFieldSize)
					aLen+=4;

				(*(unsigned long*)(data+2)) = LongToLittleEndian(aLen);
			}
			break;
		
		case 4: 
			if(mLengthIncludesLengthFieldSize)
				aLen+=4;

			data = mData-4;
			datalen+=4;
			(*(unsigned long*)data) = LongToLittleEndian(aLen); 
			break;
		
		default: break;
	}

	
	if(release)
	{
		char *beginData = mBeginData;
		Release();
		return new OffsetByteBuffer(beginData,data-beginData,datalen);
	}
	else
		return new ByteBuffer(data,datalen,false);
}

