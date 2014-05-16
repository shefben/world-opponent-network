#ifndef __WON_XGRAPHICS_H__
#define __WON_XGRAPHICS_H__

#include "Graphics.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class XGraphics : public Graphics
{
private:
	Display *mDisplay;
	Drawable mDrawable;
	int mScreen;
	GC mGC;
	Colormap mColorMap;

public:
	XGraphics();
	~XGraphics();


	virtual void Attach(DisplayContext *theContext);
	virtual void Detach();
	
//	virtual DWORD GetPixelHook(int x, int y);
	virtual void SetPixelHook(int x, int y, DWORD theColor);
	virtual void SetColorHook();

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
	virtual void SetClipRectHook(int x, int y, int width, int height);
};

}; // namespace WONAPI

#endif 
