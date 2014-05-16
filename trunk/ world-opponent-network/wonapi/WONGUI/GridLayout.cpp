#include "GridLayout.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GridLayout::GridLayout(int theNumRows, int theNumColumns)
{
	mNumRows = theNumRows;
	mNumColumns = theNumColumns;

	mColumnInfo = new ColumnInfo[theNumColumns];
	mRowInfo = new RowInfo[theNumRows];

	mCellInfo = new CellInfo*[theNumRows];
	for(int i=0; i<theNumRows; i++)
		mCellInfo[i] = new CellInfo[theNumColumns];

	mRightPad = mBottomPad = 10;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GridLayout::~GridLayout()
{
	delete [] mColumnInfo;
	delete [] mRowInfo;
	for(int i=0; i<mNumRows; i++)
		delete [] mCellInfo[i];

	delete [] mCellInfo;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GridLayout::CellInfo::SetPosSize(int x, int y, int width, int height)
{
	if(mComponent!=NULL)
		mComponent->SetPosSize(x,y,width,height);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GridLayout::SetCell(int theRow, int theColumn, Component *theComponent)
{
	CellInfo &aCellInfo = mCellInfo[theRow][theColumn];
	aCellInfo.mComponent = theComponent;
	aCellInfo.mMinWidth = theComponent->Width();
	aCellInfo.mMinHeight = theComponent->Height();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GridLayout::SetPosSize(int x, int y, int width, int height, bool commit)
{
	mBounds.x = x;
	mBounds.y = y;
	mBounds.width = width;
	mBounds.height = height;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GridLayout::SetPadding(int theRightPad, int theBottomPad)
{
	mRightPad = theRightPad;
	mBottomPad = theBottomPad;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GridLayout::SetColumnPad(int theColumn, int thePad)
{
	mColumnInfo[theColumn].mRightPad = thePad;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GridLayout::SetRowPad(int theRow, int thePad)
{
	mRowInfo[theRow].mBottomPad = thePad;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GridLayout::CalcColumnWidths()
{
	int aRow, aCol;
	int x = 0;
	for(aCol=0; aCol<mNumColumns; aCol++)
	{
		int aMaxWidth = 0;
		for(aRow=0; aRow<mNumRows; aRow++)
		{
			CellInfo &anInfo = mCellInfo[aRow][aCol];
			int aWidth = anInfo.GetMinWidth();
			if(aWidth>aMaxWidth)
				aMaxWidth = aWidth;
		}

		ColumnInfo &aColumnInfo = mColumnInfo[aCol];

		if(aCol==mNumColumns-1 && x+aMaxWidth<Width())
			aMaxWidth = Width()-x;

		aColumnInfo.mWidth = aMaxWidth;
		aColumnInfo.mx = x;
		int aPad = aColumnInfo.mRightPad;
		if(aPad<0)
			aPad = mRightPad;

		x+=aColumnInfo.mWidth+aPad;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GridLayout::CalcRowHeights()
{
	int aRow, aCol;
	int y=0;
	for(aRow=0; aRow<mNumRows; aRow++)
	{
		int aMaxHeight = 0;
		for(aCol=0; aCol<mNumColumns; aCol++)
		{
			CellInfo &anInfo = mCellInfo[aRow][aCol];
			int aHeight = anInfo.GetMinHeight();
			if(aHeight > aMaxHeight)
				aMaxHeight = aHeight;
		}

		RowInfo &aRowInfo = mRowInfo[aRow];
		aRowInfo.mHeight = aMaxHeight;
		aRowInfo.my = y;
		
		int aPad = aRowInfo.mBottomPad;
		if(aPad<0)
			aPad = mBottomPad;

		y+=aRowInfo.mHeight + aPad;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GridLayout::PositionCells()
{
	int aRow, aCol;
	for(aRow=0; aRow<mNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowInfo[aRow];
		for(aCol=0; aCol<mNumColumns; aCol++)
		{
			ColumnInfo &aColumnInfo = mColumnInfo[aCol];
			mCellInfo[aRow][aCol].SetPosSize(aColumnInfo.mx,aRowInfo.my,aColumnInfo.mWidth,aRowInfo.mHeight);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GridLayout::Execute()
{
	CalcColumnWidths();
	CalcRowHeights();
	PositionCells();
}

