#include "Rectangle.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONRectangle::WONRectangle()
{
	x = y = width = height = 0;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONRectangle::WONRectangle(int x, int y, int width, int height)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONRectangle::DoIntersect(const WONRectangle &theRect)
{
	int right1 = x + width;
	int bottom1 = y + height;

	int right2 = theRect.x + theRect.width;
	int bottom2 = theRect.y + theRect.height;

	if(theRect.x > x)
		x = theRect.x;

	if(theRect.y > y)
		y = theRect.y;

	if(right2 < right1)
		right1 = right2;

	if(bottom2 < bottom1)
		bottom1 = bottom2;

	width = right1 - x;
	height = bottom1 - y;

	if(width<0)
		width = 0;

	if(height<0)
		height = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONRectangle::DoUnion(const WONRectangle &theRect)
{
	if(width==0 || height==0)
	{
		x = theRect.x;
		y = theRect.y;
		width = theRect.width;
		height = theRect.height;
		return;
	}

	int right1 = x + width;
	int bottom1 = y + height;

	int right2 = theRect.x + theRect.width;
	int bottom2 = theRect.y + theRect.height;
	
	if(theRect.x < x)
		x = theRect.x;

	if(theRect.y < y)
		y = theRect.y;

	if(right2 > right1)
		right1 = right2;

	if(bottom2 > bottom1)
		bottom1 = bottom2;

	width = right1 - x;
	height = bottom1 - y;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONRectangle::Intersects(const WONRectangle &r) const
{
/*	return	!(
			(Right() < r.Left()) || 
			(Left() >= r.Right()) ||
			(Bottom() < r.Top()) ||
			(Top() >= r.Bottom())
			)*/

	return	(Right() >= r.Left()) && 
			(Left() < r.Right()) &&
			(Bottom() >= r.Top()) &&
			(Top() < r.Bottom());
}

