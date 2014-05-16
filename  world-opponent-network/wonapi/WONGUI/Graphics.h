#ifndef __WON_GRAPHICS_H__
#define __WON_GRAPHICS_H__

#include "GUISystem.h"

#include <list>
#include "Font.h"
#include "Image.h"
#include "Rectangle.h"
#include "DisplayContext.h"
#include "ColorScheme.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Graphics
{
protected:
	DisplayContextPtr mDisplayContext;
	int mTransX, mTransY;

	void DoTranslate(int &x, int &y);
	FontPtr mFont;
	DWORD mColor;

	typedef std::list<WONRectangle> ClipStack;
	ClipStack mClipStack;
	WONRectangle mClipRect;

	ColorScheme* mColorScheme;

public:
	virtual void ClearClipRectHook() { }
	virtual void SetClipRectHook(int /*x*/, int /*y*/, int /*width*/, int /*height*/) { }

	virtual DWORD GetPixelHook(int /*x*/, int /*y*/) { return 0; }
	virtual void SetPixelHook(int /*x*/, int /*y*/, DWORD /*theColor*/) { }
	virtual void SetColorHook() { }

	virtual void DrawLineHook(int x1, int y1, int x2, int y2);
	virtual void DrawRectHook(int x, int y, int width, int height);
	virtual void DrawDottedRectHook(int x, int y, int width, int height);
	virtual void FillRectHook(int x, int y, int width, int height);
	virtual void DrawPolygonHook(POINT thePoints[], int nPoints);
	virtual void FillPolygonHook(POINT thePoints[], int nPoints);

	virtual void DrawEllipseHook(int x, int y, int width, int height);
	virtual void FillEllipseHook(int x, int y, int width, int height);

	virtual void DrawArcHook(int x, int y, int width, int height, int startAngle, int arcAngle);
	virtual void FillRectBlendHook(int x, int y, int width, int height, int theBlendAmount);

public:
	Graphics();
	virtual ~Graphics();

	virtual void Attach(DisplayContext *theContext);
	virtual void Detach();
	void SetColor(DWORD theColor);
	DWORD GetColor() { return mColor; }

	void SetColorScheme(ColorScheme* theColorScheme) { mColorScheme = theColorScheme; }
	ColorScheme* GetColorScheme() { return mColorScheme; }
	DWORD GetColorSchemeColor(StandardColor theColorRef);
	void ApplyColorSchemeColor(StandardColor theColorRef);

	void SetFont(Font *theFont);
	Font* GetFont() { return mFont; }

	void Translate(int dx, int dy);
	int GetTransX() { return mTransX; }
	int GetTransY() { return mTransY; }
	const WONRectangle& GetClipRect() { return mClipRect; }
	void GetClipRectWithTrans(WONRectangle &theRect);


	DisplayContext* GetDisplayContext() { return mDisplayContext; }

	void DrawImage(Image *theImage, int x, int y);
	void DrawImage(Image *theImage, int x, int y, int left, int top, int width, int height);

	void DrawString(const GUIString &theStr, int x, int y);
	void DrawString(const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y);
	void DrawString(const char *theStr, int theLen, int x, int y);

	void PerformClip();
	void PushClipRect(int x, int y, int width, int height);
	void PopClipRect();

	DWORD GetPixel(int x, int y);
	void SetPixel(int x, int y, DWORD theColor);

	void DrawLine(int x1, int y1, int x2, int y2);
	void DrawRect(int x, int y, int width, int height);
	void DrawDottedRect(int x, int y, int width, int height);
	void FillRect(int x, int y, int width, int height);
	void DrawPolygon(POINT thePoints[], int nPoints);
	void FillPolygon(POINT thePoints[], int nPoints);

	void FillRectBlend(int x, int y, int width, int height, int theBlendAmount);
	virtual bool BlendSupported() { return false; }

	void DrawEllipse(int x, int y, int width, int height);
	void FillEllipse(int x, int y, int width, int height);

	void DrawArc(int x, int y, int width, int height, int startAngle, int arcAngle);
};

}; // namespace WONAPI

#endif __GRAPHICS_H__
