#include "NativeImage.h"
#include "RawImage.h"
#include <algorithm>

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void NativeImage::Init(DisplayContext *theContext, int theWidth, int theHeight, D3DTexture *theTexture)
{
	mGraphics = NULL;

	mTexture = theTexture;
	mWidth = theWidth;
	mHeight = theHeight;

	//mTransparentMask = theTransparentMask;
	mDisplayContext = new DisplayContext(theContext->mDevice);
	//mDisplayContext->mDrawable = mPixmap;

    // Create the vertex buffer.
    theContext->mDevice->CreateVertexBuffer( 4*2*sizeof(NATIVEIMAGEVERTEX),
                                                  0, (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1),
                                                  D3DPOOL_DEFAULT, &mVB );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImage::NativeImage(DisplayContext *theContext, int theWidth, int theHeight, D3DFORMAT theFormat)
{
	D3DTexture *theTexture;
	theContext->mDevice->CreateTexture(theWidth, theHeight, 0, 0, theFormat, 0, &theTexture);

	Init(theContext, theWidth, theHeight, theTexture);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImage::NativeImage(DisplayContext *theContext, int theWidth, int theHeight, D3DTexture *theTexture)
{
	Init(theContext, theWidth, theHeight, theTexture);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImage::~NativeImage()
{
	delete mGraphics;

	if (mTexture)
		mTexture->Release();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void NativeImage::Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height)
{
	// Fill the vertex buffer
	if (mVB == NULL)
		return;

	NATIVEIMAGEVERTEX* pVertices;

	mVB->Lock(0,0, (BYTE**)&pVertices, 0);

	float fmWidth  = ((float)mWidth);
	float fmHeight = ((float)mHeight);

	float zorig = 0.0;
	float xsize = width;
	float ysize = height;
	float xorig = x;
	float yorig = y;

	float texLeft = left/fmWidth;
	float texTop = 1 - (top/fmHeight);
	float texRight = width/fmWidth + texLeft;
	float texBottom = (texTop + height/fmHeight);

    pVertices[0].position	= D3DXVECTOR4( xorig, yorig+ysize, zorig, 1.0 );
	pVertices[0].color		= 0xffffffff;
	pVertices[0].tu			= texLeft;	//0.0f;
	pVertices[0].tv			= texBottom;	//0.0f;

	pVertices[1].position	= D3DXVECTOR4( xorig+xsize, yorig, zorig, 1.0 );
	pVertices[1].color		= 0xffffffff;
	pVertices[1].tu			= texRight; //1.0f;
	pVertices[1].tv			= texTop; //1.0f;

	pVertices[2].position	= D3DXVECTOR4( xorig, yorig, zorig, 1.0 );
	pVertices[2].color		= 0xffffffff;
	pVertices[2].tu			= texLeft; //0.0f;
	pVertices[2].tv			= texTop; //1.0f;

	pVertices[3].position	= D3DXVECTOR4( xorig+xsize, yorig, zorig, 1.0 );
	pVertices[3].color		= 0xffffffff;
	pVertices[3].tu			= texRight; //1.0f;
	pVertices[3].tv			= texTop; //1.0f;

    pVertices[4].position	= D3DXVECTOR4( xorig+xsize, yorig+ysize, zorig, 1.0 );
	pVertices[4].color		= 0xffffffff;
	pVertices[4].tu			= texRight; //1.0f;
	pVertices[4].tv			= texBottom; //0.0f;

	pVertices[5].position	= D3DXVECTOR4( xorig, yorig+ysize, zorig, 1.0);
	pVertices[5].color		= 0xffffffff;
	pVertices[5].tu			= texLeft; //0.0f;
	pVertices[5].tv			= texBottom; //0.0f;


	mVB->Unlock();

    // Set states
	mDisplayContext->mDevice->SetTexture( 0, mTexture );
    mDisplayContext->mDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    mDisplayContext->mDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    mDisplayContext->mDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    mDisplayContext->mDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    mDisplayContext->mDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );


    // Render the vertex buffer contents
	mDisplayContext->mDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	mDisplayContext->mDevice->SetStreamSource( 0, mVB, sizeof(NATIVEIMAGEVERTEX) );
	mDisplayContext->mDevice->SetVertexShader( (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1) );
	mDisplayContext->mDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2);


	/*
	if(mTransparentMask!=0)
	{
		XGCValues xgcvalues;
		xgcvalues.function = GXand;
		xgcvalues.foreground = theContext->mWhitePixel;
		xgcvalues.background = theContext->mBlackPixel;

		XChangeGC(theContext->mDisplay, theContext->mGC, GCFunction | GCForeground | GCBackground, &xgcvalues);

		XCopyPlane(theContext->mDisplay, mTransparentMask, theContext->mDrawable, theContext->mGC, left, top, width, height, x, y, 1);

		xgcvalues.function = GXor;
		XChangeGC(theContext->mDisplay, theContext->mGC, GCFunction, &xgcvalues);
		XCopyArea(theContext->mDisplay, mPixmap, theContext->mDrawable, theContext->mGC, left, top, width, height, x, y);
		xgcvalues.function = GXcopy;
		XChangeGC(theContext->mDisplay, theContext->mGC, GCFunction, &xgcvalues);
	}
	else
	{
		XCopyArea(theContext->mDisplay, mPixmap, theContext->mDrawable, theContext->mGC, left, top, width, height, x, y);
	}
	*/
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Graphics& NativeImage::GetGraphics()
{
/*	if(mGraphics==NULL)
	{
		mGraphics = new XGraphics;
		XGCValues aValues;
		mDisplayContext->mGC = 	XCreateGC(mDisplayContext->mDisplay, mDisplayContext->mDrawable, 0, &aValues);
		mGraphics->Attach(mDisplayContext);
	}*/
	
	return *mGraphics;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImage32Ptr NativeImage::GetRaw()
{
	return NULL;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImagePtr NativeImage::Duplicate()
{
	return NULL;
}
