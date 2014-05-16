#ifndef __WON_CURSOR_H__
#define __WON_CURSOR_H__

#include "GUISystem.h"
#include "Component.h"


namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum StandardCursor
{
	StandardCursor_Arrow,
	StandardCursor_Hand,
	StandardCursor_IBeam,
	StandardCursor_EastWest,
	StandardCursor_NorthSouth,
	StandardCursor_NorthSouthEastWest,
	StandardCursor_Max
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Cursor  : public RefCount
{
protected:
	virtual ~Cursor();

public:
	Cursor();

	virtual Cursor* GetNative() { return NULL; }
	virtual Component* GetComponent(int &/*mouseX*/, int &/*mouseY*/) { return NULL; }

	static Cursor* GetStandardCursor(StandardCursor theType);
	static void SetStandardCursor(StandardCursor theType, Cursor *theCursor);

};
typedef WONAPI::SmartPtr<Cursor> CursorPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class CursorRef : public Cursor
{
protected:
	CursorPtr mCursor;

public:
	CursorRef(Cursor *theCursor) : mCursor(theCursor) { }

	virtual Cursor* GetNative() { return mCursor->GetNative(); }
	virtual Component* GetComponent(int &mouseX, int &mouseY) { return mCursor->GetComponent(mouseX,mouseY); }

	void SetCursor(Cursor *theCursor) { mCursor = theCursor; }
};
typedef WONAPI::SmartPtr<CursorRef> CursorRefPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ComponentCursor  : public Cursor
{
protected:
	ComponentPtr mComponent;
	int mMouseX, mMouseY;

public:
	ComponentCursor(Component *theComponent, int mouseX = 0, int mouseY = 0);
	virtual bool IsNative() { return false; }

	virtual Component* GetComponent(int &mouseX, int &mouseY);
};
typedef WONAPI::SmartPtr<ComponentCursor> ComponentCursorPtr;

}; // namespace WONAPI

#endif