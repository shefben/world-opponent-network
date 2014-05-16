#include "Splitter.h"
#include "Button.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Splitter::Splitter()
{
	mSplitLeft = 0;
	mSplitRight = 0;
	mSplitTop = 0;
	mSplitBottom = 0;

	mUpDownSplitterHeight = 0;
	mLeftRightSplitterWidth = 0;

	mHaveMaxQuadrant = false;

	mMinLeft = mMinRight = mMinTop = mMinBottom = 0;
	mLeftPercent = mTopPercent = 50;
	mUpDownMax = false;
	mLeftRightMax = false;

	mLeftRightDrag = false;
	mUpDownDrag = false;


	// SplitX and SplitY are the actual center of the splitter whereas 
	// splitLeft, splitRight,splitTop, and splitBottom are the sides of the center box
	mSplitX = 0;
	mSplitY = 0;
	mSplitCoordsValid = false;

	SetComponentFlags(ComponentFlag_Clip,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Splitter::~Splitter()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::RegisterMaxButton(Button *theButton, QuadrantPosition thePosition)
{
	theButton->SetControlId(thePosition + ControlId_MaxMin);
	mButtonList.push_back(theButton);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::AddQuadrant(Component *theComponent, QuadrantPosition thePosition)
{
	mQuadrantList.push_back(Quadrant(theComponent,thePosition));
	AddChild(theComponent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::AddSplitterPrv(Component *theSplitter, SplitterType theType, Component *theDragComponent)
{
	mSplitterList.push_back(SplitterParam(theSplitter,theType));
	SplitterParam &aParam = mSplitterList.back();

	theDragComponent->SetComponentFlags(ComponentFlag_ParentDrag,true);
	if(theDragComponent->GetDefaultCursor()==NULL)
	{
		if(theType==Splitter_Center)
			theDragComponent->SetDefaultCursor(Cursor::GetStandardCursor(StandardCursor_NorthSouthEastWest));
		else if(theType & Splitter_LeftRight)
			theDragComponent->SetDefaultCursor(Cursor::GetStandardCursor(StandardCursor_EastWest));
		else if(theType & Splitter_UpDown)
			theDragComponent->SetDefaultCursor(Cursor::GetStandardCursor(StandardCursor_NorthSouth));
	}
	aParam.mDragComponent = theDragComponent;

	AddChild(theSplitter);

	if(mLeftRightSplitterWidth==0 && (theType & Splitter_LeftRight))
		mLeftRightSplitterWidth = theSplitter->Width();

	if(mUpDownSplitterHeight==0 && (theType & Splitter_UpDown))
		mUpDownSplitterHeight = theSplitter->Height();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::AddSplitter(Component *theComponent, SplitterType theType)
{
	AddSplitterPrv(theComponent,theType,theComponent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::AddComplexSplitter(Container *theContainer, SplitterType theType, Component *theDragComponent)
{
	if(theDragComponent==NULL)
	{
		ComponentPtr aDragComponent = new Component;
		theContainer->AddChildLayout(aDragComponent,CLI_SameSize,theContainer);
		theContainer->AddChildEx(aDragComponent,0);
		theDragComponent = aDragComponent;
	}

	AddSplitterPrv(theContainer,theType,theDragComponent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::AddInvisibleSplitter(SplitterType theType, int theSize)
{
	ComponentPtr aComp = new Component;
	aComp->SetSize(theSize,theSize);
	AddSplitter(aComp,theType);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::MaxQuadrant(QuadrantPosition thePosition)
{
	mHaveMaxQuadrant = true;
	mCurMaxQuadrant = thePosition;

	mLeftRightMax = mUpDownMax = true;

	int x = mSplitX, y = mSplitY;
	if(thePosition & Position_Left)
		x = Width()-mLeftRightSplitterWidth/2;
	else if(thePosition & Position_Right)
		x = mLeftRightSplitterWidth/2;
	else 
		mLeftRightMax = false;

	if(thePosition & Position_Top)
		y = Height()-mUpDownSplitterHeight/2;
	else if(thePosition & Position_Bottom)
		y = mUpDownSplitterHeight/2;
	else
		mUpDownMax = false;


	for(ButtonList::iterator aButtonItr = mButtonList.begin(); aButtonItr!=mButtonList.end(); ++aButtonItr)
	{
		Button *aButton = *aButtonItr;
		int aPosition = aButton->GetControlId() - ControlId_MaxLeft + Position_Left;
		aButton->SetCheck(aPosition==thePosition,false);
	}

	SplitterList::iterator aSplitterItr = mSplitterList.begin();
	while(aSplitterItr!=mSplitterList.end())
	{
		SplitterParam &aParam = *aSplitterItr;
		Component *aComponent = aParam.mComponent;
		if(aParam.mType & thePosition)
			aParam.mDragComponent->Enable(false);
		else
			aParam.mDragComponent->Enable(true);

		++aSplitterItr;
	}

	QuadrantList::iterator aQuadrantItr = mQuadrantList.begin();
	while(aQuadrantItr!=mQuadrantList.end())
	{
		Quadrant &aParam = *aQuadrantItr;
		Component *aComponent = aParam.mComponent;
		if(aParam.mPosition & thePosition)
			aComponent->SetVisible(true);
		else
			aComponent->SetVisible(false);

		++aQuadrantItr;
	}

	SetSplitterPos(x,y,false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::UnMaxQuadrant()
{
	mHaveMaxQuadrant = false;
	mUpDownMax = mLeftRightMax = false;

	SplitterList::iterator aSplitterItr = mSplitterList.begin();
	while(aSplitterItr!=mSplitterList.end())
	{
		SplitterParam &aParam = *aSplitterItr;
		Component *aComponent = aParam.mComponent;
		aParam.mDragComponent->Enable(true);

		++aSplitterItr;
	}

	QuadrantList::iterator aQuadrantItr = mQuadrantList.begin();
	while(aQuadrantItr!=mQuadrantList.end())
	{
		Quadrant &aParam = *aQuadrantItr;
		Component *aComponent = aParam.mComponent;
		aComponent->SetVisible(true);
		++aQuadrantItr;
	}

	SetSplitterPos(Width()*mLeftPercent/100,Height()*mTopPercent/100,false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::CalcQuadrants()
{
	QuadrantList::iterator anItr = mQuadrantList.begin();
	while(anItr!=mQuadrantList.end())
	{
		Quadrant &aParam = *anItr;
		Component *aComponent = aParam.mComponent;
		int aPosition = aParam.mPosition;

		int left = 0, right = Width(), top = 0, bottom = Height();

		if(aPosition & Position_Left)
			right = mSplitLeft;

		if(aPosition & Position_Right)
			left = mSplitRight;

		if(aPosition & Position_Top)
			bottom = mSplitTop;

		if(aPosition & Position_Bottom)
			top = mSplitBottom;

		aComponent->SetPosSize(left,top,right-left,bottom-top);
		++anItr;
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::CalcSplitterPositions()
{
	SplitterList::iterator anItr = mSplitterList.begin();
	while(anItr!=mSplitterList.end())
	{
		SplitterParam &aParam = *anItr;
		Component *aComponent = aParam.mComponent;
		if(aComponent->IsVisible())
		{
			int left = 0, top = 0, right = Width(), bottom = Height();

			if(aParam.mType & Splitter_LeftRight)
			{
				left = mSplitLeft;
				right = mSplitRight;
			}

			if(aParam.mType & Splitter_UpDown)
			{
				top = mSplitTop;
				bottom = mSplitBottom;
			}

			switch(aParam.mType)
			{
				case Splitter_LeftRight_Top: bottom = mSplitTop; break;
				case Splitter_LeftRight_Bottom: top = mSplitBottom; break;
				case Splitter_UpDown_Left: right = mSplitLeft; break;
				case Splitter_UpDown_Right: left = mSplitRight; break;
			}

			aComponent->SetPosSize(left,top,right-left,bottom-top);
		}
		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::SetMinLeftRight(int minLeft, int minRight)
{
	mMinLeft = minLeft;
	mMinRight = minRight;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Splitter::SetMinTopBottom(int minTop, int minBottom)
{
	mMinTop = minTop;
	mMinBottom = minBottom;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::SetPercentages(int theLeftPercent, int theTopPercent)
{
	mLeftPercent = theLeftPercent;
	mTopPercent = theTopPercent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::SetSplitterPos(int x, int y, bool rememberPercentage)
{
	if(!mLeftRightMax)
	{
		if(x < mMinLeft)
			x = mMinLeft;
		if(x > Width()-mMinRight)
			x = Width()-mMinRight;
	}

	if(!mUpDownMax)
	{
		if(y < mMinTop)
			y = mMinTop;
		if(y > Height()-mMinBottom)
			y = Height()-mMinBottom;
	}

	if(mSplitCoordsValid && x==mSplitX && y==mSplitY)
		return;

	mSplitCoordsValid = true;
	mSplitX = x;
	mSplitY = y;
	mSplitLeft = x - mLeftRightSplitterWidth/2;
	mSplitRight = x + mLeftRightSplitterWidth/2;
	mSplitTop = y - mUpDownSplitterHeight/2;
	mSplitBottom = y + mUpDownSplitterHeight/2;

	if(rememberPercentage)
	{
		if(Width() > 0)
			mLeftPercent = x*100.0/Width();
		if(Height() > 0)
			mTopPercent = y*100.0/Height();
	}

	CalcSplitterPositions();
	CalcQuadrants();
	Invalidate();

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::SizeChanged()
{
	Container::SizeChanged();

	mSplitCoordsValid = false;
	double aLeftPercent = mLeftPercent;
	double aTopPercent = mTopPercent;
	if(mHaveMaxQuadrant)
	{
		int xoff = 0, yoff = 0;
		if(mCurMaxQuadrant & Position_Left)
		{
			aLeftPercent = 100;
			xoff = -mLeftRightSplitterWidth/2;
		}
		else if(mCurMaxQuadrant & Position_Right)
		{
			aLeftPercent = 0;
			xoff = mLeftRightSplitterWidth/2;
		}

		if(mCurMaxQuadrant & Position_Top)
		{
			aTopPercent = 100;
			yoff = -mUpDownSplitterHeight/2;
		}
		else if(mCurMaxQuadrant & Position_Bottom)
		{
			aTopPercent = 0;
			yoff = mUpDownSplitterHeight/2;
		}

		SetSplitterPos(Width()*aLeftPercent/100 + xoff, Height()*aTopPercent/100 + yoff,false);
	}
	else		
		SetSplitterPos(Width()*aLeftPercent/100,Height()*aTopPercent/100,false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::MouseDown(int x, int y, MouseButton theButton)
{
	Container::MouseDown(x,y,theButton);
	if(theButton!=MouseButton_Left || mMouseChild==NULL)
		return;

	mLeftRightDrag = mUpDownDrag = false;
	mDragX = x - mMouseChild->Left();
	mDragY = y - mMouseChild->Top();

	Component *aGrandChild = mMouseChild->GetMouseChild();
	if(aGrandChild==NULL)
	{
		if(!mMouseChild->ComponentFlagSet(ComponentFlag_ParentDrag))
			return;
	}
	else if(!aGrandChild->ComponentFlagSet(ComponentFlag_ParentDrag))
		return;

	SplitterList::iterator anItr = mSplitterList.begin();
	while(anItr!=mSplitterList.end())
	{
		SplitterParam &aParam = *anItr;
		if(aParam.mComponent==mMouseChild)
		{
			if(aParam.mType & Splitter_UpDown)
				mUpDownDrag = true;

			if(aParam.mType & Splitter_LeftRight)
				mLeftRightDrag = true;

			break;
		}

		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::MouseUp(int x, int y, MouseButton theButton)
{
	Container::MouseUp(x,y,theButton);
	if(theButton!=MouseButton_Left || mMouseChild==NULL)
		return;

	mLeftRightDrag = mUpDownDrag = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::MouseDrag(int x, int y)
{
	Container::MouseDrag(x,y);
	if(mMouseChild!=NULL)
	{
		int aDeltaX = 0, aDeltaY = 0;
		if(mLeftRightDrag && !mLeftRightMax)
			aDeltaX = x - mMouseChild->Left() - mDragX;

		if(mUpDownDrag && !mUpDownMax)
			aDeltaY = y - mMouseChild->Top() - mDragY; 

		SetSplitterPos(mSplitX+aDeltaX, mSplitY+aDeltaY);

//		AdjustSplitters(aDeltaX, aDeltaY);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Splitter::CheckMaxButton(ComponentEvent *theEvent)
{
	if(theEvent->mType!=ComponentEvent_ButtonPressed)
		return false;

	if(theEvent->GetControlId()<ControlId_MaxLeft || theEvent->GetControlId()>ControlId_MaxBottomRight)
		return false;

	Button *aButton = dynamic_cast<Button*>(theEvent->mComponent);
	if(aButton==NULL)
		return false;
	
	if(!aButton->IsChecked())
	{
		UnMaxQuadrant();
		return true;
	}

	int aType = theEvent->GetControlId() - ControlId_MaxLeft + Position_Left;
	MaxQuadrant((QuadrantPosition)aType);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Splitter::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(CheckMaxButton(theEvent))
		return;
	else 
		Container::HandleComponentEvent(theEvent);
}
