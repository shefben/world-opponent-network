#ifndef __WON_IMAGE_H__
#define __WON_IMAGE_H__

#include "WONCommon/SmartPtr.h"

namespace WONAPI
{

class DisplayContext;
class Graphics;

WON_PTR_FORWARD(NativeImage);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Image;
typedef WONAPI::SmartPtr<Image> ImagePtr;

class Image : public WONAPI::RefCount
{
protected:
	virtual ~Image();

	unsigned short mWidth;
	unsigned short mHeight;

public:
	void DrawEntire(DisplayContext *theContext, int x, int y);
	virtual void Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height);

	unsigned short GetWidth() { return mWidth; }
	unsigned short GetHeight() { return mHeight; }

	virtual void SetSize(int /*theWidth*/, int /*theHeight*/) { }

	virtual ImagePtr Duplicate() { return NULL; }
	virtual NativeImagePtr GetNative(DisplayContext *theContext);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ImagePiece : public Image
{
private:
	ImagePtr mImage;
	int mLeft, mTop;

protected:
	virtual ~ImagePiece();

public:
	ImagePiece(Image *theImage, int left, int top, int width, int height);
	virtual void Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height);
};
typedef WONAPI::SmartPtr<ImagePiece> ImagePiecePtr;
 
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ProgramImage : public Image
{
protected:
	typedef void(*PaintFunc)(Graphics &g, int width, int height);
	PaintFunc mPaintFunc;
	virtual void Paint(Graphics &g);

public:
	ProgramImage(PaintFunc thePaintFunc);
	ProgramImage(int theWidth = 0, int theHeight = 0, PaintFunc thePaintFunc = NULL);
	
	virtual ImagePtr Duplicate() { return new ProgramImage(mWidth,mHeight,mPaintFunc); }
	virtual void Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height);

	virtual void SetSize(int theWidth, int theHeight) { mWidth = (unsigned short)theWidth; mHeight = (unsigned short)theHeight; }

};
typedef WONAPI::SmartPtr<ProgramImage> ProgramImagePtr;


}; // namespace WONAPI

#endif