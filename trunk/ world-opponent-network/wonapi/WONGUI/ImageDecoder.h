#ifndef __WON_IMAGEDECODER_H__
#define __WON_IMAGEDECODER_H__

#include "NativeImage.h"
#include "RawImage.h"
#include "Animation.h"
#include "WONCommon/StringUtil.h"
#include "WONCommon/FileReader.h"
#include <set>
#include <map>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ImageAttributes
{
public:
	int mWidth, mHeight;

	ImageAttributes();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ImageDecoder
{
protected:
	FileReader *mFileReader;
	virtual NativeImagePtr DoDecode(DisplayContext *theContext);
	virtual RawImagePtr DoDecodeRaw() = 0;
	virtual bool DoGetAttrib(ImageAttributes &theAttrib) = 0;

	RawImage8Ptr CreateRawImage8(int theWidth, int theHeight, Palette *thePalette);
	RawImage32Ptr CreateRawImage32(int theWidth, int theHeight);

public:
	ImageDecoder();
	virtual ~ImageDecoder();

	NativeImagePtr Decode(DisplayContext *theContext, const char *theFilePath);
	NativeImagePtr Decode(DisplayContext *theContext, const void *theData, unsigned long theDataLen);
	NativeImagePtr Decode(DisplayContext *theContext, FileReader &theReader);

	RawImagePtr DecodeRaw(const char *theFilePath);
	RawImagePtr DecodeRaw(const void *theData, unsigned long theDataLen);
	RawImagePtr DecodeRaw(FileReader &theReader);

	bool GetImageAttributes(const char *theFilePath, ImageAttributes &theAttrib);
	bool GetImageAttributes(const void *theData, unsigned long theDataLen, ImageAttributes &theAttrib);
	bool GetImageAttributes(FileReader &theReader, ImageAttributes &theAttrib);

	virtual AnimationPtr GetAnimation() { return NULL; }
	virtual ImageDecoder* Duplicate() = NULL;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MultiImageDecoder : public ImageDecoder
{
protected:
	typedef std::set<ImageDecoder*> DecoderSet;
	typedef std::map<std::string,ImageDecoder*,StringLessNoCase> DecoderMap;

	DecoderSet mDecoderSet;
	DecoderMap mDecoderMap;

	AnimationPtr mAnimation;
	std::string mCurExtension;

	virtual NativeImagePtr DoDecode(DisplayContext *theContext);
	virtual RawImagePtr DoDecodeRaw();
	virtual bool DoGetAttrib(ImageAttributes &theAttrib);

public:
	MultiImageDecoder();
	virtual ~MultiImageDecoder();

	void AddDecoder(ImageDecoder *theDecoder, const char *theExtension); // takes ownership of theDecoder
	void SetCurExtension(const char *theExtension) { mCurExtension = theExtension; }
	void SetFileName(const char *theFileName); // figures out cur extension

	NativeImagePtr Decode(DisplayContext *theContext, const char *theFilePath);
	RawImagePtr DecodeRaw(const char *theFilePath);
	bool GetImageAttributes(const char *theFilePath, ImageAttributes &theAttrib);
	int GetNumDecoders() { return mDecoderSet.size(); }

	virtual AnimationPtr GetAnimation(bool evenIfOnlyOneFrame = true);

	virtual ImageDecoder* Duplicate();
};

}; // namespace WONAPI

#endif