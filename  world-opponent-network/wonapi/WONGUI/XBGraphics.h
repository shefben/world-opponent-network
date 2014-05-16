#ifndef __WON_XGRAPHICS_H__
#define __WON_XGRAPHICS_H__

#include "Graphics.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class XBGraphics : public Graphics
{
private:
	IDirect3DDevice8* mDevice;
	struct BACKGROUNDVERTEX 
	{ 
		D3DXVECTOR4 p; 
		D3DCOLOR color; 
	};;

public:
	XBGraphics();
	virtual ~XBGraphics();


	virtual void Attach(DisplayContext *theContext);
	virtual void Detach();
	
//	virtual DWORD GetPixelHook(int x, int y);
	virtual void SetPixelHook(int x, int y, DWORD theColor);
	virtual void SetColor(DWORD theColor);

	virtual void DrawLineHook(int x1, int y1, int x2, int y2);
	virtual void DrawRectHook(int x, int y, int width, int height);
	virtual void DrawDottedRectHook(int x, int y, int width, int height);
	virtual void FillRectHook(int x, int y, int width, int height);
//	virtual void DrawPolygonHook(POINT thePoints[], int nPoints);
//	virtual void FillPolygonHook(POINT thePoints[], int nPoints);

	virtual void DrawEllipseHook(int x, int y, int width, int height);
	virtual void FillEllipseHook(int x, int y, int width, int height);

	virtual void DrawArcHook(int x, int y, int width, int height, int startAngle, int arcAngle);

	virtual void ClearClipRectHook();
	virtual void SetClipRectHook(int x, int y, int width, int height);};

}; // namespace WONAPI

#endif 
