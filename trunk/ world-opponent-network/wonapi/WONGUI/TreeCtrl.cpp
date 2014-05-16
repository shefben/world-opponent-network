#include "TreeCtrl.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TreeItem::TreeItem()
{
	mx = 0;
	mParent = NULL;
	mPos = -1;
	mIsOpen = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::Sort(TreeArea *theTreeArea, bool recursive)
{
	if(mChildren.empty())
		return;

	std::sort(mChildren.begin(), mChildren.end(), ListArea::LineSortPred(theTreeArea, theTreeArea->GetSortFunc(), theTreeArea->mTreeSortType));

	ItemVector::iterator anItr = mChildren.begin();
	int i=0;
	while(anItr!=mChildren.end())
	{
		TreeItem *anItem = *anItr;
		anItem->mPos = i++;
	
		if(recursive)
			anItem->Sort(theTreeArea,recursive);
		
		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListItem* TreeItem::GetMouseItem(ListArea *theListArea, int &x, int &y)
{
	TreeArea *aTree = (TreeArea*)theListArea;
	ListItem *aNewMouseItem = mItem;
	ListItem *aMouseColItem = aTree->mMouseColItem;
	if(aMouseColItem!=NULL && (aMouseColItem!=aNewMouseItem || !aMouseColItem->Contains(x,y+my)))
	{
		aMouseColItem->MouseExit(theListArea);
		theListArea->SetCursor(NULL);

		aTree->mMouseColItem = NULL;
	}


	if(aMouseColItem==NULL)
	{
		if(aNewMouseItem!=NULL && aNewMouseItem->Contains(x,y+my))
		{
			aTree->mMouseColItem = aNewMouseItem;
			aNewMouseItem->MouseEnter(theListArea,x,y);
		}
	}

	x-=mx;
	return aTree->mMouseColItem;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::MouseEnter(ListArea *theListArea, int x, int y)
{
	GetMouseItem(theListArea,x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::MouseExit(ListArea *theListArea)
{
	ListItem *anItem = ((TreeArea*)theListArea)->mMouseColItem;
	if(anItem!=NULL)
	{
		anItem->MouseExit(theListArea);
		((TreeArea*)theListArea)->mMouseItem = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::MouseMove(ListArea *theListArea, int x, int y)
{
	ListItem *anItem = GetMouseItem(theListArea,x,y);
	if(anItem!=NULL)
		anItem->MouseMove(theListArea,x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::MouseDrag(ListArea *theListArea, int x, int y)
{
	TreeArea *aList = (TreeArea*)theListArea;
	ListItem *anItem = aList->mMouseColItem;
	if(anItem!=NULL)
		anItem->MouseDrag(theListArea,x-mx,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::MouseUp(ListArea *theListArea, int x, int y, MouseButton theButton)
{	
	ListItem *anItem = GetMouseItem(theListArea,x,y);
	if(anItem!=NULL)
		anItem->MouseUp(theListArea,x,y,theButton);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::MouseDown(ListArea *theListArea, int x, int y, MouseButton theButton)
{
	if(theButton==MouseButton_Left)
	{
		TreeArea *aTreeArea = (TreeArea*)theListArea;
		if((x < mx && x > mx-14) || (theListArea->CheckDoubleClick(theButton) && aTreeArea->TreeFlagSet(TreeFlag_DoubleClickOpen)))
		{
			ToggleOpen(aTreeArea, aTreeArea->TreeFlagSet(TreeFlag_CloseAll));
			return;
		}
	}

	ListItem *anItem = GetMouseItem(theListArea,x,y);
	if(anItem!=NULL)
		anItem->MouseDown(theListArea,x,y,theButton);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TreeItem* TreeItem::GetNextSibling()
{
	if(mParent==NULL || mPos>=mParent->mChildren.size()-1)
		return NULL;
	else
		return mParent->mChildren[mPos+1];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TreeItem* TreeItem::GetChild(int thePos)
{
	if(thePos<0 || thePos>=mChildren.size())
		return NULL;

	return mChildren[thePos];
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TreeItem* TreeItem::GetNextNonChild()
{
	if(mParent==NULL)
		return NULL;
	else if(mPos>=mParent->mChildren.size()-1)
		return mParent->GetNextNonChild();
	else
		return mParent->mChildren[mPos+1];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TreeItem::GetNextNonChildPos(TreeArea *theTreeArea)
{
	TreeItem *anItem = GetNextNonChild();
	if(anItem!=NULL)
		return theTreeArea->GetItemPos(anItem);
	else
		return -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TreeItem::GetInsertPos(TreeArea *theTreeArea, TreeItem *theItem, int thePos)
{
	if(theTreeArea->mTreeSortType==ListSortType_None)
		return thePos;

	ItemVector::iterator anItr = std::lower_bound(mChildren.begin(), mChildren.end(), theItem, ListArea::LineSortPred(theTreeArea, theTreeArea->GetSortFunc(), theTreeArea->mTreeSortType));
	return anItr - mChildren.begin();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::InsertChild(TreeArea *theTreeArea, TreeItem *theItem, int thePos)
{
	thePos = GetInsertPos(theTreeArea,theItem,thePos);
	if(mParent==NULL && !theTreeArea->TreeFlagSet(TreeFlag_DrawButtons | TreeFlag_DrawLines))
		theItem->mx = 0;
	else
		theItem->mx = mx + 15;

	if(thePos<0 || thePos>mChildren.size())
		thePos = mChildren.size();

	bool onEnd = thePos==mChildren.size();

	mChildren.insert(mChildren.begin() + thePos, theItem);
	theItem->mParent = this;

	for(int i=thePos; i<mChildren.size(); i++)
	{
		TreeItem *anItem = mChildren[i];
		anItem->mPos = i;
	}

	if(IsOpen())
	{	
		int aPos;
		if(onEnd)
			aPos = GetNextNonChildPos(theTreeArea);
		else
			aPos = theTreeArea->GetItemPos(mChildren[thePos+1]);

		theTreeArea->ListArea::InsertItem(theItem, aPos);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::RemoveAllChildren(TreeArea *theTreeArea)
{
	SetOpen(theTreeArea,false,false,false);
	mChildren.clear();
	theTreeArea->Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::RemoveChild(TreeArea *theTreeArea, TreeItem *theItem)
{
	theTreeArea->BeginMultiChange();

	if(theItem->IsOpen())
		theItem->SetOpen(theTreeArea, false,false,false);

	if(IsOpen())
		theTreeArea->ListArea::DeleteItem(theItem);

	mChildren.erase(mChildren.begin() + theItem->mPos);
	for(int i=0; i<mChildren.size(); i++)
		mChildren[i]->mPos = i;

	if(mChildren.empty())
		theTreeArea->Invalidate(); // need to do this in order to redraw the button (don't need button anymore)

	theTreeArea->EndMultiChange();

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::RemoveFromParent(TreeArea *theTreeArea)
{
	if(mParent==NULL)
		return;

	mParent->RemoveChild(theTreeArea, this);	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::CloseAll()
{
	mIsOpen = false;
	ItemVector::iterator anItr = mChildren.begin();
	while(anItr!=mChildren.end())
	{
		(*anItr)->CloseAll();
		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::InsertChildren(TreeArea *theTreeArea, int thePos)
{
	for(int i=mChildren.size()-1; i>=0; i--)
	{
		TreeItem *anItem = mChildren[i];
		theTreeArea->ListArea::InsertItem(anItem,thePos);
		if(anItem->IsOpen())
			anItem->InsertChildren(theTreeArea, thePos+1);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::SetOpen(TreeArea *theTreeArea, bool isOpen, bool closeAll, bool sendEvent)
{
	if(mIsOpen==isOpen)
		return;

	mIsOpen = isOpen;

	if(mChildren.empty())
		return;

	if(!isOpen)
	{
		int aPos1 = theTreeArea->GetItemPos(mChildren.front());
		int aPos2 = GetNextNonChildPos(theTreeArea);
		theTreeArea->DeleteRange(aPos1, aPos2);
		if(closeAll)
			CloseAll();
	}
	else
	{
		theTreeArea->BeginMultiChange();
		InsertChildren(theTreeArea, theTreeArea->GetItemPos(this)+1);
		theTreeArea->EndMultiChange();

		// Figure out whether we need to scroll the TreeArea in order to make
		// the newly opened children visible
		int aSiblingYPos = theTreeArea->GetTotalHeight();
		TreeItem *anItem = GetNextNonChild();
		if(anItem!=NULL)
			aSiblingYPos = anItem->my;

		int aVertOffset = theTreeArea->GetVertOffset();
		if(aSiblingYPos > aVertOffset + theTreeArea->GetScrollArea().Height())
		{
			aVertOffset = aSiblingYPos - theTreeArea->GetScrollArea().Height();
			if(my < aVertOffset)
				aVertOffset = my;

			theTreeArea->SetVertOffset(aVertOffset);
		}		
	}

	if(sendEvent)
		theTreeArea->FireEvent(new TreeItemOpenedEvent(theTreeArea,this));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::ToggleOpen(TreeArea *theTreeArea, bool closeAll, bool sendEvent)
{
	SetOpen(theTreeArea, !mIsOpen, closeAll, sendEvent);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void DrawHorzDottedLine(Graphics &g, int x, int y, int length)
{
	DWORD aColor = g.GetColor();
	int i=0;
	if(x&1)
		i++;

	for(; i<length; i+=2)
		g.SetPixel(x+i,y,aColor);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void DrawVertDottedLine(Graphics &g, int x, int y, int length)
{
	DWORD aColor = g.GetColor();
	int i=0;
	if(y&1)
		i++;

	for(; i<length; i+=2)
		g.SetPixel(x,y+i,aColor);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TreeItem::IsLastChild()
{
	return mParent!=NULL && mPos>=mParent->mChildren.size()-1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::SetPos(int x, int y)
{
	ListItem::SetPos(x,y);
	mItem->SetPos(mx,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::Paint(Graphics &g, ListArea *theListArea)
{
	TreeArea *aTreeArea = (TreeArea*)theListArea;

	bool drawButton = aTreeArea->TreeFlagSet(TreeFlag_DrawButtons) && !mChildren.empty();

	int squareLeft		= mx - 14;
	int squareRight		= squareLeft+8;
	int squareTop		= (mHeight - 8)/2;
	int squareBottom	= squareTop+8;

	int lineTop = squareTop;
	int lineBottom = squareBottom;
	int lineLeft = squareRight;
	if(!drawButton)
	{
		lineTop = lineBottom = squareTop+4;
		lineLeft = squareLeft+6;
	}

	if(((TreeArea*)theListArea)->TreeFlagSet(TreeFlag_DrawLines))
	{
		g.ApplyColorSchemeColor(StandardColor_3DShadow);
		if(my!=0)
			DrawVertDottedLine(g,squareLeft+4,0,lineTop+1);

		if(!IsLastChild())
			DrawVertDottedLine(g,squareLeft+4,lineBottom,mHeight-lineBottom);

		DrawHorzDottedLine(g,lineLeft,squareTop+4,mx-lineLeft);
		TreeItem *anItem = mParent;
		while(anItem!=NULL && anItem->mx > 0)
		{
			if(!anItem->IsLastChild())
				DrawVertDottedLine(g,anItem->mx-10,0,mHeight);
	//		else
	//			DrawVertDottedLine(g,anItem->mx-10,0,lineBottom);

			anItem = anItem->mParent;
		}
	}

	if(drawButton)
	{
		g.ApplyColorSchemeColor(StandardColor_3DShadow);
		g.DrawRect(squareLeft,squareTop,9,9);
		g.ApplyColorSchemeColor(StandardColor_Text);
		g.DrawLine(squareLeft+2, squareTop+4, squareRight-2, squareTop+4);
		if(!mIsOpen)
			g.DrawLine(squareLeft+4, squareTop+2, squareLeft+4, squareBottom-2);
	}

	g.Translate(mx,0);//my);
	mItem->mSelected = mSelected;
	mItem->Paint(g, theListArea);
	g.Translate(-mx,0);//-my);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::SetViewport(ListArea *theListArea, Container *theViewport)
{
	mItem->SetViewport(theListArea, theViewport);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeItem::CalcDimensions(ListArea *theListArea)
{
	mItem->CalcDimensions(theListArea);
	mWidth = mx+mItem->mWidth;
	mHeight = mItem->mHeight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TreeItem::Compare(const ListItem *theItem, ListArea *theListArea) const
{
	return mItem->Compare(((TreeItem*)theItem)->mItem, theListArea);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TreeItem::CheckMouseSel(ListArea *theListArea, int x, int y)
{
	return mSelectable && x>=mx;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TreeItem::IsInItem(int x, int y)
{
	if(x<mx)
		return false;

	return mItem->Contains(x-mx,y+my);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TreeArea::TreeArea()
{
	mRoot = new TreeItem;
	mRoot->SetOpen(this,true,false,false);
	SetListFlags(ListFlag_FullRowSelect | ListFlag_HilightIcons, false); 
	SetListFlags(ListFlag_ItemWidthCheckOnSel, true);

	mTreeFlags = TreeFlag_DrawLines | TreeFlag_DrawButtons;
	mMouseColItem = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeArea::SetSortType(ListSortType theType)
{
	if(mTreeSortType==theType)
		return;

	mTreeSortType = theType;
	Sort();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeArea::Sort(TreeItem *theItem, bool recursive)
{
	if(mTreeSortType==ListSortType_None)
		return;

	if(theItem==NULL)
	{
		// Do Root
		ListArea::Clear(false);
		mRoot->Sort(this,recursive);
		mRoot->InsertChildren(this,0);
		return;
	}
		
	bool isVisible = theItem->IsOpen() && GetItemPos(theItem)>=0;

	// if the item open and visible then remove the children, sort them and then reinsert them
	if(isVisible)
		theItem->SetOpen(this,false,false,false);

	theItem->Sort(this, recursive);

	if(isVisible)
		theItem->SetOpen(this,true,false,false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TreeArea::Sort()
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeArea::NotifyMouseItemGone()
{
	ListArea::NotifyMouseItemGone();
	mMouseColItem = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TreeItem* TreeArea::InsertItem(TreeItem *theParent, ListItem *theItem, int thePos)
{
	if(theParent==NULL)
		theParent = mRoot;

	TreeItemPtr anItem = new TreeItem;
	anItem->mItem = theItem;
	anItem->mSelectable = theItem->mSelectable;
	theParent->InsertChild(this, anItem,thePos);
	return anItem;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TreeItem* TreeArea::InsertString(TreeItem *theParent, const GUIString &theStr, Image *theIcon, int thePos)
{
	TextListItemPtr anItem = new TextListItem;
	anItem->mText = theStr;
	anItem->mColor = mTextColor;
	anItem->mFont = mFont;	
	anItem->mIcon = theIcon;
	return InsertItem(theParent, anItem, thePos);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeArea::DeleteItem(TreeItem *theItem)
{
	theItem->RemoveFromParent(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeArea::Clear(bool fireEventOnChange)
{
	DeleteAllChildren(NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeArea::DeleteAllChildren(TreeItem *theParent)
{
	if(theParent==NULL)
	{
		ListArea::Clear();
		mRoot->mChildren.clear();
	}
	else
		theParent->RemoveAllChildren(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeArea::OpenItem(TreeItem *theItem, bool sendEvent)
{
	theItem->SetOpen(this, true, false, sendEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeArea::CloseItem(TreeItem *theItem, bool closeAll, bool sendEvent)
{
	theItem->SetOpen(this, false, closeAll, sendEvent); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeArea::ToggleItem(TreeItem *theItem, bool closeAll, bool sendEvent)
{
	theItem->ToggleOpen(this, closeAll, sendEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeArea::MouseDown(int x, int y, MouseButton theButton)
{
	ListArea::MouseDown(x,y,theButton);	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeArea::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==ComponentEvent_ButtonPressed && theEvent->mComponent->GetControlId()==ControlId_TreeToggleOpen)
	{
			TreeItem* anItem = (TreeItem*)FindItemAt(theEvent->mComponent->Top());
			if(anItem!=NULL)
				ToggleItem(anItem,TreeFlagSet(TreeFlag_CloseAll));
	}
	else
		ListArea::HandleComponentEvent(theEvent);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TreeArea::SetTreeFlags(int theFlags, bool on)
{
	if(on)
		mTreeFlags |= theFlags;
	else
		mTreeFlags &= ~theFlags;
}

