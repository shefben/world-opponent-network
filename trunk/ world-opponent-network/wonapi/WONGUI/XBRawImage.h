#ifndef __WON_XBRAWIMAGE_H__
#define __WON_XBRAWIMAGE_H__

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RawImage32 : public Image
{
protected:
	DWORD *mImageData;
	DWORD *mImagePtr;
	DWORD *mImageEnd;

	virtual ~RawImage32();

public:
	RawImage32(int theWidth, int theHeight, DWORD *theImageData = NULL);
	void SetPixel(int x, int y, DWORD theColor);
	DWORD GetPixel(int x, int y);

	DWORD* StartTraversal(int x = 0, int y = 0);
	bool HasMorePixels() { return mImagePtr!=mImageEnd; }
	void NextPixel() { mImagePtr++; }
	void SetPixel(DWORD theColor) { *mImagePtr = theColor; }
	DWORD GetPixel() { return *mImagePtr; }	

	NativeImagePtr GetNative(DisplayContext *theContext);

	virtual void Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height);
};

typedef WONAPI::SmartPtr<RawImage32> RawImage32Ptr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RawImage8 : public Image
{
protected:
	unsigned char *mImageData;
	unsigned char *mImagePtr;
	unsigned char *mImageEnd;
	PalettePtr mPalette;
	int mTransparentColor;

	virtual ~RawImage8();

public:
	RawImage8(int theWidth, int theHeight, Palette *thePalette, unsigned char *theImageData = NULL);
	void SetPixel(int x, int y, DWORD theColor);
	DWORD GetPixel(int x, int y);

	unsigned char* StartTraversal(int x = 0, int y = 0);
	bool HasMorePixels() { return mImagePtr!=mImageEnd; }
	void NextPixel() { mImagePtr++; }
	void SetPixel(unsigned char theColorIndex) { *mImagePtr = theColorIndex; }
	unsigned char GetPixel() { return *mImagePtr; }	

	void SetTransparentColor(int theColorIndex) { mTransparentColor = theColorIndex; }
	DWORD GetTransparentColor() { return mTransparentColor!=-1?mPalette->GetColor(mTransparentColor):0; }

	NativeImagePtr GetNative(DisplayContext *theContext);
	virtual void Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height);
};

typedef WONAPI::SmartPtr<RawImage8> RawImage8Ptr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RawImage1 : public Image
{
protected:
	unsigned char *mImageData;
	unsigned char *mImagePtr;
	unsigned char *mImageEnd;
	int mMask;
	int mCount;
	int mRowPad;
	int mRowSize;

	virtual ~RawImage1();

public:
	RawImage1(int theWidth, int theHeight, unsigned char *theImageData = NULL);
	void SetPixel(int x, int y, bool on);
	bool GetPixel(int x, int y);

	unsigned char* StartTraversal(int x = 0, int y = 0);
	bool HasMorePixels() { return mImagePtr!=mImageEnd; }
	void NextPixel() { mImagePtr++; }
/*	void NextPixel() 
	{	
		mCount++; 
		if(mCount==mWidth) 
		{
			mCount = 0; 
			mMask = 128;
			mImagePtr+=mRowPad;
		}
		else 
		{ 
			mMask>>=1; 
			if(mMask==0) 
			{ 
				mImagePtr++; 
				mMask = 128; 
			}	
		}		
	}*/

	void SetPixel(bool on) { if(on) *mImagePtr |= mMask; else *mImagePtr &= ~mMask; }
	bool GetPixel() { return (*mImagePtr)&mMask?true:false; }	

	NativeImagePtr GetNative(DisplayContext *theContext);
	//Pixmap GetMask(DisplayContext *theContext);
	virtual void Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height);
};

typedef WONAPI::SmartPtr<RawImage1> RawImage1Ptr;


}; // namespace WONAPI


#endif
