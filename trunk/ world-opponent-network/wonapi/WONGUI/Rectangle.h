#ifndef __WON_RECTANGLE_H__
#define __WON_RECTANGLE_H__

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONRectangle
{
public:
	int x, y, width, height;

	WONRectangle();
	WONRectangle(int x, int y, int width, int height);

	int Left() const { return x; }
	int Right() const { return x+width; }

	int Top() const { return y; }
	int Bottom() const { return y+height; }

	int Width() const { return width; }
	int Height() const { return height; }

	void DoIntersect(const WONRectangle &theRect);
	void DoUnion(const WONRectangle &theRect);

	bool Contains(int px, int py) const { return px>=Left() && px<Right() && py>=Top() && py<Bottom(); }
	bool Intersects(const WONRectangle &r) const;
};

}; // namespace WONAPI

#endif