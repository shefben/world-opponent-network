#include "ChildLayouts.h"
#include "Container.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChildLayoutList::ChildLayoutList(Component *c1)
{
	mChildList.push_back(c1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChildLayoutList::ChildLayoutList(Component *c1, Component *c2)
{
	mChildList.push_back(c1); mChildList.push_back(c2);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChildLayoutList::ChildLayoutList(Component *c1, Component *c2, Component *c3)
{
	mChildList.push_back(c1); mChildList.push_back(c2); mChildList.push_back(c3);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChildLayoutList::ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4)
{
	mChildList.push_back(c1); mChildList.push_back(c2); mChildList.push_back(c3); mChildList.push_back(c4);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChildLayoutList::ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4, Component *c5)
{
	mChildList.push_back(c1); mChildList.push_back(c2); mChildList.push_back(c3); mChildList.push_back(c4); 
	mChildList.push_back(c5);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChildLayoutList::ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4, Component *c5, Component *c6)
{
	mChildList.push_back(c1); mChildList.push_back(c2); mChildList.push_back(c3); mChildList.push_back(c4); 
	mChildList.push_back(c5); mChildList.push_back(c6);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChildLayoutList::ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4, Component *c5, Component *c6, Component *c7)
{
	mChildList.push_back(c1); mChildList.push_back(c2); mChildList.push_back(c3); mChildList.push_back(c4); 
	mChildList.push_back(c5); mChildList.push_back(c6); mChildList.push_back(c7);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChildLayoutList::ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4, Component *c5, Component *c6, Component *c7, Component *c8)
{
	mChildList.push_back(c1); mChildList.push_back(c2); mChildList.push_back(c3); mChildList.push_back(c4); 
	mChildList.push_back(c5); mChildList.push_back(c6); mChildList.push_back(c7); mChildList.push_back(c8);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChildLayoutList::Rewind() const
{
	mItr = mChildList.begin();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ChildLayoutList::HasMore() const
{	
	return mItr!=mChildList.end();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Component* ChildLayoutList::GetNext() const
{
	if(mItr==mChildList.end())
		return NULL;
	else
		return *mItr++;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChildLayoutList::AddTo(ChildList &theList) const
{
	Rewind();
	while(HasMore())
		theList.push_back(GetNext());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChildLayoutInstruction::Execute(Component *theParent)
{
	int refLeft, refTop;
	if(mReference==mTarget->GetParent())
	{
		refLeft = 0;
		refTop = 0;
	}
	else
	{
		refLeft = mReference->Left();
		refTop = mReference->Top();
	}

	int refWidth = mReference->Width();
	int refHeight = mReference->Height();
	int refRight = refLeft + refWidth;
	int refBottom = refTop + refHeight;

	int left = mTarget->Left();
	int top = mTarget->Top();
	int width = mTarget->Width();
	int height = mTarget->Height();

	int aType = 1;
	while(aType<CLI_Max)
	{
		if(mType&aType)
		{
			switch(aType)
			{
				case CLI_SameWidth: width = refWidth+mHorzSizePad; break;
				case CLI_SameHeight: height = refHeight+mVertSizePad; break;
	
				case CLI_Above: top = refTop-height+mVertPosPad; break;
				case CLI_Below: top = refBottom+mVertPosPad; break;
				case CLI_Right: left = refRight+mHorzPosPad; break;
				case CLI_Left:  left = refLeft-width+mHorzPosPad; break;
			
				case CLI_SameLeft: left = refLeft+mHorzPosPad; break;
				case CLI_SameRight: left = refRight-width+mHorzPosPad; break;
				case CLI_SameTop: top = refTop+mVertPosPad; break;
				case CLI_SameBottom: top = refBottom-height+mVertPosPad; break;

				case CLI_GrowToRight: width = refRight-left+mHorzSizePad; break;
				case CLI_GrowToLeft: width = refLeft-left+mHorzSizePad; break;
				case CLI_GrowToTop: height = refTop-top+mVertSizePad; break;
				case CLI_GrowToBottom: height = refBottom-top+mVertSizePad; break;
			}
		}

		aType<<=1;
	}

	mTarget->SetPosSize(left,top,width,height);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PercentLayoutParam::Init(Component *left, Component *top, Component *right, Component *bottom, int px, int py, int pw, int ph, int lp, int tp, int rp, int bp)
{
	SetBorders(left,top,right,bottom);
	SetPercentages(px,py,pw,ph);
	SetPadding(lp,tp,rp,bp);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PercentLayoutParam::PercentLayoutParam(Component *ref, int px, int py, int pw, int ph, int lp, int tp, int rp, int bp)
{
	Init(ref,ref,ref,ref,px,py,pw,ph,lp,tp,rp,bp);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PercentLayoutParam::PercentLayoutParam(Component *ref1, Component *ref2, int px, int py, int pw, int ph, int lp, int tp, int rp, int bp)
{
	Init(ref1,ref2,ref1,ref2,px,py,pw,ph,lp,tp,rp,bp);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PercentLayoutParam::PercentLayoutParam(Component *left, Component *top, Component *right, Component *bottom, int px, int py, int pw, int ph, int lp, int tp, int rp, int bp)
{
	Init(left,top,right,bottom,px,py,pw,ph,lp,tp,rp,bp);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PercentLayoutParam::SetBorders(Component *theLeft, Component *theTop, Component *theRight, Component *theBottom)
{
	left = theLeft;
	top = theTop;
	right = theRight;
	bottom = theBottom;

	Component *aValid = NULL;
	if(left==NULL) left = right; else aValid = left;
	if(right==NULL) right = left; else aValid = right;
	if(top==NULL) top = bottom; else aValid = top;
	if(bottom==NULL) bottom = top; else aValid = bottom;

	if(left==NULL) left = aValid;
	if(right==NULL) right = aValid;
	if(top==NULL) top = aValid;
	if(bottom==NULL) bottom = aValid;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PercentLayoutParam::SetPercentages(int thePercentX, int thePercentY, int thePercentWidth, int thePercentHeight)
{
	px = thePercentX;
	py = thePercentY;
	pw = thePercentWidth;
	ph = thePercentHeight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PercentLayoutParam::SetPadding(int theLeftPad, int theTopPad, int theRightPad, int theBottomPad)
{
	lp = theLeftPad;
	tp = theTopPad;
	rp = theRightPad;
	bp = theBottomPad;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PercentLayoutParam::DoCalc(Component *theTarget, Component *theParent)
{
	int x = theTarget->Left();
	int y = theTarget->Top();
	int w = theTarget->Width();
	int h = theTarget->Height();

	if(px>=0 || pw>=0)
	{
		int l =	(left==theParent)?0:left->Left();
		int r = (right==theParent)?theParent->Width():right->Right();
		l+=lp;
		r-=rp;

		if(px>=0)
			x = l + (r-l)*px/100;

		if(pw>=0)
			w = (r-l)*pw/100;
	}

	if(py>=0 || ph>=0)
	{

		int t =	(top==theParent)?0:top->Top();
		int b = (bottom==theParent)?theParent->Height():bottom->Bottom();

		t+=tp;
		b-=bp;

		if(py>=0)
			y = t + (b-t)*py/100;

		if(ph>=0)
			h = (b-t)*ph/100; 
	}

	theTarget->SetPosSize(x,y,w,h);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PercentLayout::PercentLayout(Component *target, Component *ref, int px, int py, int pw, int ph, int lp, int tp, int rp, int bp)
: mParam(ref,px,py,pw,ph,lp,tp,rp,bp) 
{
	mTarget = target;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PercentLayout::PercentLayout(Component *target, Component *ref1, Component *ref2, int px, int py, int pw, int ph, int lp, int tp, int rp, int bp)
: mParam(ref1,ref2,px,py,pw,ph,lp,tp,rp,bp) 
{
	mTarget = target;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PercentLayout::PercentLayout(Component *target, Component *left, Component *top, Component *right, Component *bottom, int px, int py, int pw, int ph, int lp, int tp, int rp, int bp)
: mParam(left,top,right,bottom,px,py,pw,ph,lp,tp,rp,bp) 
{
	mTarget = target;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PercentLayout::Execute(Component *theParent)
{
	mParam.DoCalc(mTarget,theParent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CenterLayout::Execute(Component *theParent)
{
	if(mComponentList.empty())
		return;

	ComponentList::iterator anItr;

	WONRectangle aRect;
	for(anItr = mComponentList.begin(); anItr!=mComponentList.end(); ++anItr)
		aRect.DoUnion((*anItr)->GetBounds());

	int dx = 0, dy = 0;
	Component *aParent = theParent;
	if(mCenterBox!=NULL && mCenterBox!=theParent)
	{
		aParent = mCenterBox;
		if(mHorz)
			dx = aParent->Left();
	
		if(mVert)
			dy = aParent->Top();
	}

	if(mHorz)
		dx += (aParent->Width()-aRect.Width())/2 - aRect.Left();

	if(mVert)
		dy += (aParent->Height()-aRect.Height())/2 - aRect.Top();

	if(dx==0 && dy==0)
		return;

	for(anItr = mComponentList.begin(); anItr!=mComponentList.end(); ++anItr)
	{
		Component *aComponent = *anItr;
		aComponent->SetPos(aComponent->Left()+dx, aComponent->Top()+dy);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CenterLayout::Center(Component *theParent, Component *theChild, bool horz, bool vert)
{
	CenterLayoutPtr aLayout = new CenterLayout(theParent,theChild,horz,vert);
	aLayout->Execute(theParent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
VerticalFlowLayout::VerticalFlowLayout(Component *theTop)
{
	mTopComp = theTop;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
VerticalFlowLayout* VerticalFlowLayout::Add(Component *theChild, int theTopPad, int theLeftPad)
{
	mComponentList.push_back(ItemInfo(theChild,theTopPad,theLeftPad));
	return this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void VerticalFlowLayout::Execute(Component *theParent)
{
	if(mComponentList.empty())
		return;

	ComponentList::iterator anItr = mComponentList.begin(); 
	int aTop, aLeft;
	if(anItr->mComponent->GetParent()==mTopComp)
	{
		aTop = 0;
		aLeft = 0;
	}
	else
	{
		aTop = mTopComp->Bottom();
		aLeft = mTopComp->Left();
	}
	
	for(; anItr!=mComponentList.end(); ++anItr)
	{
		aTop += anItr->mTopPad;
		Component *aComp = anItr->mComponent;
		int aLeftPad = anItr->mLeftPad;
		if(aLeftPad>=0)
			aComp->SetPos(aLeft+aLeftPad,aTop);
		else
			aComp->SetTop(aTop);

		aTop += aComp->Height();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SameSizeLayout* SameSizeLayout::Add(Component *theComp)
{
	mComponentList.push_back(theComp);
	return this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SameSizeLayout::Execute(Component *theParent)
{
	int aMaxWidth = 0;
	int aMaxHeight = 0;
	ComponentList::iterator anItr;
	for(anItr = mComponentList.begin(); anItr!=mComponentList.end(); ++anItr)
	{
		Component *aComp = *anItr;
		if(mHorz && aComp->Width()>aMaxWidth)
			aMaxWidth = aComp->Width();
		if(mVert && aComp->Height()>aMaxHeight)
			aMaxHeight = aComp->Height();		
	}

	for(anItr = mComponentList.begin(); anItr!=mComponentList.end(); ++anItr)
	{
		Component *aComp = *anItr;
		if(mHorz && mVert)
			aComp->SetSize(aMaxWidth,aMaxHeight);
		else if(mHorz)
			aComp->SetWidth(aMaxWidth);
		else if(mVert)
			aComp->SetHeight(aMaxHeight);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ClearRightLayout::Execute(Component *theParent)
{
	int aMaxRight = 0;
	for(ComponentList::iterator anItr = mComponentList.begin(); anItr!=mComponentList.end(); ++anItr)
	{
		int aRight = (*anItr)->Right();
		if(aRight>aMaxRight)
			aMaxRight = aRight;
	}

	aMaxRight += mPad;
	mComponent->SetPos(aMaxRight,mComponent->Top());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SpaceEvenlyLayout::Execute(Component *theParent)
{
	if(mComponentList.empty())
		return;

	int aTotalSpace = 0;
	ComponentList::iterator anItr;
	for(anItr = mComponentList.begin(); anItr!=mComponentList.end(); ++anItr)
		aTotalSpace += mIsHorz?(*anItr)->Width():(*anItr)->Height();

	int anAvailableSpace = mIsHorz?mSpaceBox->Width():mSpaceBox->Height();
	int anExtraSpace = anAvailableSpace - mLeftPad - mRightPad - aTotalSpace;

	if(anExtraSpace<0)
		anExtraSpace = 0;


	int pad = 0;
	int remainder = 0;
	if(mComponentList.size()>1)
	{
		pad = anExtraSpace/(mComponentList.size()-1);
		remainder = anExtraSpace%(mComponentList.size()-1);
	}

	int pos = mIsHorz?mSpaceBox->Left():mSpaceBox->Top();
	if(mSpaceBox==theParent)
		pos = 0;

	pos+=mLeftPad;
	for(anItr = mComponentList.begin(); anItr!=mComponentList.end(); ++anItr)
	{
		Component *aComponent = *anItr;

		if(mIsHorz)
			aComponent->SetPos(pos,aComponent->Top());
		else
			aComponent->SetPos(aComponent->Left(),pos);
		
		if(mIsHorz)
			pos+=aComponent->Width() + pad;
		else
			pos+=aComponent->Height() + pad;

		if(remainder>0)
		{
			pos++;
			remainder--;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void NoOverlapLayout::Execute(Component *theParent)
{
	int dx = 0, dy = 0;
	if(mRef==theParent)
	{
		if(mType&CLI_Left)
		{
			dx = mHorzPad - mTarget->Left();
			if(dx<0)
				dx = 0;
		}
		else if(mType&CLI_Right)
		{
			dx = mRef->Width() + mHorzPad - mTarget->Right();
			if(dx>0)
				dx = 0;
		}

		if(mType&CLI_Above)
		{
			dy = mVertPad - mTarget->Top();
			if(dy<0)
				dy = 0;
		}
		else if(mType&CLI_Below)
		{
			dy = mRef->Height() + mVertPad - mTarget->Bottom();
			if(dy>0)
				dy = 0;
		}
	}
	else
	{
		if(mType&CLI_Left)
		{
			dx = mRef->Left() + mHorzPad - mTarget->Right();
			if(dx>0)
				dx = 0;
		}
		else if(mType&CLI_Right)
		{
			dx = mRef->Right() + mHorzPad - mTarget->Left();
			if(dx<0)
				dx = 0;
		}

		if(mType&CLI_Above)
		{
			dy = mRef->Top() + mVertPad - mTarget->Bottom();
			if(dy>0)
				dy = 0;
		}
		else if(mType&CLI_Below)
		{
			dy = mRef->Bottom() + mVertPad - mTarget->Top();
			if(dy<0)
				dy = 0;
		}

	}

	if(dx==0 && dy==0)
		return;

	mTarget->SetPos(mTarget->Left()+dx, mTarget->Top()+dy);
	for(ComponentList::iterator anItr = mComponentList.begin(); anItr!=mComponentList.end(); ++anItr)
	{
		Component *aComp = *anItr;
		aComp->SetPos(aComp->Left()+dx, aComp->Top()+dy);
	}	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SizeAtLeastLayout::Execute(Component *theParent)
{
	int aMaxWidth = 0;
	int aMaxHeight = 0;
	ComponentList::iterator anItr;
	for(anItr = mComponentList.begin(); anItr!=mComponentList.end(); ++anItr)
	{
		Component *aComp = *anItr;
		if(aComp->Width()>aMaxWidth)
			aMaxWidth = aComp->Width();
		if(aComp->Height()>aMaxHeight)
			aMaxHeight = aComp->Height();		
	}

	aMaxWidth += mHorzPad;
	aMaxHeight += mVertPad;

	mTarget->SetSizeAtLeast(aMaxWidth,aMaxHeight);
}

