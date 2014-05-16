#ifndef __WON_GIFDECODER_H__
#define __WON_GIFDECODER_H__

#pragma warning(disable:4786)
#include "WONCommon/FileReader.h"

#include <string>
#include<list>
#include "Animation.h"
#include "RawImage.h"
#include "ImageDecoder.h"


namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GIFDecoder : public ImageDecoder
{
private:
	DisplayContext *mContext;

	std::string mSignature;
	
	unsigned short mScreenWidth;
	unsigned short mScreenHeight;
	unsigned char mNumBitsPerPixel;
	unsigned char mNumBitsOfColorResolution;
	bool mHaveGlobalColorMap;
	unsigned char mBackgroundColorIndex;
	PalettePtr mGlobalColorMap;
	
	AnimationFrameList mFrameList;
	unsigned short mNumLoops;

	int mCurImageWidth;
	int mCurImageHeight;
	int mCurImageTransparentColor;
	bool mCurImageIsInterlaced;
	PalettePtr mCurImagePalette;
	AnimationFramePtr mCurFrame;

	bool mNeedAccumulate;
	bool mIsRawDecode;

	NativeImagePtr mImage;
	AnimationPtr mAnimation;

private:
	struct LZWNode
	{
		LZWNode *mPrev;
		unsigned char mRootData;
		unsigned char mData;
		unsigned short mLen;

		void Init(LZWNode *thePrev, unsigned char theData)
		{
			mData = theData;
			mPrev = thePrev;
			mLen = thePrev->mLen+1;
			mRootData = thePrev->mRootData;
		}
	};

	LZWNode mStringTable[4096];

	bool ReadSignature();
	void ReadScreenDescriptor();
	PalettePtr ReadColorMap(int theNumColors);

	void InitStringTable();
	void ReadExtension();

	void ReadImageData();
	void ReadImage();
	void NextImage();

	ImagePtr BaseDecode();
	NativeImagePtr DoDecode(DisplayContext *theContext);
	RawImagePtr DoDecodeRaw();
	virtual bool DoGetAttrib(ImageAttributes &theAttrib);


public:
	GIFDecoder();
	static void Add(MultiImageDecoder *theDecoder);

	NativeImage* GetImage() { return mImage; }
	virtual AnimationPtr GetAnimation() { return mAnimation; }

	virtual ImageDecoder* Duplicate() { return new GIFDecoder; }

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


}; // namespace WONAPI

#endif