#include "GIFDecoder.h"
#include <memory>
using namespace std;

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GIFDecoder::GIFDecoder()
{
	mGlobalColorMap = NULL;
	mNumLoops = 0;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GIFDecoder::Add(MultiImageDecoder *theDecoder)
{
	GIFDecoder *aDecoder = new GIFDecoder;
	theDecoder->AddDecoder(aDecoder,"gif");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GIFDecoder::InitStringTable()
{
	for(int i=0; i<256; i++)
	{
		mStringTable[i].mData = i;
		mStringTable[i].mRootData = i;
		mStringTable[i].mLen = 1;
		mStringTable[i].mPrev = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static unsigned long GetBits(unsigned long &theVal, unsigned char theNumBits)
{
	unsigned long aVal = theVal & ((1<<theNumBits)-1);
	theVal>>=theNumBits;
	return aVal;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GIFDecoder::ReadSignature()
{
	mSignature.resize(6);
	mFileReader->ReadBytes((void*)mSignature.data(),mSignature.size());
	string aSub = mSignature.substr(3);

	return StringCompareNoCase(aSub,"GIF")!=0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GIFDecoder::ReadScreenDescriptor()
{
	mScreenWidth = mFileReader->ReadShort();
	mScreenHeight = mFileReader->ReadShort();

	unsigned long aVal = mFileReader->ReadByte();
	mNumBitsPerPixel = GetBits(aVal,3)+1;
	GetBits(aVal,1); // should be zero
	mNumBitsOfColorResolution = GetBits(aVal,3)+1;
	mHaveGlobalColorMap = GetBits(aVal,1)?true:false;

	mBackgroundColorIndex = mFileReader->ReadByte();
	aVal = mFileReader->ReadByte(); // should be zero
	
	if(mHaveGlobalColorMap)
		mGlobalColorMap = ReadColorMap(1<<mNumBitsPerPixel);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PalettePtr GIFDecoder::ReadColorMap(int theNumColors)
{
	Palette* aColorMap = new Palette(theNumColors);
	for(int i=0; i<theNumColors; i++)
	{
		unsigned char r = mFileReader->ReadByte();
		unsigned char g = mFileReader->ReadByte();
		unsigned char b = mFileReader->ReadByte();
		aColorMap->SetColor(i, (r<<16) | (g<<8) | b);
	}

	return aColorMap;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GIFDecoder::ReadExtension()
{
	unsigned char aFunctionCode = mFileReader->ReadByte();
	unsigned char aByteCount = mFileReader->ReadByte();
	switch(aFunctionCode)
	{
		case 0xf9: // graphics control extension
		{
			unsigned long aVal = mFileReader->ReadByte();
			bool transparentColor = GetBits(aVal,1)?true:false;
			bool userInput = GetBits(aVal,1)?true:false;
			unsigned char aDisposalMethod = GetBits(aVal,3); // 0 - Not Specified, 1 - Leave, 2 - Restore BG Color, 3 - Restore what was there before
			unsigned short aDelayTime = mFileReader->ReadShort();
			unsigned char aTransparentColorIndex = mFileReader->ReadByte();

			if(aDisposalMethod < AnimationFrame::DisposalMethod_Max)
				mCurFrame->mDisposalMethod = (AnimationFrame::DisposalMethod)aDisposalMethod;
			else
				mCurFrame->mDisposalMethod = AnimationFrame::DisposalMethod_None;

			if(aDelayTime==0)
				mNumLoops = 1;

			mCurFrame->mDelayTime = aDelayTime*10;
			if(transparentColor)
				mCurImageTransparentColor = aTransparentColorIndex;
			else
				mCurImageTransparentColor = -1;

			aByteCount = mFileReader->ReadByte();
		}
		break;

		case 0xff: // application extension
		{
			string anIdentifier;
			anIdentifier.resize(11);
			mFileReader->ReadBytes((void*)anIdentifier.data(),anIdentifier.size());
			aByteCount = mFileReader->ReadByte();
			if(anIdentifier=="NETSCAPE2.0")
			{
				unsigned char aByte = mFileReader->ReadByte();
				unsigned short aNumLoops = mFileReader->ReadShort();
				aByteCount = mFileReader->ReadByte();

				mNumLoops = aNumLoops;
			}
		}
		break;
	}
	
	while(aByteCount!=0)
	{
		mFileReader->SkipBytes(aByteCount);
		aByteCount = mFileReader->ReadByte();
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GIFDecoder::ReadImageData()
{
	unsigned char aCodeSize = mFileReader->ReadByte();
	unsigned char aByteCount = mFileReader->ReadByte();

	unsigned char aCompressionSize = aCodeSize+1;
	int aCompressionMask = (1<<aCompressionSize)-1;

	unsigned short aClearCode = 1<<aCodeSize;
	unsigned short anEndCode = aClearCode+1;
	unsigned short nextCode = aClearCode+2;
	
	RawImage8Ptr anImage = CreateRawImage8(mCurImageWidth,mCurImageHeight,mCurImagePalette);
	int anImageSize = mCurImageWidth*mCurImageHeight;
	int aRowPad = anImage->GetBytesPerRow() - mCurImageWidth;

	unsigned long anImagePos = 0;

	unsigned short code = 0;
	int oldCode = -1;
	LZWNode *oldString = NULL, *newString = NULL;
	int aBitCount = 0;
	int aData = 0;
	int aBytesLeft = aByteCount;

	int aDebugCount = 0;
	while(aByteCount!=0)
	{
		if(aCompressionSize > aBitCount)
		{
			if(aBytesLeft==0)
			{
				aByteCount = aBytesLeft = mFileReader->ReadByte();
				continue;
			}

			aData |= (mFileReader->ReadByte()<<aBitCount);
			aBitCount+=8;
			aBytesLeft--;
			continue;
		}

		code = aData&aCompressionMask;
		aData>>=aCompressionSize;
		aBitCount-=aCompressionSize;

		if(code>nextCode)
			break;

		if(code==aClearCode)
		{
			oldCode = -1;
			aCompressionSize = aCodeSize+1;
			aCompressionMask = (1<<aCompressionSize)-1;
			nextCode = aClearCode+2;
		}
		else if(code==anEndCode)
			break; 
		else 
		{
			if(code<nextCode)
				newString = mStringTable + code;
			else
			{
				newString = mStringTable + nextCode;
				newString->Init(oldString, oldString->mRootData);
			}

			int aLen = newString->mLen;
			LZWNode *aNode = newString;

			anImagePos += newString->mLen-1;
			if(anImagePos >= anImageSize) // keep from crashing with corrupt image
				break;

			int aRow = anImagePos/mCurImageWidth;
			int aCol = anImagePos%mCurImageWidth;
			unsigned char *anImageData = anImage->StartRowTraversal(aRow)+aCol;
			while(aLen--)
			{
				*anImageData = aNode->mData;
				aNode = aNode->mPrev;
				aCol--;
				anImageData--;
				if(aCol<0)
				{
					anImageData-=aRowPad;
					aCol = mCurImageWidth-1;
				}
			}
			anImagePos++;


			if(oldCode!=-1)
			{
				mStringTable[nextCode++].Init(oldString,newString->mRootData);
				if(nextCode > aCompressionMask && nextCode<4096)
				{
					aCompressionSize++;
					aCompressionMask = (1<<aCompressionSize)-1;
				}
			}

			oldCode = code;
			oldString = newString;
		}
	}
	
	unsigned long aVal = mFileReader->ReadByte(); // should be zero

	// Handle interlaced image
	if(mCurImageIsInterlaced)
	{
		RawImage8Ptr anInterlacedImage = anImage;
		anImage = CreateRawImage8(mCurImageWidth,mCurImageHeight,mCurImagePalette);
		
		static const int dy[] = {8, 8, 4, 2};
		static const int yoff[] = {0, 4, 2, 1};
		int aWidth = anImage->GetBytesPerRow();

		int y_interlaced = 0;
		for(int i = 0; i<4; i++)
		{
			int y = yoff[i];
			while(y < mCurImageHeight)
			{
				memcpy(anImage->GetImageData() + y*aWidth, anInterlacedImage->GetImageData() + y_interlaced*aWidth, aWidth);
				y+=dy[i];
				y_interlaced++;
			}
		}
	}
		
	anImage->SetTransparentColor(mCurImageTransparentColor);
	if(mIsRawDecode)
		mCurFrame->mImage = anImage;
	else
		mCurFrame->mImage = anImage->GetNative(mContext);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GIFDecoder::ReadImage()
{
	mCurFrame->mLeft = mFileReader->ReadShort();
	mCurFrame->mTop = mFileReader->ReadShort();
	mCurImageWidth = mFileReader->ReadShort();
	mCurImageHeight = mFileReader->ReadShort();
	if(mCurImageWidth*mCurImageHeight>3000*3000)
		throw FileReaderException("Image too big.");
	else if(mCurImageWidth<=0 || mCurImageHeight<=0)
		throw FileReaderException("Image too small.");

	unsigned long aVal = mFileReader->ReadByte();
	int aNumBitsPerPixel =  GetBits(aVal,3)+1;
	GetBits(aVal,3); // should be 0
	mCurImageIsInterlaced = GetBits(aVal,1)?true:false;

	bool haveLocalColorMap = GetBits(aVal,1)?true:false;
	if(haveLocalColorMap)
		mCurImagePalette = ReadColorMap(1<<aNumBitsPerPixel);
	else
		mCurImagePalette = mGlobalColorMap;

	ReadImageData();

	Image *anImage = (Image*)mCurFrame->mImage;
	if(anImage->GetWidth()!=mScreenWidth || anImage->GetHeight()!=mScreenHeight)
		mNeedAccumulate = true;

	mFrameList.push_back(mCurFrame);
	NextImage();
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GIFDecoder::NextImage()
{
	mCurImageWidth = mCurImageHeight = 0;
	mCurImageTransparentColor = -1;
	mCurImageIsInterlaced = false;
	mCurImagePalette = NULL;
	mCurFrame = new AnimationFrame;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GIFDecoder::DoGetAttrib(ImageAttributes &theAttrib)
{
	if(!ReadSignature())
		return false;

	ReadScreenDescriptor();
	theAttrib.mWidth = mScreenWidth;
	theAttrib.mHeight = mScreenHeight;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImagePtr GIFDecoder::BaseDecode()
{
	InitStringTable();

	bool success = false;
	mImage = NULL;
	mAnimation = NULL;

	mNeedAccumulate = false;

	mFrameList.clear();

	try
	{
		ReadSignature();
		ReadScreenDescriptor();

		NextImage();
		bool done = false;
		while(!success)
		{
			char aChar = mFileReader->ReadByte();
			switch(aChar)
			{
				case '!': 
					ReadExtension(); 
					break;

				case ',': 
					ReadImage(); 
					break;

				case ';': 
					success = true; 
					break;
			}
		} 
	}
	catch(WONAPI::FileReaderException&)
	{
	}

	if(!mFrameList.empty())
	{
		ImagePtr anImage = mFrameList.front()->mImage.get();

		mAnimation = new Animation(mFrameList, mScreenWidth, mScreenHeight);
		mAnimation->SetNumLoops(mNumLoops);
		mAnimation->SetNeedAccumulate(mNeedAccumulate);
		if(mIsRawDecode)
		{
			RawImage8 *aRawImage = (RawImage8*)anImage.get();
			mAnimation->SetCopyFirstImageToAccumulate(aRawImage->GetTransparentColor()>=0);
		}
		else
		{
			NativeImage *aNativeImage = (NativeImage*)anImage.get();
			mAnimation->SetCopyFirstImageToAccumulate(aNativeImage->IsTransparent());
			mAnimation->RealizeNative(mContext);
		}				

		return anImage;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr GIFDecoder::DoDecode(DisplayContext *theContext)
{
	mIsRawDecode = false;
	mContext = theContext;
	mImage = (NativeImage*)BaseDecode().get();
	return mImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr GIFDecoder::DoDecodeRaw()
{
	mIsRawDecode = true;
	return (RawImage*)BaseDecode().get();
}
