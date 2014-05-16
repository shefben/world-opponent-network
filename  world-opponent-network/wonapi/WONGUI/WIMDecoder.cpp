#include "WIMDecoder.h"
#include "WONCommon/WriteBuffer.h"
#include "WONCommon/FileWriter.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WIMDecoder::DoGetAttrib(ImageAttributes &theAttrib)
{
	char aSig[4];
	mFileReader->ReadBytes(aSig,3);
	aSig[3] = '\0';
	if(strcmp(aSig,"WIM")!=0)
		return false;

	DWORD aType = mFileReader->ReadLong();
	if(aType!=32)
		return false;

	bool applyTransparency = mFileReader->ReadByte()?true:false;

	unsigned long aWidth = mFileReader->ReadLong();
	unsigned long aHeight = mFileReader->ReadLong();

	theAttrib.mWidth = aWidth;
	theAttrib.mHeight = aHeight;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr WIMDecoder::DoDecodeRaw()
{
	try
	{
		char aSig[4];
		mFileReader->ReadBytes(aSig,3);
		aSig[3] = '\0';
		if(strcmp(aSig,"WIM")!=0)
			return NULL;

		DWORD aType = mFileReader->ReadLong();
		if(aType!=32)
			return NULL;

		bool applyTransparency = mFileReader->ReadByte()?true:false;

		unsigned long aWidth = mFileReader->ReadLong();
		unsigned long aHeight = mFileReader->ReadLong();
		unsigned long aNumBytes = aWidth*aHeight*4;

		if(mFileReader->Available()<aNumBytes)
			return NULL;

		RawImage32Ptr anImage = CreateRawImage32(aWidth,aHeight);
		anImage->SetDoTransparency(applyTransparency);
		mFileReader->ReadBytes(anImage->GetImageData(),aNumBytes);
		return anImage.get();
	}
	catch(FileReaderException&)
	{
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WIMDecoder::EncodeToFile(RawImagePtr theImage, const char *theFilePath)
{
	FileWriter aWriter;
	if(!aWriter.Open(theFilePath,true))
		return false;

	ByteBufferPtr aBuf = EncodeToBuffer(theImage);
	if(aBuf.get()==NULL)
		return false;

	try
	{
		aWriter.WriteBytes(aBuf->data(),aBuf->length());
	}
	catch(FileWriterException&)
	{
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ByteBufferPtr WIMDecoder::EncodeToBuffer(RawImagePtr theImage)
{
	if(theImage->GetType()!=RawImageType_32)
		return NULL;

	RawImage32 *anImage = (RawImage32*)theImage.get();
	
	WriteBuffer aBuf;
	aBuf.AppendBytes("WIM",3); // file format identifier
	aBuf.AppendLong(32); // file subtype (32-bit raw)
	aBuf.AppendByte(anImage->GetDoTransparency()?1:0); // apply transparency bit?
	aBuf.AppendLong(anImage->GetWidth());
	aBuf.AppendLong(anImage->GetHeight());
	aBuf.AppendBytes(anImage->GetImageData(), 4*anImage->GetWidth()*anImage->GetHeight());
	return aBuf.ToByteBuffer();
}