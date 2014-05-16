#include "Container.h"
#include "ChildLayouts.h"


using namespace WONAPI;

bool Container::mMouseWheelGoesToFocusChildDef = false;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Container::Container()
{
	mMouseChild = NULL;
	mFocusChild = NULL;

	mCurrentCursor = NULL;
	mChildNeedsTimer = false;

	mMouseWheelGoesToFocusChild = mMouseWheelGoesToFocusChildDef;
	mOnlyChildInput = false;
	mIsolateTabFocus = false;
	mRemovingAllChildren = false;
	mHasUsedDialogKeys = false;

	mModalCount = 0;
	mNextFocusId = 0;

	SetComponentFlags(ComponentFlag_WantFocus,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Container::~Container()
{
//	RemoveAllChildren();

	// Mark children as not having a parent since I'm deleted
	for(ChildList::iterator anItr = mChildList.begin(); anItr!=mChildList.end(); ++anItr)
		(*anItr)->SetParent(NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned char Container::GetNextFocusId()
{
	mNextFocusId++;
	if(mNextFocusId==0)
		mNextFocusId++;

	return mNextFocusId;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::SetFocusToLastFocusChild()
{
	unsigned char aCurFocusId = mNextFocusId;
	bool skipCur = false;
	if(mFocusChild!=NULL)
	{
		aCurFocusId = mFocusChild->GetFocusId();
		mFocusChild->LostFocus();
		mFocusChild = NULL;
		skipCur = true;
	}

	Component *aLast = NULL;
	unsigned char aMinDist = 0;
	for(ChildList::iterator anItr = mChildList.begin(); anItr!=mChildList.end(); ++anItr)
	{
		unsigned char aFocusId = (*anItr)->GetFocusId();
		if(aFocusId==0)
			continue;

		if(aFocusId==aCurFocusId && skipCur)
			continue;

		if(aMinDist==0 || aCurFocusId-aFocusId<aMinDist)
		{
			aMinDist = aCurFocusId - aFocusId;
			aLast = *anItr;
		}
	}

	if(aLast!=NULL)
		aLast->RequestFocus();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::AddLayoutPoint(LayoutPoint *thePoint)
{
	thePoint->SetParent(this);
	mChildLayout.AddLayoutPoint(thePoint);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::ClearChildLayouts()
{
	mChildLayout.Clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::EnableLayout(int theId, bool enable)
{
	mChildLayout.EnableInstructions(theId,enable);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::AddChildLayout(Component *target, int theType, Component *ref, 
							   int horzPosPad, int vertPosPad, int horzSizePad, int vertSizePad, int theId)
{
	ChildLayoutInstruction *anInstr = new ChildLayoutInstruction;
	anInstr->mType = theType;
	anInstr->mTarget = target;
	anInstr->mReference = ref;
	anInstr->mHorzPosPad = horzPosPad;
	anInstr->mVertPosPad = vertPosPad;
	anInstr->mHorzSizePad = horzSizePad;
	anInstr->mVertSizePad = vertSizePad;
	AddChildLayout(anInstr,theId);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::AddChildLayout(ChildLayoutInstructionBase *theInstr, int theId)
{
	theInstr->mId = theId;
	if(theId==0)
	{
		theInstr->mEnabled = true;
		theInstr->Execute(this);
	}
	else
		theInstr->mEnabled = false;

	mChildLayout.AddInstruction(theInstr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::EnterModal()
{
	mModalCount++;
	if(mMouseChild!=NULL && !mMouseChild->ComponentFlagSet(ComponentFlag_AllowModalInput))
		MouseAt();

	if(mFocusChild!=NULL && !mFocusChild->ComponentFlagSet(ComponentFlag_AllowModalInput))
	{
		SetFocusToLastFocusChild();

//		mFocusChild = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::LeaveModal()
{
	if(mModalCount > 0)
	{
		mModalCount = 0;
		MouseAt();

		if(mFocusChild==NULL)
	 		SetFocusToLastFocusChild();

	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::RecalcLayout()
{
	SizeChanged();
	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::SizeChanged()
{
	Component::SizeChanged();
	mChildLayout.ExecuteInstructions(this);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::GetDesiredSize(int &width, int &height)
{
	int aMaxRight = 0;
	int aMaxBottom = 0;
	for(ChildList::iterator anItr = mChildList.begin(); anItr!=mChildList.end(); ++anItr)
	{
		Component *aComponent = *anItr;
		if(aComponent->Right() > aMaxRight)
			aMaxRight = aComponent->Right();
		if(aComponent->Bottom() > aMaxBottom)
			aMaxBottom = aComponent->Bottom();
	}

	width = aMaxRight;
	height = aMaxBottom;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::PrePaint(Graphics &g)
{
	if(!mChildList.empty())
	{
		ChildList::iterator anItr = mChildList.end();
		do
		{	
			--anItr;

			Component *aComponent = *anItr;
			if(aComponent->NeedPrePaint())
			{
				g.Translate(aComponent->Left(), aComponent->Top());
				aComponent->PrePaint(g);
				g.Translate(-aComponent->Left(), -aComponent->Top());

/*				if(aComponent->ComponentFlagSet(ComponentFlag_RemoveAfterPrePaint))
				{
					aComponent->SetParent(NULL);
					ChildList::iterator anEraseItr = anItr;
					bool atBeginning = anItr == mChildList.begin();
					if(!atBeginning)
						--anItr;

					mChildList.erase(anEraseItr);
					if(atBeginning)
						anItr = mChildList.begin();
				}*/
			}
		} while(anItr!=mChildList.begin());
	}

	Component::PrePaint(g);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::Paint(Graphics &g)
{
	ColorScheme *cs = g.GetColorScheme();

	Component::Paint(g);

	ChildList::iterator anItr = mChildList.begin();
	while(anItr!=mChildList.end())
	{
		Component *aComponent = *anItr;
		if(aComponent->IsInvalid() && aComponent->IsVisible())
		{
			g.Translate(aComponent->Left(), aComponent->Top());
			if(aComponent->ComponentFlagSet(ComponentFlag_Clip))
				g.PushClipRect(0,0,aComponent->Width(),aComponent->Height());

			if(aComponent->GetOwnColorScheme()!=NULL)
				g.SetColorScheme(aComponent->GetOwnColorScheme());
			else
				g.SetColorScheme(cs);

			aComponent->Paint(g);

			if(aComponent->ComponentFlagSet(ComponentFlag_Clip))
				g.PopClipRect();
		
			g.Translate(-aComponent->Left(), -aComponent->Top());
		}

		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::EnableControls(const ControlIdSet &theSet, bool enable)
{
	Component::EnableControls(theSet,enable);

	ChildList::iterator anItr = mChildList.begin();
	while(anItr!=mChildList.end())
	{
		Component *aComponent = *anItr;
		aComponent->EnableControls(theSet,enable);
		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::NotifyRootVisibilityChange(bool isVisible)
{
	Component::NotifyRootVisibilityChange(isVisible);
	mHasUsedDialogKeys = false;

	ChildList::iterator anItr = mChildList.begin();
	while(anItr!=mChildList.end())
	{
		Component *aComponent = *anItr;
		if(aComponent->IsVisible())
			aComponent->NotifyRootVisibilityChange(isVisible);
	
		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Container::WantInput(int x, int y)
{
	if(!Component::WantInput(x,y))
		return false;

	if(mOnlyChildInput)
	{
		ChildList::iterator anItr = mChildList.end();
		do
		{
			--anItr;
			Component *aComponent = *anItr;
			if(aComponent->Contains(x,y) && aComponent->WantInput(x-aComponent->Left(), y-aComponent->Top()))
				return true;;
				
		} while(anItr!=mChildList.begin());

		return false;
	}
	else		
		return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Component* Container::FindChildAt(int x, int y)
{	
	if(mChildList.empty())
		return NULL;

	ChildList::iterator anItr = mChildList.end();
	do
	{
		--anItr;
		Component *aComponent = *anItr;
		if(aComponent->Contains(x,y) && aComponent->WantInput(x-aComponent->Left(), y-aComponent->Top()))
		{
			if(aComponent->Disabled())
				return NULL;

			if(mModalCount>0 && !aComponent->ComponentFlagSet(ComponentFlag_AllowModalInput))
				return NULL;

			return aComponent;
		}
			
	} while(anItr!=mChildList.begin());

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::MouseAt(int x, int y)
{	
	Component *aComponent = FindChildAt(x,y);
	if (aComponent != mMouseChild)
	{
		if (mMouseChild != NULL)
		{
			mMouseChild->MouseExit();
			mMouseChild = aComponent;  // since SetCursor checks the MouseChild
			SetCursor(GetDefaultCursor());
		}
		else
			mMouseChild = aComponent;
		
		if (aComponent != NULL)
			aComponent->MouseEnter(x-aComponent->Left(), y-aComponent->Top());			
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::MouseAt()
{
	if(GetWindow()->MouseIsInClientArea())
	{
		int x, y;
		GetMousePos(x,y);
		MouseMove(x,y);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::MouseEnter(int x, int y)
{
	Component::MouseEnter(x,y);
	MouseAt(x,y); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::MouseExit()
{
	Component::MouseExit();
	if(mMouseChild!=NULL)
	{
		mMouseChild->MouseExit();
		mMouseChild = NULL;
	}
	mCurrentCursor = NULL;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::MouseMove(int x, int y)
{
	MouseAt(x, y);
	if (mMouseChild != NULL)
		mMouseChild->MouseMove(x-mMouseChild->Left(), y-mMouseChild->Top());		
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Container::MouseWheel(int theAmount)
{
//	MouseAt(x, y);
	Component *aWheelChild = mMouseChild;
	if(mMouseWheelGoesToFocusChild /*&& mFocusChild!=NULL*/)
		aWheelChild = mFocusChild;

	if (aWheelChild != NULL)
		return aWheelChild->MouseWheel(theAmount);	
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::MouseHover(DWORD theElapsed)
{
	Component::MouseHover(theElapsed);
	if(mMouseChild!=NULL)
		mMouseChild->MouseHover(theElapsed);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::MouseDrag(int x, int y)
{
	if(mMouseChild != NULL)
		mMouseChild->MouseDrag(x-mMouseChild->Left(), y-mMouseChild->Top());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::MouseUp(int x, int y, MouseButton theButton)
{
	Component *oldMouseChild = mMouseChild;
	MouseAt(x, y);
	if (mMouseChild!=NULL && mMouseChild==oldMouseChild)
		mMouseChild->MouseUp(x-mMouseChild->Left(), y-mMouseChild->Top(), theButton);		
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::MouseDown(int x, int y, MouseButton theButton)
{		
	MouseAt(x, y);
	if(mMouseChild!=NULL)
		mMouseChild->MouseDown(x-mMouseChild->Left(), y-mMouseChild->Top(), theButton);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Container::InvalidateUp(Component *theChild)
{

	if(IsSelfInvalid()) // already completely invalid
		return false;
	
	mInvalidState = InvalidState_PartiallyInvalid; // some children are invalid

	bool higherWidgetIsInvalid = false;
	ChildList::iterator anItr = mChildList.end();
	while(anItr!=mChildList.begin())
	{
		--anItr;

		Component *aChild = *anItr;
		if(aChild==theChild)
			break;

		if(aChild->Intersects(theChild) && aChild->IsVisible())
		{
			aChild->InvalidateFully();				
			higherWidgetIsInvalid = true;
		}
	}
			
	// If partially invalidating myself causes a widget that paints in front of me to 
	// become invalid, then entirely invalidate myself
//	if(mParent!=NULL && mParent->InvalidateUp(this))
//		Invalidate();
	if(mParent!=NULL)
		mParent->InvalidateUp(this);

	return higherWidgetIsInvalid;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::InvalidateDown()
{
	if(IsSelfInvalid()) // already invalidated the children
	{
		Component::InvalidateDown(); 
		return;
	}

	Component::InvalidateDown();
	mInvalidState = InvalidState_FullyInvalid;
	ChildList::iterator anItr = mChildList.begin();
	while(anItr!=mChildList.end())
	{
		(*anItr)->InvalidateDown();
		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::Invalidate()
{			
	Component::Invalidate();		
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::AddedToParent()
{
	Component::AddedToParent();
	mHasUsedDialogKeys = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::AddedToRoot()
{
	Component::AddedToRoot();
	ChildList::iterator anItr = mChildList.begin();
	while(anItr!=mChildList.end())
	{
		(*anItr)->AddedToRoot();
		++anItr;
	}
}

/*
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::Validate()
{
	Component::Validate();
	ChildList::iterator anItr = mChildList.begin();
	while(anItr!=mChildList.end())
	{
		(*anItr)->Validate();
		++anItr;
	}
}*/

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChildList::iterator Container::FindTop(ChildList::iterator theItr)
{
//	if(mChildList.empty())
//		return mChildList.end();
	if(theItr==mChildList.begin())
		return mChildList.begin();

	ChildList::iterator anItr = theItr;//mChildList.end();
	--anItr;
	while(true)
	{
		Component *aChild = *anItr;
		if(!aChild->ComponentFlagSet(ComponentFlag_AlwaysOnTop))
			return ++anItr;

		
		if(anItr==mChildList.begin())
			return anItr;

		--anItr;
	} 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChildList::iterator Container::FindTop()
{
	return FindTop(mChildList.end());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::AddChild(Component *theChild)
{
	// A component can't have two parents
	if(theChild->GetParent()!=NULL)
		theChild->GetParent()->RemoveChild(theChild);

	theChild->SetParent(this);
	theChild->SetChildListItr(mChildList.insert(FindTop(), theChild));
	theChild->AddedToParent();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::AddChildEx(Component *theChild, int thePos)
{
	// A component can't have two parents
	if(theChild->GetParent()!=NULL)
		theChild->GetParent()->RemoveChild(theChild);

	theChild->SetParent(this);
	ChildList::iterator anItr = mChildList.begin();
	for(int i=0; i<thePos && anItr!=mChildList.end(); i++)
		++anItr;

	theChild->SetChildListItr(mChildList.insert(FindTop(anItr), theChild));
	theChild->AddedToParent();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::RemoveChild(Component *theChild)
{
	if(theChild->GetParent()!=this)
		return;

	ComponentPtr aPrePaintReplace;
	if(!mRemovingAllChildren && theChild->ComponentFlagSet(ComponentFlag_GrabBG))
	{
		theChild->InvalidateFully();
		RootContainer *aRoot = GetRoot();
		if(aRoot!=NULL)
			aRoot->AddBG(theChild->GetGrabBG());
	}
	else
		Invalidate();

	ComponentPtr aChild = theChild; // keep the extra reference around (since child is erased from mChildList and that might be the last reference)
//	theChild->SetParent(NULL);
	mChildList.erase(theChild->GetChildListItr());
	if(!mRemovingAllChildren)
		mChildLayout.RemoveComponent(theChild);

	if(theChild==mFocusChild)
	{
		SetFocusToLastFocusChild();
	}

	if(theChild==mMouseChild)
	{
//		mMouseChild = NULL;
		MouseAt(); // Let MouseAt() set the mouse child to NULL
	}	
	theChild->SetParent(NULL); // we don't want the parent to be NULL until after we've called LostFocus and/or MouseExit
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::RemoveAllChildren()
{
	mRemovingAllChildren = true;
	while(!mChildList.empty())
		RemoveChild(mChildList.front());

	mChildLayout.Clear();
	mRemovingAllChildren = false;
	SetCursor(NULL);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::BringToTop(Component *theChild)
{
	if(theChild->GetParent()!=this)
		return;

	ComponentPtr aChild = theChild; // keep the extra reference around (since child is erased from mChildList and that might be the last reference)

	BackgroundGrabber *aGrabber = theChild->GetGrabBG();
	RootContainer *aRoot = GetRoot();
	bool needInvalidate = true;
	if(aGrabber!=NULL)
	{
		theChild->Invalidate();
		aRoot->AddBG(aGrabber);
		needInvalidate = false;
	}

	mChildList.erase(theChild->GetChildListItr());
	theChild->SetChildListItr(mChildList.insert(FindTop(), theChild));

	if(needInvalidate)		
		Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::EnsureOnTopOf(Component *theBottom, Component *theTop)
{
	if(theBottom->GetParent()!=this || theTop->GetParent()!=this)
		return;

	ChildList::iterator anItr = theBottom->GetChildListItr();
	while(anItr!=theTop->GetChildListItr() && anItr!=mChildList.end())
		++anItr;

	if(anItr!=mChildList.end())
		return;

	ComponentPtr aBottom = theBottom; // keep the extra reference around (since child is erased from mChildList and that might be the last reference)
	mChildList.erase(theBottom->GetChildListItr());
	theBottom->SetChildListItr(mChildList.insert(theTop->GetChildListItr(),theBottom));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::ChildRequestTimer(Component *theChild)
{
	if(theChild->WantTimer()) // need timer
	{
		mChildTimerSet.insert(theChild);
		bool needRequestParent = !WantTimer();
		mTimerFlags |= TimerFlag_ChildWants;
		if(needRequestParent && mParent!=NULL)
			mParent->ChildRequestTimer(this);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Container::TimerEvent(int theDelta)
{
	bool childNeedsTimer = false;
	ChildSet::iterator anItr = mChildTimerSet.begin();
	while(anItr!=mChildTimerSet.end())
	{
		Component *aChild = *anItr;
		if(aChild->WantTimer() && aChild->GetParent()==this && aChild->TimerEvent(theDelta))
		{
			childNeedsTimer = true;
			++anItr;
		}
		else
		{
			aChild->RequestTimer(false);
			mChildTimerSet.erase(anItr++);
		}
	}

	if(!childNeedsTimer)
		mTimerFlags &= ~TimerFlag_ChildWants;

	return WantTimer();

/*	bool childNeedsTimer = false;
	ChildList::iterator anItr = mChildList.begin();
	while(anItr!=mChildList.end())
	{
		Component *aChild = *anItr;
		if(aChild->WantTimer())
		{
			if(aChild->TimerEvent(theDelta))
				childNeedsTimer = true;
		}

		++anItr;
	}

	if(!childNeedsTimer)
		mTimerFlags &= ~TimerFlag_ChildWants;

	return WantTimer();*/
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::BroadcastEventDownPrv(ComponentEvent* theEvent)
{
	Component::BroadcastEventDownPrv(theEvent);

	ChildList::iterator anItr;
	for(anItr = mChildList.begin(); anItr != mChildList.end(); anItr++)
	{
		Component *aComponent = *anItr;
		aComponent->BroadcastEventDownPrv(theEvent);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::SetCursor(Cursor *theCursor)
{
	if(mMouseChild!=NULL && mMouseChild->ComponentFlagSet(ComponentFlag_NoParentCursor))
	{
		Cursor *aDefaultCursor = GetDefaultCursor();
		if(mCurrentCursor.get()!=aDefaultCursor)
			mParent->SetCursor(aDefaultCursor);

		return;
	}

	if(theCursor==NULL)
		theCursor = GetDefaultCursor();

	if(theCursor!=mCurrentCursor)
	{
		mCurrentCursor = theCursor;
		if(mParent!=NULL)
			mParent->SetCursor(theCursor);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::GotFocus()
{
	Component::GotFocus();
	if(mFocusChild!=NULL)
		mFocusChild->GotFocus();

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::LostFocus()
{
	Component::LostFocus();
	if(mFocusChild!=NULL)
	{
		mFocusChild->LostFocus();
//		mFocusChild = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::ChildRequestFocus(Component *theChild)
{
	if(!IsVisible() || Disabled())
		return;

	if(mFocusChild==theChild && HasFocus())
		return;

	if(mModalCount>0 && theChild!=NULL && !theChild->ComponentFlagSet(ComponentFlag_AllowModalInput))
		return;

	if(mFocusChild!=NULL && HasFocus())
		mFocusChild->LostFocus();
	
	mFocusChild = theChild;
	if(theChild!=NULL)
	{
		theChild->SetFocusId(GetNextFocusId());
	
		if(!HasFocus() && mParent!=NULL)
			mParent->ChildRequestFocus(this);		
		else 
		{
			SetComponentFlags(ComponentFlag_HasFocus,true);

			if(mParent==NULL && GetRoot()==this && !GetWindow()->HasFocus())
				return;

			theChild->GotFocus();
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Container::ChildChanged(Component *theChild)
{
	bool isBad = theChild->Disabled() || !theChild->IsVisible();
	if(mFocusChild==theChild)
	{
		if(isBad)
		{
			mFocusChild->LostFocus();
			mFocusChild = NULL;
		}
	}
		
	Container *aRoot = GetRoot();
	if(aRoot==NULL) // don't need to check mouse states if we're not in a window
		return; 

	int x, y;
//	aRoot->GetMousePos(x,y);
	GetMousePos(x,y);
	if(theChild->Contains(x,y) || (mMouseChild==theChild && isBad))
		MouseAt(x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Container::TakeTabFocus(bool forward, bool init)
{
	if(!WantFocus())
		return false;

	if(init && mIsolateTabFocus)
		return false;

	if(init && ComponentFlagSet(ComponentFlag_WantTabFocus))
	{
		mHasUsedDialogKeys = true;
		RequestFocus();
		return true;
	}

	if(mChildList.empty())
		return false;

	bool wrap = mIsolateTabFocus;

	Component *aChild = init?NULL:mFocusChild;
	if(aChild==NULL)
		aChild = forward?mChildList.front():mChildList.back();
	else
		aChild = aChild->GetNextSibling(forward,wrap);

	int aCount = 0;
	while(aChild!=NULL)
	{
		if(aChild->TakeTabFocus(forward,true))
		{
			mHasUsedDialogKeys = true;
			return true;
		}
		else
			aChild = aChild->GetNextSibling(forward,wrap);

		aCount++;
		if(aCount>=mChildList.size())
			break;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Container::KeyDown(int theKey)
{
	if(mFocusChild!=NULL && mFocusChild->KeyDown(theKey))
		return true;

	if(theKey!=KEYCODE_TAB)
		return false;

	if(mChildList.empty())
		return false;

	bool forward = (GetKeyMod() & KEYMOD_SHIFT)==0;
	return TakeTabFocus(forward,false);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Container::KeyUp(int theKey)
{
	if(mFocusChild!=NULL)
	{
		mFocusChild->KeyUp(theKey);
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Container::KeyChar(int theKey)
{
	if(mFocusChild!=NULL)
	{
		return mFocusChild->KeyChar(theKey);
//		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Container::AllowWindowClose()
{
	ChildList::iterator anItr = mChildList.begin();
	while(anItr!=mChildList.end())
	{
		Component *aComponent = *anItr;
		if(!aComponent->AllowWindowClose())
			return false;

		++anItr;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Component* Container::GetComponentById(int theControlId)
{
	// perform breadth first search

	std::list<Component*> aList;
	aList.push_back(this);

	std::list<Component*>::iterator anItr = aList.begin();
	while(anItr!=aList.end())
	{
		Component *aComponent = *anItr;
		if(aComponent->GetControlId()==theControlId)
			return aComponent;
		else
		{
			Container *aContainer = dynamic_cast<Container*>(aComponent);
			if(aContainer!=NULL)
			{
				ChildList::iterator aChildItr = aContainer->mChildList.begin();
				while(aChildItr!=aContainer->mChildList.end())
				{
					aList.push_back(*aChildItr);
					++aChildItr;
				}
			}
		}

		++anItr;
	}

	/*
	ChildList::iterator aChildItr = mChildList.begin();
	while(aChildItr!=mChildList.end())
	{
		Component *aChild = *aChildItr;
		if(aChild->GetControlId()==theControlId)
			return aChild;
	
		++aChildItr;
	}*/

	return NULL;
}
