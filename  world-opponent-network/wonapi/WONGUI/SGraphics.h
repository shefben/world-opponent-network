#ifndef __WON_SGRAPHICS_H__
#define __WON_SGRAPHICS_H__
#include "WONGUI/Graphics.h"
#include "Surface.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SGraphics : public Graphics
{
protected:
	SurfaceBasePtr mSurface;

protected:
	virtual DWORD GetPixelHook(int x, int y);
	virtual void SetPixelHook(int x, int y, DWORD theColor);
	virtual void SetColorHook();

	virtual void DrawLineHook(int x1, int y1, int x2, int y2);
	virtual void DrawRectHook(int x, int y, int width, int height);
	virtual void DrawDottedRectHook(int x, int y, int width, int height);
	virtual void FillRectHook(int x, int y, int width, int height);
	virtual void DrawPolygonHook(POINT thePoints[], int nPoints);
	virtual void FillPolygonHook(POINT thePoints[], int nPoints);

	virtual void DrawEllipseHook(int x, int y, int width, int height);
	virtual void FillEllipseHook(int x, int y, int width, int height);
	virtual void FillRectBlendHook(int x, int y, int width, int height, int theBlendAmount);
	virtual bool BlendSupported();

	virtual void DrawArcHook(int x, int y, int width, int height, int startAngle, int arcAngle);

	virtual void ClearClipRectHook();
	virtual void SetClipRectHook(int x, int y, int width, int height);

public:
	SGraphics();
	virtual ~SGraphics();

	virtual void Attach(DisplayContext *theContext);
	virtual void Detach();
};

}	// namespace WONAPI

#endif