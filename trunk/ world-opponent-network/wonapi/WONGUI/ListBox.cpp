#include "ListBox.h"
#include "Window.h"
#include "WONCommon/StringUtil.h"
#include <algorithm>

using namespace WONAPI;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListItem::ListItem()
{
	my = mWidth = mHeight = 0;
	mSelected = false;
	mSelectable = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextListItem::CalcDimensions(ListArea *theListArea)
{
	if(mFont.get()==NULL)
		mFont = theListArea->GetFont();

	if(mColor==COLOR_INVALID)
		mColor = theListArea->GetTextColor();

	mWidth = mFont->GetStringWidth(mText) + 2;
	mHeight = mFont->GetHeight();

	if(mIcon.get()!=NULL)
	{
		mWidth += mIcon->GetWidth() + 2;
		if(mIcon->GetHeight() > mHeight)
			mHeight = mIcon->GetHeight();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TextListItem::Compare(const ListItem *theItem, ListArea *theListArea) const
{
	TextListItem *anItem = (TextListItem*)theItem;
	return mText.compareNoCase(anItem->mText);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextListItem::Paint(Graphics &g, ListArea *theListArea)
{
	int aColor = mColor;

	if(mSelected)
	{
		g.SetColor(theListArea->GetSelColor().GetBackgroundColor(g));
/*		int aWidth = theListArea->GetTotalWidth();
		if(aWidth < theListArea->Width())
			aWidth = theListArea->Width();*/
		int aWidth = theListArea->GetPaintColumnWidth();
		if(!theListArea->ListFlagSet(ListFlag_FullRowSelect))
			aWidth = mWidth;

		int x = 0;
		if(mIcon.get()!=NULL && !theListArea->ListFlagSet(ListFlag_HilightIcons))
		{
			x+=mIcon->GetWidth()+2;
			aWidth-=mIcon->GetWidth()+2;
		}

		g.FillRect(x,0,aWidth,mHeight);
		aColor = theListArea->GetSelColor().GetForegroundColor(g,mColor);
	}

	g.SetColor(aColor);
	g.SetFont(mFont);
	int x = 2;
	if(mIcon.get()!=NULL)
	{
		Font *aFM = mFont;
		int y = (mHeight - mIcon->GetHeight())/2;
		if(y < 0)
			y = 0;

		g.DrawImage(mIcon, x, y);
		x+=mIcon->GetWidth() + 2;
	}

	int y = (mHeight - mFont->GetHeight())/2;
	g.DrawString(mText, x, y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentListItem::CalcDimensions(ListArea *theListArea)
{
	if(mComponent.get()!=NULL)
	{
		mWidth = mComponent->Width();
		mHeight = mComponent->Height();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentListItem::SetViewport(ListArea *theListArea, Container *theViewport)
{
	mComponent->SetParent(theViewport);	
	if(theViewport!=NULL)
		mComponent->AddedToParent();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentListItem::SetPos(int x, int y)
{
	ListItem::SetPos(x,y);
	mComponent->SetPos(x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ComponentListItem::Contains(int x, int y)
{
//	if(!ListItem::Contains(x,y))
//		return false;

	return mComponent->Contains(x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentListItem::CheckInvalid(ListArea *theListArea)
{
//	if(mComponent->IsInvalid())
//		theListArea->Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentListItem::Paint(Graphics &g, ListArea *theListArea)
{
	if(mComponent.get()!=NULL)
	{
		if(mIsSizable)
			mComponent->SetSize(theListArea->GetPaintColumnWidth(), mComponent->Height());

		mComponent->InvalidateDown();
		mComponent->Paint(g);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ListItemClickedEvent::IsSelectedItem()
{
	return mItem.get()==((ListArea*)mComponent)->GetSelItem();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListArea::ListArea()
{
	mSearchLine = new ListItem;

	mTopLineVectorPos = 0;
	mListFlags = ListFlag_FullRowSelect | ListFlag_HilightIcons | ListFlag_ShowPartialLines;
	mSortType = ListSortType_None;

	mTextColor = ColorScheme::GetColorRef(StandardColor_Text);
	mFont = GetNamedFont("ListArea");

	mSelItem = NULL;
	mKeyItem = NULL;
	mMouseItem = NULL;

	SetComponentFlags(ComponentFlag_WantFocus, true);
	mTopPad = mLeftPad = mRightPad = mBottomPad = 2;
	mPaintColumnWidth = -1;

	mDoingMultiChange = 0;
	mSortFunc = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListArea::~ListArea()
{
	if(mItemToolTip.get()!=NULL)
		mItemToolTip->EndTip();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::BeginMultiChange()
{
	mDoingMultiChange++;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::EndMultiChange()
{
	mDoingMultiChange--;
	if(mDoingMultiChange<=0)
	{
		mDoingMultiChange = 0;
		if(!Sort())
			EnforceInvariants();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::Clear(bool fireEventOnChange)
{
	mTotalWidth = mTotalHeight = 0;
	mVertOffset = mHorzOffset = 0;
	ClearSelection(fireEventOnChange);
	mSelItem = NULL;
	mKeyItem = NULL;
	if(mMouseItem!=NULL)
		NotifyMouseItemGone();

	mLineVector.clear();

	AdjustPosition();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ListArea::NumberSortFunc(ListArea *theListArea, const ListItem *l1, const ListItem *l2)
{
	return ((TextListItem*)l1)->mText.atoi() - ((TextListItem*)l2)->mText.atoi();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListArea::SortFunc ListArea::GetSortFunc()
{
	if(mSortFunc==NULL)
		return DefaultSortFunc;
	else
		return mSortFunc;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ListArea::Sort()
{
	if(mSortType==ListSortType_None)
		return false;
	
	std::sort(mLineVector.begin(), mLineVector.end(), LineSortPred(this, GetSortFunc(), mSortType));
	EnforceInvariants();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ListArea::FindInsertPos(ListItem *theItem, int thePos)
{

	if(mDoingMultiChange || mSortType==ListSortType_None)
	{
		if(thePos<0 || thePos>mLineVector.size())
			thePos = mLineVector.size();

		return thePos;
	}

	LineVector::iterator anItr = std::lower_bound(mLineVector.begin(), mLineVector.end(), theItem, LineSortPred(this, GetSortFunc(), mSortType));
	return anItr-mLineVector.begin();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::SetSortType(ListSortType theType)
{
	if(mSortType==theType)
		return;

	mSortType = theType;
	Sort();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListArea::LineVector::iterator ListArea::FindLineAt(int theYPos)
{
	mSearchLine->my = theYPos;
	LineVector::iterator anItr = std::lower_bound(mLineVector.begin(), mLineVector.end(), mSearchLine, LineSearchPred());
	if(anItr==mLineVector.end() || (*anItr)->my>theYPos)
	{
		if(anItr!=mLineVector.begin())
			--anItr;
	}

	return anItr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListItem* ListArea::FindItemAt(int theYPos)
{
	LineVector::iterator anItr = FindLineAt(theYPos);
	if(anItr==mLineVector.end())
		return NULL;
	else
	{
		ListItem *anItem = *anItr;
		if(anItem->my + anItem->mHeight < theYPos)
			return NULL;
		else
			return anItem;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::FindTopLine()
{
	LineVector::iterator mTopLineItr = FindLineAt(mVertOffset);
	mTopLineVectorPos = mTopLineItr - mLineVector.begin();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::AdjustPosition(bool becauseOfScrollbar)
{
	mExtraHeight = 0;
	if(ListFlagSet(ListFlag_ScrollFullLine) && mTotalHeight>mScrollArea.height) 
	{
		// Need to calculate extra height in listbox in order to be able to see the last line 
		// while still seeing the entire top line
		LineVector::iterator anItr = FindLineAt(mTotalHeight-mScrollArea.height+1);
		if(anItr!=mLineVector.end())
		{
			ListItem *anItem = *anItr;
			mExtraHeight = anItem->my + anItem->mHeight + mScrollArea.height - mTotalHeight;
			if(mExtraHeight<0)
				mExtraHeight = 0;
		}
	}

	ScrollArea::AdjustPosition(becauseOfScrollbar);

	FindTopLine();

	if(ListFlagSet(ListFlag_ScrollFullLine))
	{
		if(mTopLineVectorPos<mLineVector.size())
		{
			ListItem *aLine = mLineVector[mTopLineVectorPos]; 
			mVertOffset = aLine->my;
			mDownScrollAmount = aLine->mHeight;
			if(mTopLineVectorPos>0)
			{
				aLine = mLineVector[mTopLineVectorPos-1];
				mUpScrollAmount = aLine->mHeight;
			}
			ScrollArea::AdjustPosition(true);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::SizeChanged()
{
	ScrollArea::SizeChanged();
	AdjustPosition();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ListArea::IsItemVisible(ListItem *theItem)
{
	if(theItem->my + theItem->mHeight < mVertOffset)
		return false;

	int aYPos = theItem->my;
	if(!ListFlagSet(ListFlag_ShowPartialLines))
		aYPos += theItem->mHeight;

	return aYPos <= mVertOffset+mScrollArea.height;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::EnsureVisible(ListItem *theItem)
{
	int aYPos = mVertOffset;
	if(theItem->my + theItem->mHeight > mVertOffset + mScrollArea.Height())
		aYPos = theItem->my + theItem->mHeight - mScrollArea.Height();

	if(aYPos > theItem->my)
		aYPos = theItem->my;

	if(aYPos!=mVertOffset)
		SetVertOffset(aYPos);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::ScrollPaint(Graphics &g)
{
	LineVector::iterator aLineItr = mLineVector.begin()+mTopLineVectorPos; //mTopLineItr;
	int aSelWidth = mTotalWidth;
	if(mScrollArea.width > aSelWidth)
		aSelWidth = mScrollArea.width;

	if(mPaintColumnWidth==-1)
		mPaintColumnWidth = aSelWidth;

	while(aLineItr!=mLineVector.end())
	{
		ListItem *aLine = *aLineItr;
		if(!IsItemVisible(aLine))
			break;

		g.Translate(0,aLine->my);
		aLine->Paint(g, this);
		g.Translate(0,-aLine->my);

		++aLineItr;
	}

	mPaintColumnWidth = -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::CheckMouseSel(ListItem *theItem, int x, int y, bool fromMouseDown)
{
	if(theItem==NULL)
	{
		if(ListFlagSet(ListFlag_SelOnlyWhenOver))
			ClearSelection();
	
		return;
	}

	// Don't allow mouse selection of clipped lines which can't be seen
	if(!IsItemVisible(theItem))
		return;

	// Unselectable items (i.e. separators) do not affect the selection
	if(!theItem->CheckMouseSel(this,x,y-theItem->my))
	{
		if(ListFlagSet(ListFlag_SelOnlyWhenOver))
			ClearSelection();

		return;
	}

	mKeyItem = theItem;
	bool add = false;
	if(fromMouseDown && ListFlagSet(ListFlag_MultiSelect))
	{
		int aKeyMod = GetWindow()->GetKeyMod();
		if(aKeyMod & KEYMOD_CTRL)
		{
			add = true;
			theItem->mSelected = !theItem->mSelected;
			Invalidate();
		}
		else if((aKeyMod&KEYMOD_SHIFT) && mSelItem!=NULL)
		{
			SelectRange(GetItemPos(mSelItem),GetItemPos(theItem));
			return;
		}
		else
		{
			// Clear the selection and check if this guy was the only one selected (then just leave it that way)
			bool noChange = mSelItem==theItem && ClearSelectionPrv()==1;
			theItem->mSelected = true;
			if(noChange)
				mSelItem = theItem;
		}
	}
	else if(ListFlagSet(ListFlag_SelOver) || !ListFlagSet(ListFlag_ToggleSel))
		theItem->mSelected = true;
	else
		theItem->mSelected = !theItem->mSelected;

	SetSelItem(theItem,add);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::ListSelChangedHook()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ListArea::SetSelItem(ListItem *theItem, bool add, bool fireEventOnChange)
{
	if(theItem==NULL)
		return ClearSelectionPrv(fireEventOnChange)>0;

	ListItem *anOldSel = mSelItem;
	if(theItem!=mSelItem)
	{
		if(mSelItem!=NULL && (!ListFlagSet(ListFlag_MultiSelect) || !add))
		{
			bool oldSelected = theItem->mSelected;
			ClearSelectionPrv();
			theItem->mSelected = oldSelected;
//			mSelItem->mSelected = false;
		}

		mSelItem = theItem;
	}

	if(mSelItem!=NULL && !mSelItem->mSelected)
		mSelItem = NULL;

	if(theItem!=anOldSel)
		Invalidate();

	if(mSelItem!=anOldSel)
	{
		Invalidate();
		if(mFocusChild!=NULL)
		{
			mFocusChild->LostFocus();
			mFocusChild = NULL;
		}

		if(fireEventOnChange)
			FireEvent(ComponentEvent_ListSelChanged);

		ListSelChangedHook();
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::SelectRange(int theStartIndex, int theEndIndex)
{
	ClearSelectionPrv(false);

	if(theStartIndex >= GetNumItems()) theStartIndex = GetNumItems()-1;
	if(theEndIndex >= GetNumItems()) theEndIndex = GetNumItems()-1;

	if(theStartIndex < 0) theStartIndex = 0;
	if(theEndIndex < 0) theEndIndex = 0;

	if(theStartIndex >= GetNumItems() || theEndIndex >= GetNumItems())
		return;

	int i,dx;
	dx = theStartIndex<=theEndIndex?1:-1;

	for(i = theStartIndex; i!=theEndIndex+dx; i+=dx)
		GetItem(i)->mSelected = true;

	SetSelItem(GetItem(theStartIndex));

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::AddSelItem(int theIndex, bool fireEventOnChange)
{
	if (theIndex<0 || theIndex>=GetNumItems())
		return;

	ListItem *anItem = mLineVector[theIndex];
	anItem->mSelected = true;
	mKeyItem = anItem;
	SetSelItem(anItem, true, fireEventOnChange);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::SetSelItem(int theIndex, bool fireEventOnChange)
{
	if(theIndex<0 || theIndex>=GetNumItems())
	{
		SetSelItem((ListItem*)NULL,false,fireEventOnChange);
		mKeyItem = NULL;
	}
	else
	{
		ListItem *anItem = mLineVector[theIndex];
		anItem->mSelected = true;
		mKeyItem = anItem;
		SetSelItem(anItem,false,fireEventOnChange);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::RewindSelections()
{
	mSelItemItr = -1;
	FindNextSelection();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ListArea::HasMoreSelections()
{
	return mSelItemItr < mLineVector.size();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::FindNextSelection()
{
	mSelItemItr++;
	while(mSelItemItr<mLineVector.size() && !mLineVector[mSelItemItr]->mSelected)
		mSelItemItr++;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ListArea::GetTextColor()
{
	return mTextColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListItem* ListArea::GetNextSelection()
{
	if(HasMoreSelections())
	{
		ListItem *anItem = mLineVector[mSelItemItr];
		FindNextSelection();
		return anItem;
	}
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ListArea::ClearSelectionPrv(bool fireEventOnChange)
{
	if(mSelItem==NULL)
		return 0;

	int aCount = 0;
	if(ListFlagSet(ListFlag_MultiSelect))
	{
		LineVector::iterator anItr = mLineVector.begin();
		while(anItr!=mLineVector.end())
		{
			if((*anItr)->mSelected)
			{
				(*anItr)->mSelected = false;
				aCount++;
			}
			++anItr;
		}
	}
	else
	{
		aCount = 1;
		mSelItem->mSelected = false;
	}

	mSelItem = NULL;
	Invalidate();
	if(fireEventOnChange)
	{
		FireEvent(ComponentEvent_ListSelChanged);
		ListSelChangedHook();
	}

	return aCount;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::ClearSelection(bool fireEventOnChange)
{
	ClearSelectionPrv(fireEventOnChange);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::SetListFlags(int theFlags, bool on)
{
	bool selOnlyWhenOver = ListFlagSet(ListFlag_SelOnlyWhenOver);
	if(on)
		mListFlags |= theFlags;
	else
		mListFlags &= ~theFlags;

	// Check if SelOver turned on
	if(!selOnlyWhenOver && ListFlagSet(ListFlag_SelOnlyWhenOver))
	{
		Window *aWindow = GetWindow();
		if(aWindow==NULL)
			return;

		int x = aWindow->GetMouseX();
		int y = aWindow->GetMouseY();
		Container *aParent = GetParent();
		if(aParent!=NULL)
		{
			aParent->RootToChild(x,y);
			if(!Contains(x,y))
				ClearSelection();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool CheckContains(ListItem *theItem, int x, int y, bool itemWidthCheckOnSel)
{
	if(!theItem->Contains(x,y))
		return false;

	if(itemWidthCheckOnSel && x>theItem->mWidth)
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::ItemMouseAt(int &x, int &y)
{
	ScrollTranslate(x,y);
	bool itemWidthCheckOnSel = ListFlagSet(ListFlag_ItemWidthCheckOnSel);
	if(mMouseItem!=NULL && !CheckContains(mMouseItem,x,y,itemWidthCheckOnSel))
	{
		mMouseItem->MouseExit(this);
		SetCursor(GetDefaultCursor());
		NotifyMouseItemGone();
	}

	if(mMouseItem==NULL)
	{
		mMouseItem = FindItemAt(y);
		if(mMouseItem!=NULL)
		{
			if(CheckContains(mMouseItem,x,y,itemWidthCheckOnSel))
				mMouseItem->MouseEnter(this,x,y-mMouseItem->my);
			else
				mMouseItem = NULL;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListAreaToolTip::ListAreaToolTip(ListArea *theListArea)
{
	mListArea = theListArea;
	mItem = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListAreaToolTip::Paint(Graphics &g)
{
	Component::Paint(g);
	if(mItem==NULL)
		return;

	ColorScheme *cs = g.GetColorScheme();
	if(GetOwnColorScheme()==NULL)
		g.SetColorScheme(mListArea->GetColorScheme());

	int x = mListArea->GetLeftPad(), y = -mItem->my + 2;	

	g.ApplyColorSchemeColor(StandardColor_ToolTipBack);
	g.FillRect(0,0,Width(),Height());
//	g.PushClipRect(0,0,Width(),Height());
//	mListArea->GetBackground().Paint(g,0,0,mListArea->Width(),mListArea->Height(),0,mItem->my - mListArea->GetTopPad());
//	g.PopClipRect();

	g.PushClipRect(0,2,Width(),Height()-4);

	g.Translate(x,y);

	mListArea->ScrollPaint(g);

	g.Translate(-x,-y);

	g.PopClipRect();
	g.ApplyColorSchemeColor(StandardColor_ToolTipText);
	g.DrawRect(0,0,Width(),Height());

	g.SetColorScheme(cs);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListAreaToolTip::DoTip(ListItem *theItem)
{
	if(mItem.get()==theItem)
	{
		if(mItem.get()!=NULL && GetParent()!=NULL) // already doing the tip
			return;
	}

	EndTip();
	mItem = theItem;
	if(mItem.get()==NULL)
		return;

	int x = 0,y = mListArea->GetTopPad() - mListArea->GetVertOffset();
	mListArea->ChildToRoot(x,y);
	y += mItem->my - 2;
	PopupParams aParams(this,PopupFlag_StandardTip,mListArea,x,y);
	int aWidth = mItem->mWidth + mListArea->GetLeftPad() + mListArea->GetRightPad();
	if(aWidth < mListArea->Width())
		aWidth = mListArea->Width();

	SetSize(aWidth,mItem->mHeight + 4);
	mListArea->GetWindow()->DoPopup(aParams);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListAreaToolTip::EndTip()
{
	if(GetParent()==NULL)
		return;

	mListArea->GetWindow()->EndPopup(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::MouseMove(int x, int y)
{
	ScrollArea::MouseMove(x,y);
	ItemMouseAt(x,y);

	if(ListFlagSet(ListFlag_SelOver))
		CheckMouseSel(mMouseItem,x,y);

	if(mMouseItem!=NULL)
		mMouseItem->MouseMove(this,x,y-mMouseItem->my);

	if(ListFlagSet(ListFlag_DoListItemToolTip))
	{
		bool showItem = false;
		if(mMouseItem!=NULL && mMouseItem->mWidth > mScrollArea.width)
		{
			int halfHeight = mMouseItem->mHeight>>1;
			if(mMouseItem->my+halfHeight>mVertOffset && mMouseItem->my+halfHeight<mVertOffset+mScrollArea.height)
				showItem = true;
		}
			
		if(showItem)
		{
			if(mItemToolTip.get()==NULL)
				mItemToolTip = new ListAreaToolTip(this);

			mItemToolTip->DoTip(mMouseItem);
		}
		else if(mItemToolTip.get()!=NULL)
			mItemToolTip->DoTip(NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::MouseDrag(int x, int y)
{
	ScrollArea::MouseDrag(x,y);


	if(ListFlagSet(ListFlag_SelOver))
	{
		ItemMouseAt(x,y);
		CheckMouseSel(mMouseItem,x,y);
	}
	else
		ScrollTranslate(x,y);

	if(mMouseItem!=NULL)
		mMouseItem->MouseDrag(this,x,y-mMouseItem->my);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::MouseDown(int x, int y, MouseButton theButton)
{
	ScrollArea::MouseDown(x,y,theButton);

	ItemMouseAt(x,y);
	if(theButton!=MouseButton_Middle)
	{
		RequestFocus();
		CheckMouseSel(mMouseItem,x,y,true);
	}

	if(mMouseItem!=NULL)
	{
		mMouseItem->MouseDown(this,x,y-mMouseItem->my,theButton);
		FireEvent(new ListItemClickedEvent(this,mMouseItem,theButton,CheckDoubleClick(theButton),false,x,y-mMouseItem->my));
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::MouseUp(int x, int y, MouseButton theButton)
{
	ScrollArea::MouseUp(x,y,theButton);
	ItemMouseAt(x,y);
	if(mMouseItem!=NULL)
	{
		mMouseItem->MouseUp(this,x,y-mMouseItem->my,theButton);
		FireEvent(new ListItemClickedEvent(this,mMouseItem,theButton,CheckDoubleClick(theButton),true,x,y-mMouseItem->my));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::MouseEnter(int x, int y)
{
	ScrollArea::MouseEnter(x,y);
	ItemMouseAt(x,y);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::MouseExit()
{
	ScrollArea::MouseExit();
	if(mMouseItem!=NULL)
	{
		mMouseItem->MouseExit(this);
		NotifyMouseItemGone();
	}

	if(ListFlagSet(ListFlag_SelOnlyWhenOver))
		ClearSelection();

	if(ListFlagSet(ListFlag_DoListItemToolTip))
	{
		if(mItemToolTip.get()!=NULL)
			mItemToolTip->DoTip(NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListItem* ListArea::FindSelectableItem(int theStartPos, bool up, bool tryOpposite)
{
	int dx = up?-1:1;
	
	int aPos = theStartPos;
	ListItem *anItem = GetItem(aPos);
	while(anItem!=NULL)
	{
		if(anItem->mSelectable)
			return anItem;

		aPos+=dx;
		anItem = GetItem(aPos);
	}

	if(ListFlagSet(ListFlag_KeyMoveWrapAround))
	{
		if(up)
			return FindSelectableItem(GetNumItems()-1,!up,false);
		else
			return FindSelectableItem(0,!up,false);
	}


	if(tryOpposite)
		return FindSelectableItem(theStartPos,!up,false);
	else
		return NULL;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ListArea::KeyDown(int theKey)
{
	if(ScrollArea::KeyDown(theKey))
		return true;

	if(mKeyItem==NULL)
	{
		if(theKey==KEYCODE_DOWN || theKey==KEYCODE_UP || theKey==KEYCODE_PGDN || theKey==KEYCODE_PGUP)
		{
			ListItem *anItem = FindItemAt(mVertOffset);
			if(anItem==NULL)
				return false;

			int aPos = GetItemPos(anItem);
			anItem = FindSelectableItem(aPos,theKey==KEYCODE_UP || theKey==KEYCODE_PGUP);
			if(anItem!=NULL)
			{
				mKeyItem = anItem;
				anItem->mSelected = true;
				SetSelItem(anItem);
			}

			return true;
		}
		return false;
	}


	int y = mKeyItem->my;
	int height = mKeyItem->mHeight+1;
	int anItemPos = -1;

	switch(theKey)
	{
		case KEYCODE_RIGHT: mHorzOffset+=10; break;
		case KEYCODE_LEFT: mHorzOffset-=10; break;
		case KEYCODE_DOWN: anItemPos = y+height; break;
		case KEYCODE_UP: anItemPos = y-1; break;
		case KEYCODE_PGDN: anItemPos = y+mScrollArea.height; break;
		case KEYCODE_PGUP: anItemPos = y+height-mScrollArea.height; break;
		default: return false;
	}

	bool up = anItemPos<y;
	if(ListFlagSet(ListFlag_KeyMoveWrapAround))
	{
		if(anItemPos<0)
		{
			anItemPos = mTotalHeight;
			up = true;
		}
		else if(anItemPos>=mTotalHeight)
		{
			anItemPos = 0;
			up = false;
		}
	}

	if(anItemPos!=-1)
	{
		LineVector::iterator anItr = FindLineAt(anItemPos);
		if(anItr==mLineVector.end())
			return false;

		ListItem *anItem = *anItr;
		int aPos = GetItemPos(anItem);
		anItem = FindSelectableItem(aPos,up);

		if(anItem!=NULL)
		{
			if(mSelItem!=NULL && ListFlagSet(ListFlag_MultiSelect) && GetWindow()->GetKeyMod()&KEYMOD_SHIFT)
			{
				SelectRange(GetItemPos(mSelItem),GetItemPos(anItem));
			}
			else
			{
				anItem->mSelected = true;
				SetSelItem(anItem);
			}

			mKeyItem = anItem;
			if(anItem->my+anItem->mHeight > mVertOffset+mScrollArea.height)
				mVertOffset = anItem->my+anItem->mHeight-mScrollArea.height + mExtraHeight;
			if(anItem->my < mVertOffset)
				mVertOffset = anItem->my;
		}
	}

	AdjustPosition();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::EnforceInvariants()
{
	if(mDoingMultiChange)
		return;

	LineVector::iterator anItr = mLineVector.begin();
	mTotalHeight = mTotalWidth = 0;
	ListItem *anItem;
	while(anItr!=mLineVector.end())
	{
		anItem = *anItr;
	
		anItem->SetPos(0,mTotalHeight);
		
		mTotalHeight += anItem->mHeight;
		if(anItem->mWidth > mTotalWidth)
			mTotalWidth = anItem->mWidth;

		++anItr;
	}

	AdjustPosition();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ListArea::GetItemPos(ListItem *theItem)
{
	if(theItem==NULL)
		return -1;

	LineVector::iterator anItr = FindLineAt(theItem->my);
	while(anItr!=mLineVector.end())
	{
		ListItem *anItem = *anItr;
		if(anItem==theItem)
			return anItr-mLineVector.begin();
		else if(anItem->my>theItem->my)
			break;

		++anItr;
	}

	return -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ListArea::GetSelItemPos()
{	
	return GetItemPos(mSelItem);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListItem* ListArea::GetItemAtYPos(int theYPos)
{
	LineVector::iterator anItr = FindLineAt(theYPos);
	if(anItr==mLineVector.end())
		return NULL;
	else
		return *anItr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ListArea::GetItemPosAtYPos(int theYPos)
{
	LineVector::iterator anItr = FindLineAt(theYPos);
	return anItr - mLineVector.begin();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListItem* ListArea::GetItem(int thePos)
{
	if(thePos<0 || thePos>=mLineVector.size())
		return NULL;

	return mLineVector[thePos];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListItem* ListArea::InsertItem(ListItem *theItem, int thePos)
{
	thePos = FindInsertPos(theItem, thePos);

	theItem->SetViewport(this,GetViewport());
	
	theItem->CalcDimensions(this);
	if(theItem->mHeight<=0)
		theItem->mHeight = 1;

	theItem->mSelected = false;
	bool onEnd = thePos == mLineVector.size();
	mLineVector.insert(mLineVector.begin() + thePos, theItem);

	if(mSelItemItr>=thePos)
		mSelItemItr++;

	if(onEnd)
	{
		theItem->SetPos(0,mTotalHeight);
		mTotalHeight += theItem->mHeight;
		if(theItem->mWidth > mTotalWidth)
			mTotalWidth = theItem->mWidth;

		if(!ListFlagSet(ListFlag_NoAutoAdjustScrollPos) && OnBottom()) // make sure list stays on bottom if it's currently on the bottom
			mVertOffset+=theItem->mHeight;

		AdjustPosition();
	}
	else
	{
		// Keep list from jerking around when items are inserted above
		if(!ListFlagSet(ListFlag_NoAutoAdjustScrollPos) && (mTopLineVectorPos >= thePos || OnBottom()) && mTotalHeight>mScrollArea.Height())
			mVertOffset+=theItem->mHeight;

		if(mDoingMultiChange)
		{
			int aPrevY, aNextY;
			if(thePos==0)
				aPrevY = 0;
			else
				aPrevY = mLineVector[thePos-1]->my;

			aNextY = mLineVector[thePos+1]->my;

			int y = aPrevY+1;
			if(y>aNextY)
				y = aNextY;

			theItem->my = y; // useful for calls to GetItemPos while doing multi change
		}
		else
			EnforceInvariants();
	}

	return theItem;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::UpdateItem(ListItem *theItem)
{
	int anOldHeight = theItem->mHeight;
	theItem->SetViewport(this,GetViewport());
	theItem->CalcDimensions(this);
	if(theItem->mHeight<=0)
		theItem->mHeight = 1;

	if(!mDoingMultiChange)
	{
		int anItemPos = GetItemPos(theItem);
		if(anItemPos>=0 && mSortType!=ListSortType_None) // Maintain sort order
		{
			LineSortPred aLessPred(this,GetSortFunc(),mSortType);
			ListItem *aPrevItem = GetItem(anItemPos-1);
			ListItem *aNextItem = GetItem(anItemPos+1);
			bool needSort = false;

			if(aPrevItem!=NULL && aLessPred(theItem,aPrevItem))
				needSort = true;
			else if(aNextItem!=NULL && aLessPred(aNextItem,theItem))
				needSort = true;

			if(needSort)
			{
				// Put correctly sorted item in place of this item so FindInsertPos works correctly
				ListItemPtr anItemRef = theItem; // make sure item is still ref counted
				if(aPrevItem!=NULL)
					mLineVector[anItemPos] = aPrevItem;
				else
					mLineVector[anItemPos] = aNextItem;

				int anInsertPos = FindInsertPos(theItem,0);
				if(anItemPos < anInsertPos)
					anInsertPos--;

				if(anInsertPos>=mLineVector.size())
					anInsertPos = mLineVector.size()-1;
				if(anInsertPos<0)
					anInsertPos = 0;

				if(anItemPos > anInsertPos)
				{
					for(int i=anItemPos; i>anInsertPos; i--)
						mLineVector[i] = mLineVector[i-1];
				}
				else
				{
					for(int i=anItemPos; i<anInsertPos; i++)
						mLineVector[i] = mLineVector[i+1];
				}

				mLineVector[anInsertPos] = theItem;
			}
		}


		if(anItemPos>=0)
		{
			// Keep list from jerking around when items are replaced above
			if(!ListFlagSet(ListFlag_NoAutoAdjustScrollPos) && (mVertOffset >= theItem->my || OnBottom()) && mTotalHeight>mScrollArea.Height())
				mVertOffset += (theItem->mHeight - anOldHeight);

			EnforceInvariants();
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::DeleteItemPrv(int thePos)
{
	LineVector::iterator anItr = mLineVector.begin() + thePos;
	ListItem *anItem = *anItr;

	anItem->SetViewport(this,NULL);

	// Keep list from jerking around when items are deleted from above
	if(!ListFlagSet(ListFlag_NoAutoAdjustScrollPos) && mVertOffset >= anItem->my)
		mVertOffset-=anItem->mHeight;

	if(anItem==mSelItem)
	{
		anItem->mSelected = false;
//		mSelItem = NULL;
		SetSelItem(-1);
	}

	if(anItem==mKeyItem)
		mKeyItem = NULL;


	if(anItem==mMouseItem)
		NotifyMouseItemGone();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::NotifyMouseItemGone()
{
	mMouseItem = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::DeleteItem(int thePos)
{
	DeleteRange(thePos, thePos+1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::DeleteItem(ListItem *theItem)
{
	int aPos = GetItemPos(theItem);
	if(aPos>=0)
		DeleteItem(aPos);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ListArea::DeleteRange(int theStartPos, int theEndPos)
{
	if(theEndPos<0 || theEndPos>mLineVector.size())
		theEndPos = mLineVector.size();

	if(theStartPos<0 || theEndPos<=theStartPos)
		return;

	int theAmount = theEndPos - theStartPos;

	if(theStartPos + theAmount > mLineVector.size())
		theAmount = mLineVector.size() - theStartPos;

	for(int i = 0; i<theAmount; i++)
		DeleteItemPrv(theStartPos + i);

	if(mSelItemItr>=theStartPos)
		mSelItemItr-=theAmount;

	LineVector::iterator anItr = mLineVector.begin() + theStartPos;
	mLineVector.erase(anItr, anItr+theAmount);
	EnforceInvariants();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListItem* ListArea::InsertString(const GUIString &theStr, int thePos)
{
	return InsertItem(new TextListItem(theStr), thePos);
}
	


