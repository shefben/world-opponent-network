#include "MultiListBox.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListItem::InsertItem(ListItem *theItem)
{
	mItems.push_back(theItem);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListItem::SetItem(ListItem *theItem, int thePos)
{
	if(mItems.size()<=thePos)
		mItems.resize(thePos+1);

	mItems[thePos] = theItem;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListItem* MultiListItem::GetItem(int thePos) const
{
	if(thePos<0 || thePos>=mItems.size())
		return NULL;
	else
		return mItems[thePos];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListItem::Paint(Graphics &g, ListArea *theListArea)
{
	MultiListArea *aListArea = (MultiListArea*)theListArea;
	int aColumn = aListArea->mColumnPaintContext;
	if(aColumn<0 && mSelected) // can paint between columns
	{
		if(aListArea->ListFlagSet(ListFlag_FullRowSelect))
		{
			g.SetColor(aListArea->GetSelColor().GetBackgroundColor(g));
	/*		int anXPos = 0, aColBorderWidth = aListArea->mColumnBorderWidth;
			MultiListArea::ColumnInfoVector &aVec = aListArea->mColumnInfoVector;
			for(int i=0; i<aVec.size(); i++)
			{
				anXPos += aVec[i].mWidth;
				g.FillRect(anXPos-aColBorderWidth,0,aColBorderWidth,mHeight);
			}*/
			g.FillRect(0,0,aListArea->mTotalColumnWidth,mHeight);
		}
		return;
	}

	if(aColumn >= mItems.size())
		return;

	ListItem *anItem = mItems[aColumn];
	if(anItem!=NULL)
	{
		anItem->mHeight = mHeight;
		if(mSelected && (aListArea->ListFlagSet(ListFlag_FullRowSelect) || aColumn==0))
			anItem->mSelected = mSelected;
		else
			anItem->mSelected = false;

		unsigned char anAlign = aListArea->mColumnInfoVector[aColumn].mHAlign;
		int tx = 0;
		if(anAlign==HorzAlign_Center) tx = (aListArea->mPaintColumnWidth - anItem->mWidth)/2;
		else if(anAlign==HorzAlign_Right) tx = aListArea->mPaintColumnWidth - anItem->mWidth;
		if(tx<0) tx = 0;

		g.Translate(tx,0);
		anItem->Paint(g,theListArea);
		g.Translate(-tx,0);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListItem::SetViewport(ListArea *theListArea, Container *theViewport)
{
	MultiListArea::ColumnInfoVector &aColVec = ((MultiListArea*)theListArea)->mColumnInfoVector;
	int i =0;

	ItemVector::iterator anItemItr = mItems.begin();
	while(anItemItr!=mItems.end())
	{
		ListItem *anItem = *anItemItr;
		if(anItem!=NULL)
		{
			if(i < aColVec.size())
			{
				Container *aViewport;
				if(theViewport==NULL)
					aViewport = NULL;
				else
					aViewport = aColVec[i].mContainer;

				anItem->SetViewport(theListArea,aViewport);
			}
		}

		++i;
		++anItemItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListItem::CalcDimensions(ListArea *theListArea)
{
	MultiListArea *aListArea = (MultiListArea*)theListArea;

	mHeight = 0;
	ItemVector::iterator anItemItr = mItems.begin();
	while(anItemItr!=mItems.end())
	{
		ListItem *anItem = *anItemItr;
		if(anItem!=NULL)
		{
			anItem->CalcDimensions(theListArea);
			if(anItem->mHeight > mHeight)
				mHeight = anItem->mHeight;
		}

		++anItemItr;
	}

	mWidth = aListArea->mTotalColumnWidth;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MultiListItem::Compare(const ListItem *theItem, ListArea *theListArea) const
{
	int aCol = ((MultiListArea*)theListArea)->mSortColumn;
	ListItem *anItem1 = GetItem(aCol);
	ListItem *anItem2 = ((MultiListItem*)theItem)->GetItem(aCol);
	if (anItem1 == anItem2) // Need to return equal at some point if both items are null or sort alg will fail.  if pointers are equal save time by not comparing since they are the same item
		return 0;
	else if(anItem1==NULL)
		return -1;
	else if(anItem2==NULL)
		return 1;
	else
	{
		typedef int(*SortFunc)(ListArea *theListArea, const ListItem *l1, const ListItem *l2);
		SortFunc aFunc = ((MultiListArea*)theListArea)->mSortColFunc;
		if(aFunc!=NULL)
			return aFunc(theListArea,anItem1,anItem2);
		else
			return anItem1->Compare(anItem2, theListArea);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListItem* MultiListItem::GetMouseItem(ListArea *theListArea, int &x)
{
	MultiListArea *aList = (MultiListArea*)theListArea;
	int aColNum = aList->GetColumnAt(x);
	x-=aList->GetColumnPos(aColNum);
	return GetItem(aColNum);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListItem* MultiListItem::GetMouseItem(ListArea *theListArea, int &x, int &y)
{
	MultiListArea *aList = (MultiListArea*)theListArea;
	int aColNum = aList->GetColumnAt(x);
	x-=aList->GetColumnPos(aColNum);
	ListItem *aNewMouseItem = GetItem(aColNum);
	ListItem *aMouseColItem = aList->mMouseColItem;
	if(aMouseColItem!=NULL && (aMouseColItem!=aNewMouseItem || !aMouseColItem->Contains(x,y+my)))
	{
		aMouseColItem->MouseExit(theListArea);
		theListArea->SetCursor(NULL);

		aList->mMouseColItem = NULL;
	}


	if(aMouseColItem==NULL)
	{
		if(aNewMouseItem!=NULL && aNewMouseItem->Contains(x,y+my))
		{
			aList->mMouseColItem = aNewMouseItem;
			aList->mMouseColNum = aColNum;
			aNewMouseItem->MouseEnter(theListArea,x,y);
		}
	}

	return aList->mMouseColItem;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MultiListItem::CheckMouseSel(ListArea *theListArea, int x, int y)
{
	if(!ListItem::CheckMouseSel(theListArea,x,y))
		return false;

	ListItem *anItem = GetMouseItem(theListArea,x);
	if(anItem!=NULL)
		return anItem->CheckMouseSel(theListArea,x,y);
	else
		return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListItem::SetPos(int x, int y)
{
	ListItem::SetPos(x,y);
	ItemVector::iterator anItemItr = mItems.begin();
	while(anItemItr!=mItems.end())
	{
		ListItem *anItem = *anItemItr;
		if(anItem!=NULL)
			anItem->SetPos(0,y);

		++anItemItr;
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListItem::MouseEnter(ListArea *theListArea, int x, int y)
{
	GetMouseItem(theListArea,x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListItem::MouseExit(ListArea *theListArea)
{
	ListItem *anItem = ((MultiListArea*)theListArea)->mMouseColItem;
	if(anItem!=NULL)
	{
		anItem->MouseExit(theListArea);
		((MultiListArea*)theListArea)->mMouseItem = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListItem::MouseMove(ListArea *theListArea, int x, int y)
{
	ListItem *anItem = GetMouseItem(theListArea,x,y);
	if(anItem!=NULL)
		anItem->MouseMove(theListArea,x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListItem::MouseDrag(ListArea *theListArea, int x, int y)
{
	MultiListArea *aList = (MultiListArea*)theListArea;
	ListItem *anItem = aList->mMouseColItem;
	if(anItem!=NULL)
		anItem->MouseDrag(theListArea,x-aList->GetColumnPos(aList->mMouseColNum),y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListItem::MouseDown(ListArea *theListArea, int x, int y, MouseButton theButton)
{
	ListItem *anItem = GetMouseItem(theListArea,x,y);
	if(anItem!=NULL)
		anItem->MouseDown(theListArea,x,y,theButton);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListItem::MouseUp(ListArea *theListArea, int x, int y, MouseButton theButton)
{	
	ListItem *anItem = GetMouseItem(theListArea,x,y);
	if(anItem!=NULL)
		anItem->MouseUp(theListArea,x,y,theButton);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiListHeader::MultiListHeader(MultiListArea *theArea)
{
	SetComponentFlags(ComponentFlag_Clip,true);
	mDragColumn = -1;
	mCurSortCol = -1;
	mDrawSortArrow = true;
	SetMultiListArea(theArea);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////	
void MultiListHeader::SetMultiListArea(MultiListArea *theListArea)
{
	mListArea = theListArea;
	if(mListArea!=NULL)
		mListArea->mListHeader = this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////	
void MultiListHeader::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==ComponentEvent_ButtonPressed && mListArea!=NULL)
	{
		Component *aChild = theEvent->mComponent;
		Container *aParent = aChild->GetParent();
		while(aParent!=this && aParent!=NULL)
		{
			aChild = aParent;
			aParent = aChild->GetParent();
		}

		if(aParent==NULL)
			return;

		int aCol = aChild->GetControlId();
		if(aCol>=0 && aCol<mListArea->GetNumColumns() && aCol<mColumnVector.size())
		{
			ListSortType aType = mColumnVector[aCol].mSortType;
			if(aCol==mCurSortCol || aType==ListSortType_None)
			{
				if(aType==ListSortType_Ascending)
					aType = ListSortType_Descending;
				else
					aType = ListSortType_Ascending;
			}
			//mCurSortCol = aCol;
			mListArea->SetSortColumn(aCol, aType);
			return;
		}
	}

	Container::HandleComponentEvent(theEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListHeader::SetSortColumn(int theCol, ListSortType theType)
{
	if(theCol<0 || theCol>=mColumnVector.size())
		return;

	mColumnVector[theCol].mSortType = theType;
	mCurSortCol = theCol;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListHeader::SetColumnItem(int theColumn, Component *theItem)
{
	mDragColumn = -1;

	if(mColumnVector.size() <= theColumn)
		mColumnVector.resize(theColumn+1);

	Component *aComponent = mColumnVector[theColumn].mComponent;
	if(aComponent!=NULL)
		RemoveChild(aComponent);

	theItem->SetControlId(theColumn);
	theItem->SetComponentFlags(ComponentFlag_Clip,true);
	mColumnVector[theColumn].mComponent = theItem;
	AddChild(theItem);

	if(mListArea!=NULL && theColumn<mListArea->mColumnInfoVector.size() && mListArea->GetColumnWidth(theColumn)<=0)
	{
		int width,height;
		theItem->GetDesiredSize(width,height);
		mListArea->SetColumnWidth(theColumn,width); // will call NotifyColumnChange

		if(mListArea->mSortColumn==theColumn)
		{
			mCurSortCol = theColumn;
			mColumnVector[theColumn].mSortType = mListArea->mSortType;
		}
	}
	else
		NotifyColumnChange();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListHeader::NotifyColumnChange()
{
	if(mListArea==NULL)
		return;

	MultiListArea::ColumnInfoVector &aVec = mListArea->mColumnInfoVector;
	int i, x = -mListArea->GetHorzOffset();
	int aSize = mColumnVector.size();
	for(i=0; i<aSize; i++)
	{
		Component *aComponent = mColumnVector[i].mComponent;

		int aWidth = -1;
		if(i<aVec.size())
			aWidth = aVec[i].mWidth;

		if(aComponent!=NULL)
		{
			if(aWidth==-1)
			{
				aComponent->Enable(false);
//				aComponent->SetComponentFlags(ComponentFlag_Disabled,true);
				aWidth = 0;
			}
			else
				aComponent->Enable(true);

			aComponent->SetVisible(aWidth!=0);
			aComponent->SetPosSize(x,0,aWidth,Height());
		}
		x+=aWidth;
	}

	if(mColumnVector.size()>0)
	{
		Component *aComponent = mColumnVector[mColumnVector.size()-1].mComponent;
		aComponent->SetVisible(true);
		aComponent->SetPosSize(aComponent->Left(),0,Width(),Height());
	}

	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListHeader::SizeChanged()
{
	Container::SizeChanged();
	NotifyColumnChange();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListHeader::Paint(Graphics &g)
{
//	int anOffset = mListArea->GetHorzOffset();
//	g.Translate(-anOffset,0);
	Container::Paint(g);
//	g.Translate(anOffset,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MultiListHeader::GetColumnIndex(Component *theComponent)
{
	int i;
	for(i=0; i<mColumnVector.size(); i++)
	{
		Component *aComponent = mColumnVector[i].mComponent;
		if(aComponent==theComponent)
			return i;
	}

	return -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListSortType MultiListHeader::GetSortType(int theColumnIndex)
{
	if(theColumnIndex<0 || theColumnIndex>=mColumnVector.size() || theColumnIndex!=mCurSortCol)
		return ListSortType_None;

	return mColumnVector[theColumnIndex].mSortType;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ListSortType MultiListHeader::GetSortTypeForButton(Component *theHeaderButton)
{
	// Find Sort Order
	Component *aHeaderComp = theHeaderButton;
	MultiListHeader *aHeader = NULL;
	while(true)
	{
		Container *aParent = aHeaderComp->GetParent();
		if(aParent==NULL)
			return ListSortType_None;

		aHeader = dynamic_cast<MultiListHeader*>(aParent);
		if(aHeader!=NULL)
		{
			if(aHeader->GetDrawSortArrow())
				return aHeader->GetSortType(aHeader->GetColumnIndex(aHeaderComp));
			else
				return ListSortType_None;
		}

		aHeaderComp = aParent;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Component* MultiListHeader::GetDragComponent(int x)
{
	mDragColumn = -1;
	mDragComponent = NULL;

	int i;
	MultiListArea::ColumnInfoVector &aVec = mListArea->mColumnInfoVector;
	for(i=0; i<mColumnVector.size(); i++)
	{
		Component *aComponent = mColumnVector[i].mComponent;
		if(aComponent!=NULL && x>=aComponent->Left() && x<=aComponent->Right())
		{
			int aWidth = aComponent->Width()/4;
			if(aWidth>5)
				aWidth = 5;

			if(x-aComponent->Left()<aWidth && i!=0)
				mDragColumn = i-1;
			else if(aComponent->Right()-x<aWidth && i!=mColumnVector.size())
				mDragColumn = i;
			else
				return NULL;

			if(mDragColumn>=aVec.size())
			{
				mDragColumn = -1;
				return NULL;
			}

			MultiListArea::ColumnInfo &anInfo = aVec[mDragColumn];
			if(anInfo.mSizeMode==0 || (anInfo.mSizeMode==2 && (anInfo.mPartnerColumn<0 || anInfo.mPartnerColumn>=aVec.size())))
			{
				mDragColumn = -1;
				return NULL;
			}

			mDragX = x - mListArea->mColumnInfoVector[mDragColumn].mWidth;
			mDragComponent = mColumnVector[mDragColumn].mComponent;
			return mDragComponent;
		}
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Component* MultiListHeader::FindChildAt(int x, int y)
{
//	if(GetDragComponent(x)!=NULL)
//	{
//		return NULL;
//	}
//	else
		return Container::FindChildAt(x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListHeader::MouseDown(int x, int y, MouseButton theButton)
{
	if(theButton==MouseButton_Left && GetDragComponent(x)!=NULL)
		return;

	Container::MouseDown(x,y,theButton);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListHeader::MouseMove(int x, int y)
{
	Container::MouseMove(x,y);
	if(GetDragComponent(x)!=NULL)
//	if(mDragComponent!=NULL)
		SetCursor(Cursor::GetStandardCursor(StandardCursor_EastWest));
	else if(mCurrentCursor.get()==Cursor::GetStandardCursor(StandardCursor_EastWest)) // hack
		SetCursor(NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListHeader::MouseDrag(int x, int y)
{
	if(mDragColumn>=0)
	{
		int aNewWidth = x - mDragX;
		if(aNewWidth<0)
			aNewWidth = 0;

		mListArea->SetColumnWidth(mDragColumn,aNewWidth);
		SetCursor(Cursor::GetStandardCursor(StandardCursor_EastWest));
	}
	else
		Container::MouseDrag(x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListHeader::MouseUp(int x, int y, MouseButton theButton)
{
	if(theButton==MouseButton_Left && mDragColumn!=-1)
		mDragColumn = -1;
	else
		Container::MouseUp(x,y,theButton);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListHeader::MouseExit()
{
	mDragColumn = -1;
	Container::MouseExit();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiListArea::MultiListArea(int theNumColumns)
{
	mLineVector.reserve(11000);
	mTotalColumnWidth = 0;
	mSortColumn = 0;
	mSortColumnPrev = 0;
	mListHeader = NULL;
	mColumnBorderWidth = 5;
	mDynamicSizeColumn = -1;
	mMaxTotalColumnWidth = 0;
	mMouseColItem = NULL;
	mSortColFunc = NULL;
	mAutoGrowColumns = false; 
	SetNumColumns(theNumColumns);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListArea::NotifyMouseItemGone()
{
	ListArea::NotifyMouseItemGone();
	mMouseColItem = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListArea::NotifyListHeader()
{
	if(mListHeader!=NULL)
		mListHeader->NotifyColumnChange();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListArea::SetNumColumns(int theNumColumns)
{
	mColumnInfoVector.resize(theNumColumns);
	mTotalColumnWidth = 0;
	for(int i=0; i<mColumnInfoVector.size(); i++)
	{
		ScrollAreaViewport *aContainer = mColumnInfoVector[i].mContainer;
		if(aContainer==NULL)
		{
			aContainer = new ScrollAreaViewport(this);
			mColumnInfoVector[i].mContainer = aContainer;
			GetViewport()->AddChild(aContainer);
		}

		mTotalColumnWidth += mColumnInfoVector[i].mWidth;
	}

	NotifyListHeader();
	AdjustPosition();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListArea::SetColumnSizeMode(int theCol, unsigned char theMode)
{
	if(theCol<0 || theCol >= mColumnInfoVector.size())
		return;
	
	int anOldMode = mColumnInfoVector[theCol].mSizeMode;
	mColumnInfoVector[theCol].mSizeMode = theMode;	

	if(theMode!=2 && anOldMode!=2)
		return;

	int aPrevPartner = -1;
	for(int i=0; i<mColumnInfoVector.size(); i++)
	{
		ColumnInfo &anInfo = mColumnInfoVector[i];
		if(anInfo.mSizeMode==2)
		{
			if(aPrevPartner>=0)
				mColumnInfoVector[aPrevPartner].mPartnerColumn = i;

			aPrevPartner = i;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListArea::SetColumnHAlign(int theCol, HorzAlign theAlign)
{
	if(theCol<0 || theCol>=mColumnInfoVector.size())
		return;

	mColumnInfoVector[theCol].mHAlign = theAlign;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListArea::SetColumnWidth(int theCol, int theWidth, bool fromSizeChanged)
{
	if(theCol < mColumnInfoVector.size())
	{
		if(theWidth<0)
			theWidth = 0;

		ColumnInfo &aCol = mColumnInfoVector[theCol];
		if(!fromSizeChanged && aCol.mSizeMode==2 && aCol.mPartnerColumn>=0 && aCol.mPartnerColumn<mColumnInfoVector.size())
		{
			ColumnInfo &aCol2 = mColumnInfoVector[aCol.mPartnerColumn];
			int anOldWidth = aCol.mWidth;
			int aDelta = theWidth - anOldWidth;
			if(aDelta>aCol2.mWidth)
				aDelta = aCol2.mWidth;

			aCol.mWidth += aDelta;
			aCol2.mWidth -= aDelta;
		}
		else
		{
			int anOldWidth = aCol.mWidth;

			int aDelta = theWidth - anOldWidth;
			if(mMaxTotalColumnWidth>0 && aDelta>0 && mTotalColumnWidth+aDelta>mMaxTotalColumnWidth)
			{
				aDelta = mMaxTotalColumnWidth - mTotalColumnWidth;
				if(aDelta<0)
					aDelta = 0;
			}

			aCol.mWidth += aDelta;
			mTotalColumnWidth += aDelta;
		}

		int aTotalWidth = 0;
		for(int i=0; i<mColumnInfoVector.size(); i++)
		{
			ScrollAreaViewport *aContainer = mColumnInfoVector[i].mContainer;
			if(aContainer!=NULL)
				aContainer->SetPos(aTotalWidth,0);

			mColumnInfoVector[i].mx = aTotalWidth;
			aTotalWidth += mColumnInfoVector[i].mWidth;
		}
		
		NotifyListHeader();
		AdjustPosition();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MultiListArea::GetColumnWidth(int theCol)
{
	if(theCol<0 || theCol>=mColumnInfoVector.size())
		return 0;

	return mColumnInfoVector[theCol].mWidth;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MultiListArea::GetColumnAt(int x)
{
	for(int i=0; i<mColumnInfoVector.size(); i++)
	{
		if(mColumnInfoVector[i].mx + mColumnInfoVector[i].mWidth >= x)
			return i;
	}

	return mColumnInfoVector.size()-1; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MultiListArea::GetColumnPos(int theCol)
{
	if(theCol<0 || theCol>=mColumnInfoVector.size())
		return -1;

	return mColumnInfoVector[theCol].mx;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListArea::SetSortColumn(int theCol, ListSortType theType)
{
	if(theCol<0 || theCol>=mColumnInfoVector.size())
		return;

	mSortColumnPrev = mSortColumn;
	mSortColumn = theCol;
	mSortType = theType;
	mSortColFunc = mColumnInfoVector[theCol].mSortFunc;
	if(mListHeader!=NULL)
		mListHeader->SetSortColumn(theCol,theType);

	Sort();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListArea::SetSortColumnFunc(int theCol, SortFunc theFunc)
{
	if(theCol<0 || theCol>=mColumnInfoVector.size())
		return;

	mColumnInfoVector[theCol].mSortFunc = theFunc;
	if(mSortColumn==theCol)
		mSortColFunc = theFunc;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListArea::AdjustPosition(bool becauseOfScrollbar)
{
	mTotalWidth = mTotalColumnWidth;
	ListArea::AdjustPosition(becauseOfScrollbar);
	NotifyListHeader();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListArea::SizeChanged()
{
	int oldWidth = mScrollArea.Width();

	ListArea::SizeChanged();
	if (mDynamicSizeColumn >= 0 && mDynamicSizeColumn < GetNumColumns())
	{
		int aSizeDelta = mScrollArea.Width() - oldWidth;
		if(mTotalColumnWidth + aSizeDelta > mScrollArea.Width())
		{
			aSizeDelta = mScrollArea.Width() - mTotalColumnWidth;
			if(aSizeDelta<0)
				aSizeDelta = 0;
		}

		if(aSizeDelta!=0)
			SetColumnWidth(mDynamicSizeColumn, mColumnInfoVector[mDynamicSizeColumn].mWidth + aSizeDelta,true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListArea::ScrollPaint(Graphics &g)
{
	int anXPos = 0;
	bool fullRowSelect = ListFlagSet(ListFlag_FullRowSelect);

	// Give chance to paint over all columns
	mColumnPaintContext = -1;
	mPaintColumnWidth = mTotalColumnWidth;
	ListArea::ScrollPaint(g);

	// Now paint each column
	for(int i=0; i<mColumnInfoVector.size(); i++)
	{		
		ColumnInfo &aCol = mColumnInfoVector[i];
		mColumnPaintContext = i;
		mPaintColumnWidth = aCol.mWidth-mColumnBorderWidth;

		g.PushClipRect(anXPos,mVertOffset,mPaintColumnWidth,mScrollArea.height);
		g.Translate(anXPos,0);
		ListArea::ScrollPaint(g);
		g.Translate(-anXPos,0);
		g.PopClipRect();

/*		if(fullRowSelect && mSelItem!=NULL && IsItemVisible(mSelItem))
		{
			g.SetColor(GetSelColor().GetBackgroundColor());
			g.FillRect(anXPos+aCol.mWidth-mColumnBorderWidth,mSelItem->my,mColumnBorderWidth,mSelItem->mHeight);
		}*/
	
		anXPos += aCol.mWidth;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiListItem* MultiListArea::InsertRow(int theRow)
{
	return InsertItem(new MultiListItem, theRow);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiListItem* MultiListArea::InsertItem(MultiListItem *theItem, int theRow)
{
	ListArea::InsertItem(theItem, theRow);
	return theItem;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiListItem* MultiListArea::UpdateItem(MultiListItem *theItem)
{
	ListArea::UpdateItem(theItem);
	if(mAutoGrowColumns)
	{
		for(int i=0; i<mColumnInfoVector.size(); i++)
		{
			ListItem *anItem = theItem->GetItem(i);
			if(anItem!=NULL && anItem->mWidth>0)
			{
				int aWidth = mColumnInfoVector[i].mWidth;
				if(anItem->mWidth + mColumnBorderWidth > aWidth)
					SetColumnWidth(i,anItem->mWidth + mColumnBorderWidth);
			}
		}
	}
	return theItem;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiListItem* MultiListArea::SetCell(int theRow, int theCol, ListItem *theCellItem)
{
	if(theCol >= GetNumColumns())
		return NULL;

	MultiListItem* aRow = GetItem(theRow);
	if(aRow==NULL)
		return NULL;

	aRow->SetItem(theCellItem, theCol);
	UpdateItem(aRow);
	return aRow;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiListItem* MultiListArea::SetString(int theRow, int theCol, const GUIString &theStr, Image *theIcon)
{
	TextListItemPtr anItem = new TextListItem;
	anItem->mText = theStr;
	anItem->mColor = mTextColor;
	anItem->mFont = mFont;	
	anItem->mIcon = theIcon;
	return SetCell(theRow, theCol, anItem);
}

