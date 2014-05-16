//----------------------------------------------------------------------------------
// ProgressBarComponent.h
//----------------------------------------------------------------------------------
#ifndef __ProgressBarComponent_H__
#define __ProgressBarComponent_H__

#include "WONGUI/Component.h"
#include "WONGUI/WONGUIConfig/ComponentConfig.h"

namespace WONAPI
{

//----------------------------------------------------------------------------------
// ProgressBarComponent
//----------------------------------------------------------------------------------
class ProgressBarComponent: public Component
{
protected:
	// Attributes.
	int      mStartPos;
	int      mEndPos;
	int      mCurPos;
	Background	mBackground;
	Background	mForeground;

	// Restricted Operations.
	inline void CheckBounds(void)           
	{
		if (mCurPos > mEndPos) 
			mCurPos = mEndPos;
		if (mCurPos < mStartPos) 
			mCurPos = mStartPos;
	}
	void Init(void);

public:
	// Operations.
	inline int  GetPosition(void) const               { return mCurPos; }
	inline void SetPosition(int Pos)                  { mCurPos = Pos; CheckBounds(); Invalidate(); }
	inline void IncrementPosition(int Pos = 1)        { mCurPos += Pos; CheckBounds(); Invalidate(); }
	inline void SetRange(int Start, int End)          { mStartPos = Start; mEndPos = End; CheckBounds(); Invalidate(); }
	inline void GetRange(int& Start, int& End) const  { Start = mStartPos; End = mEndPos; }
	inline int  GetStart(void) const                  { return mStartPos; }
	inline int  GetEnd(void) const                    { return mEndPos; }

	inline void SetBackground(const Background& theBackground)  { mBackground = theBackground; Invalidate(); }
	inline void SetForeground(const Background& theForeground)    { mForeground = theForeground; Invalidate(); }

	// Overrides.
	virtual void Paint(Graphics &g);

	// Constructor.
	inline ProgressBarComponent(void)                 { Init(); }
	inline ~ProgressBarComponent(void)                {}
};

typedef SmartPtr<ProgressBarComponent> ProgressBarComponentPtr;

class ProgressComponentConfig : public ComponentConfig
{
protected:
	ProgressBarComponent* m_pProgressBar;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	ProgressComponentConfig(void) {}
	virtual void SetComponent(Component *theComponent);

	static ComponentConfig* CfgFactory() { return new ProgressComponentConfig; }
};

}

#endif
