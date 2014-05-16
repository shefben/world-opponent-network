#ifndef __WON_XBNATIVEIMAGE_H__
#define __WON_XBNATIVEIMAGE_H__

#include "Image.h"
#include "Graphics.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class NativeImage : public Image
{
protected:
	Graphics				*mGraphics;
	DisplayContextPtr		mDisplayContext;
	D3DTexture				*mTexture;
	D3DVertexBuffer			*mVB;

	struct NATIVEIMAGEVERTEX
	{
		D3DXVECTOR4 position; // The position
		D3DCOLOR    color;    // The color
		FLOAT       tu, tv;   // The texture coordinates
	};

	virtual ~NativeImage();
	void Init(DisplayContext *theContext, int theWidth, int theHeight, D3DTexture *theTexture);

public:
	NativeImage(DisplayContext *theContext, int theWidth, int theHeight, D3DFORMAT theFormat = D3DFMT_LIN_X8R8G8B8);
	NativeImage(DisplayContext *theContext, int theWidth, int theHegith, D3DTexture *theTexture);
	void Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height);
//	void SetTransparentColor(DWORD theColor, bool saveOld = false) { }
//	void SetTransparentMask(Pixmap theMask) { } 
	bool IsTransparent() { return false; }
	
	RawImage32Ptr NativeImage::GetRaw();
	ImagePtr NativeImage::Duplicate();

	Graphics& GetGraphics();
};

typedef WONAPI::SmartPtr<NativeImage> NativeImagePtr;

}; // namespace WONAPI

#endif
