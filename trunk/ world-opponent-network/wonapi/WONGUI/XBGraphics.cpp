#include "XBGraphics.h"
#include <math.h>

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
XBGraphics::XBGraphics()
{
	mDevice = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
XBGraphics::~XBGraphics()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBGraphics::Detach()
{
	Graphics::Detach();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBGraphics::Attach(DisplayContext *theContext)
{
	Graphics::Attach(theContext);
	mDevice = theContext->mDevice;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBGraphics::SetPixelHook(int x, int y, DWORD theColor)
{
	theColor |= 0xFF000000;

	static LPDIRECT3DVERTEXBUFFER8 XMenu_VB = NULL;
	BACKGROUNDVERTEX *v;

    if(XMenu_VB == NULL)
        mDevice->CreateVertexBuffer(1*sizeof(BACKGROUNDVERTEX), D3DUSAGE_WRITEONLY, 
                                          0L, D3DPOOL_DEFAULT, &XMenu_VB);

    // Setup vertices for a background-covering quad
	XMenu_VB->Lock(0, 0, (BYTE **)&v, 0);
    v[0].p = D3DXVECTOR4(x, y, 1.0f, 1.0f);
	v[0].color = theColor;
	XMenu_VB->Unlock();

    // Set states
    mDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
    mDevice->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
    mDevice->SetStreamSource(0, XMenu_VB, sizeof(BACKGROUNDVERTEX));

    // Render the quad
    mDevice->DrawPrimitive(D3DPT_POINTLIST, 0, 1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBGraphics::SetColor(DWORD theColor)
{
	theColor = theColor | 0xff000000;
	if(mColor==theColor)
		return;

	Graphics::SetColor(theColor);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBGraphics::DrawLineHook(int x1, int y1, int x2, int y2)
{
	// The (leftmost,bottommost) pixel in the line doesn't get drawn.  
	// To fix end point problems, just draw both end points too.
	SetPixelHook(x1,y1,mColor);
	SetPixelHook(x2,y2,mColor);
	static LPDIRECT3DVERTEXBUFFER8 XMenu_VB = NULL;
	BACKGROUNDVERTEX *v;

    if(XMenu_VB == NULL)
        mDevice->CreateVertexBuffer(2*sizeof(BACKGROUNDVERTEX), D3DUSAGE_WRITEONLY, 
                                          0L, D3DPOOL_DEFAULT, &XMenu_VB);

    // Setup vertices for a background-covering quad
	XMenu_VB->Lock(0, 0, (BYTE **)&v, 0);
    v[0].p = D3DXVECTOR4(x1-0.01f, y1-0.01f, 1.0f, 1.0f);
	v[0].color = mColor;
    v[1].p = D3DXVECTOR4(x2+0.01f, y2+0.01f, 1.0f, 1.0f);
	v[1].color = mColor;
	XMenu_VB->Unlock();

    // Set states
    mDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
    mDevice->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
    mDevice->SetStreamSource(0, XMenu_VB, sizeof(BACKGROUNDVERTEX));

    // Render the quad
    //mDevice->DrawPrimitive(D3DPT_LINELIST, 0, 1);
	mDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, 1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBGraphics::DrawRectHook(int x, int y, int w, int h)
{
	w--; h--;

	static LPDIRECT3DVERTEXBUFFER8 XMenu_VB = NULL;
	BACKGROUNDVERTEX *v;

    if(XMenu_VB == NULL)
        mDevice->CreateVertexBuffer(5*sizeof(BACKGROUNDVERTEX), D3DUSAGE_WRITEONLY, 
                                          0L, D3DPOOL_DEFAULT, &XMenu_VB);

    // Setup vertices for a background-covering quad
	XMenu_VB->Lock(0, 0, (BYTE **)&v, 0);
    v[0].p = D3DXVECTOR4(x, y, 1.0f, 1.0f);
	v[0].color = mColor;
    v[1].p = D3DXVECTOR4(x+w, y, 1.0f, 1.0f);
	v[1].color = mColor;
    v[2].p = D3DXVECTOR4(x+w, y+h, 1.0f, 1.0f); 
	v[2].color = mColor;
    v[3].p = D3DXVECTOR4(x, y+h, 1.0f, 1.0f);
	v[3].color = mColor;
    v[4].p = D3DXVECTOR4(x, y, 1.0f, 1.0f);
	v[4].color = mColor;
	XMenu_VB->Unlock();

    // Set states
    mDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
    mDevice->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
    mDevice->SetStreamSource(0, XMenu_VB, sizeof(BACKGROUNDVERTEX));

    // Render the quad
    mDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, 4);
	SetPixelHook(x,y+h,mColor); // once again, the bottom/left pixel is not set
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBGraphics::FillRectHook(int x, int y, int w, int h)
{	
	static LPDIRECT3DVERTEXBUFFER8 XMenu_VB = NULL;
	BACKGROUNDVERTEX *v;

    if(XMenu_VB == NULL)
        mDevice->CreateVertexBuffer(4*sizeof(BACKGROUNDVERTEX), D3DUSAGE_WRITEONLY, 
                                          0L, D3DPOOL_DEFAULT, &XMenu_VB);

    // Setup vertices for a background-covering quad
	XMenu_VB->Lock(0, 0, (BYTE **)&v, 0);
    v[0].p = D3DXVECTOR4(x, y, 1.0f, 1.0f);
	v[0].color = mColor;
    v[1].p = D3DXVECTOR4(x+w, y, 1.0f, 1.0f);
	v[1].color = mColor;
    v[2].p = D3DXVECTOR4(x, y+h, 1.0f, 1.0f); 
	v[2].color = mColor;
    v[3].p = D3DXVECTOR4(x+w, y+h, 1.0f, 1.0f);
	v[3].color = mColor;
	XMenu_VB->Unlock();

    // Set states
    mDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
    mDevice->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
    mDevice->SetStreamSource(0, XMenu_VB, sizeof(BACKGROUNDVERTEX));

    // Render the quad
    mDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBGraphics::DrawDottedRectHook(int x, int y, int width, int height)
{
	Graphics::DrawDottedRect(x,y,width,height);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBGraphics::DrawEllipseHook(int x, int y, int width, int height)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBGraphics::FillEllipseHook(int x, int y, int width, int height)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBGraphics::DrawArcHook(int x, int y, int width, int height, int startAngle, int arcAngle)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBGraphics::ClearClipRectHook()
{
//	XSetClipMask(mDisplay, mGC, None);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void XBGraphics::SetClipRectHook(int x, int y, int width, int height)
{
//	XRectangle anXRect = {x, y, width, height};	
//	XSetClipRectangles(mDisplay, mGC, 0, 0, &anXRect, 1, Unsorted);
}

