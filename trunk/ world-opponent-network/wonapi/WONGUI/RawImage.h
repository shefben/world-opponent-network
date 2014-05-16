#ifndef __WON_RAWIMAGE_H__
#define __WON_RAWIMAGE_H__

#include "Image.h"

namespace WONAPI
{
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class NativeImage;
typedef SmartPtr<NativeImage> NativeImagePtr;

class NativeMask;
typedef SmartPtr<NativeMask> NativeMaskPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Palette;
typedef WONAPI::SmartPtr<Palette> PalettePtr;

class Palette : public RefCount
{
private:
	DWORD *mColors;
	int mNumColors;

protected:
	virtual ~Palette() { delete mColors; }

public:
	Palette(int theNumColors) : mNumColors(theNumColors) { mColors = new DWORD[theNumColors]; }
	void SetColor(int thePos, unsigned long theVal) { mColors[thePos] = theVal; }
	unsigned long GetColor(int thePos) { return mColors[thePos]; }

	int GetNumColors() { return mNumColors; }
	PalettePtr Duplicate() 
	{ 
		Palette *aPalette = new Palette(mNumColors); 
		memcpy(aPalette->mColors, mColors, mNumColors*sizeof(DWORD)); 
		return aPalette; 
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum RawImageType
{
	RawImageType_1,
	RawImageType_8,
	RawImageType_32
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WON_PTR_FORWARD(RawImage32);

class RawImage : public Image
{
protected:
	RawImageType mType;

	RawImage(RawImageType theType) : mType(theType) { }

public:
	RawImageType GetType() { return mType; }

	virtual NativeImagePtr GetNative(DisplayContext *theContext) = 0;
	virtual RawImage32Ptr GetRawImage32() = 0;
	void Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height);

	virtual bool HasTransparency() { return false; }
};
typedef SmartPtr<RawImage> RawImagePtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RawImage32 : public RawImage
{
public:
	typedef DWORD PixelType;

protected:
	DWORD *mImageData;
	DWORD *mImageEnd;
	DWORD *mRowPtr;
	DWORD *mRowEnd;
	bool mDoTransparency;

	virtual ~RawImage32();

	virtual NativeImagePtr PlatformGetNative(DisplayContext *theContext) = 0;
	virtual RawImage32* PlatformDuplicate() = 0;

public:
	RawImage32(int theWidth, int theHeight);

	// Image Params
	void CopyInfo(RawImage32 *theImage) { mDoTransparency = theImage->mDoTransparency; }
	void SetDoTransparency(bool doTransparency) { mDoTransparency = doTransparency; }
	bool GetDoTransparency() { return mDoTransparency; }
	virtual bool HasTransparency() { return mDoTransparency; }

	// Pixel operations
	void SetPixel(int x, int y, DWORD theColor);
	DWORD GetPixel(int x, int y);
	DWORD* GetImageData() { return mImageData; }

	// Row operations
	DWORD* StartRowTraversal(int theRow);
	bool HasMoreRowPixels() { return mRowPtr<mRowEnd; }
	bool HasMoreRows() { return mRowEnd<=mImageEnd; }
	void NextRowPixel() { mRowPtr++; }
	void NextRowPixel(int theAmount) { mRowPtr += theAmount; }
	void NextRow() { mRowPtr = mRowEnd; mRowEnd+=mWidth; }
	void NextRowPixelWrap() 
	{ 
		NextRowPixel(); 
		if(!HasMoreRowPixels())
			NextRow();
	}

	void SetRowPixel(DWORD theColor) { *mRowPtr = theColor; }
	DWORD GetRowPixel() { return *mRowPtr; }	
	
	// Converting to NativeImages 
	NativeImagePtr GetNative(DisplayContext *theContext);
	RawImage32Ptr GetRawImage32() { return this; }

	virtual ImagePtr Duplicate();
};

typedef WONAPI::SmartPtr<RawImage32> RawImage32Ptr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RawImage8 : public RawImage
{
public:
	typedef unsigned char PixelType;

protected:
	unsigned char *mImageData;
	unsigned char *mImageEnd;
	unsigned char *mRowPtr;
	unsigned char *mRowEnd;
	int mRowPad, mBytesPerRow;
	PalettePtr mPalette;
	int mTransparentColor;

	virtual ~RawImage8();
	virtual NativeImagePtr PlatformGetNative(DisplayContext *theContext) = 0;
	virtual RawImage8* PlatformDuplicate() = 0;
	
	void FinishConstructor();

public:
	RawImage8(int theWidth, int theHeight, Palette *thePalette = NULL);

	// Platform dependent row padding
	int GetBytesPerRow() { return mBytesPerRow; }

	// Copies palette and transparency info
	void CopyInfo(RawImage8 *theImage) { mPalette = theImage->mPalette; mTransparentColor = theImage->mTransparentColor; }

	// Palette/Transparency Access
	void SetTransparentColor(int theColorIndex) { mTransparentColor = theColorIndex; }
	int GetTransparentColor() { return mTransparentColor; }
	virtual bool HasTransparency() { return mTransparentColor>=0; }

	Palette* GetPalette() { return mPalette; }
	void SetPalette(Palette *thePalette) { mPalette = thePalette; }

	// Pixel operations
	void SetPixel(int x, int y, unsigned char theColor);
	unsigned char GetPixel(int x, int y);
	unsigned char* GetImageData() { return mImageData; }

	// Row operations
	unsigned char* StartRowTraversal(int theRow);
	bool HasMoreRowPixels() { return mRowPtr<mRowEnd; }
	bool HasMoreRows() { return mRowEnd<=mImageEnd; }
	void NextRowPixel() { mRowPtr++; }
	void NextRowPixel(int theAmount) { mRowPtr += theAmount; }
	void NextRow() { mRowPtr = mRowEnd+mRowPad; mRowEnd+=mBytesPerRow; }
	void NextRowPixelWrap() 
	{ 
		NextRowPixel(); 
		if(!HasMoreRowPixels())
			NextRow();
	}

	void SetRowPixel(unsigned char theColorIndex) { *mRowPtr = theColorIndex; }
	unsigned char GetRowPixel() { return *mRowPtr; }	

	// Converting to NativeImages 
	NativeImagePtr GetNative(DisplayContext *theContext);
	RawImage32Ptr GetRawImage32();
	virtual ImagePtr Duplicate();
};

typedef WONAPI::SmartPtr<RawImage8> RawImage8Ptr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RawImage1 : public RawImage
{
public:
	typedef bool PixelType;

protected:
	unsigned char *mImageData;
	unsigned char *mImageEnd;
	unsigned char *mRowPtr;
	unsigned char *mRowEnd;
	int mRowMask;
	int mRowPad;
	int mEndRowMask;
	int mBytesPerRow;
	int mBytesPerRowMinusPad;
	int mCompleteBytesPerRow;
	int mWidthMod8;

	virtual ~RawImage1();

	void FinishConstructor();

public:
	RawImage1(int theWidth, int theHeight);

	// Platform dependent row padding
	int GetBytesPerRow() { return mBytesPerRow; }

	// Image params
	void CopyInfo(RawImage1 * /*theImage*/) { }

	// Pixel Operations
	void SetPixel(int x, int y, bool on);
	bool GetPixel(int x, int y);
	unsigned char* GetImageData() { return mImageData; }

	// Row Operations
	unsigned char* StartRowTraversal(int theRow);
	bool HasMoreRowPixels() { return mRowPtr<mRowEnd || mRowMask>=mEndRowMask; }
	bool HasMoreRows() { return mRowEnd<=mImageEnd; }
	void NextRowPixel() 
	{	
		mRowMask>>=1; 
		if(mRowMask==0) 
		{ 
			mRowPtr++; 
			mRowMask = 128; 
		}	
	}

	void NextRowPixel(int theAmount) 
	{ 
		for(int i=0; i<theAmount; i++)
			NextRowPixel();
	}
	void NextRow() { mRowPtr = mRowEnd+mRowPad; mRowEnd+=mBytesPerRow; mRowMask = 128; }
	void NextRowPixelWrap() 
	{ 
		NextRowPixel(); 
		if(!HasMoreRowPixels())
			NextRow();
	}

	void SetRowPixel(bool on) { if(on) *mRowPtr |= mRowMask; else *mRowPtr &= ~mRowMask; }
	bool GetRowPixel() { return (*mRowPtr)&mRowMask?true:false; }	


	// Converting to NativeImage or NativeMask
	RawImage32Ptr GetRawImage32() { return NULL; }
	virtual NativeImagePtr GetNative(DisplayContext *theContext) = 0;
	virtual NativeMaskPtr GetMask(DisplayContext *theContext) = 0;
};

typedef WONAPI::SmartPtr<RawImage1> RawImage1Ptr;


}; // namespace WONAPI

#endif