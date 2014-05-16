#include "InputBox.h"
#include "Container.h"
#include "Clipboard.h"
#include "ColorScheme.h"

using namespace std;
using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool InputBox::NumericFilter(InputBox *theInputBox, GUIString &theStr)
{
	int aPos = 0;
	if(theStr.at(0)=='-')
	{
		if(theInputBox->GetSelStart()>0)
			return false;

		aPos++;
	}

	for(;aPos<theStr.length();aPos++)
		if(!isdigit(theStr.at(aPos)))
			return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InputBox::InputBox()
{
	mSelStartChar = mSelEndChar = 0;
	mBackwardsSel = false;
	mSelecting = false;

	mHorzOffset = 0;
	mCursorOn = false;
	mUnderlineCursor = false;
	mSelectOnFocus = false;
	mCursorOnTime = 500;
	mCursorOffTime = 500;

	mFont = GetNamedFont("InputBox");

	SetComponentFlags(ComponentFlag_WantFocus | ComponentFlag_WantTabFocus, true);

	mTextColor = ColorScheme::GetColorRef(StandardColor_Text);
	mDisabledTextColor = ColorScheme::GetColorRef(StandardColor_GrayText);

	mDisabledBackground.SetColor(ColorScheme::GetColorRef(StandardColor_3DFace));
	mUseDisabledColors = true;

	mPasswordChar = 0;
	mAllowSelection = true;

	mMaxChars = 0;

	SetDefaultCursor(Cursor::GetStandardCursor(StandardCursor_IBeam));

	mInputHistoryPos = 0;
	mMaxInputHistory = 100;
	mHaveTraversedHistory = false;

	mTransform = NULL;

	mLeftPad = 4;
	mRightPad = 4;
	mTopPad = 3;
	mBottomPad = 3;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::AdjustTextRect()
{
	int aLeftPad = mLeftPad;
	int aRightPad = mRightPad;
	int aTopPad = mTopPad;
	int aBottomPad = mBottomPad;

	if(mIcon.get()!=NULL)
		aLeftPad += mIcon->GetWidth()+2;

	mTextRect.x = aLeftPad;
	mTextRect.width = Width() - aLeftPad-aRightPad;

	mTextRect.y = aTopPad;
	mTextRect.height = Height() - aTopPad-aBottomPad;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::SetIcon(Image *theIcon)
{
	mIcon = theIcon;
	AdjustTextRect();
	AdjustPosition();
	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::SetAllowSelection(bool allowSelection)
{
	if(allowSelection!=mAllowSelection)
	{
		mAllowSelection = allowSelection;
		if(allowSelection)
			SetDefaultCursor(Cursor::GetStandardCursor(StandardCursor_IBeam));
		else
			SetDefaultCursor(NULL);


		mSelecting = false;
		if(!mAllowSelection && mSelStartChar!=mSelEndChar)
		{
			mSelStartChar = mSelEndChar = 0;
			Invalidate();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::SizeChanged()
{
	Component::SizeChanged();
	AdjustTextRect();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::GetDesiredSize(int &width, int &height)
{
	width = 100;
	height = mFont->GetHeight() + 6;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::SetCharWidth(int theEm)
{
	SetSize(mFont->GetCharWidth('M')*theEm + 8,Height());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::GotFocus()
{
	Component::GotFocus();
	mCursorDelay = 0;
	mCursorOn = true;
	RequestTimer(true);
	if(mSelectOnFocus)
		SetSel();

	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::LostFocus()
{
	Component::LostFocus();
	mCursorOn = false;
	RequestTimer(false);
	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::CheckSel(int x, int y, bool newStart)
{
	if(!mAllowSelection)
		return;

	if(x<0) x = 0;
	else if(x>Width()) x = Width();
	
	x += mHorzOffset - mTextRect.x;

	Font* aFM = mFont;
	int aSelChar = mBackwardsSel?mSelStartChar:mSelEndChar;
	int aWidth = aFM->GetStringWidth(mText, 0, aSelChar);
	if(aWidth >= x)
	{
		aSelChar--;
		while(aSelChar>=0 && aWidth>=x)
		{
			aWidth-=aFM->GetCharWidth(mText.at(aSelChar));
			if(aWidth>=x)
				aSelChar--;
		}
		aSelChar++;
	}
	else
	{
		while(aSelChar<mText.length() && aWidth<x)
		{
			aWidth+=aFM->GetCharWidth(mText.at(aSelChar));
//			if(aWidth<=x)
				aSelChar++;
		}
	}
	if(aSelChar<0)
		aSelChar=0;
	if(aSelChar>mText.length())
		aSelChar = mText.length();


	if(newStart)
	{
		mSelStartChar = mSelEndChar = aSelChar;
		mBackwardsSel = false;
	}
	else
	{
		if(mBackwardsSel)
			mSelStartChar = aSelChar;
		else
			mSelEndChar = aSelChar;

		if(mSelStartChar > mSelEndChar)
		{
			mBackwardsSel = !mBackwardsSel;
			std::swap(mSelStartChar,mSelEndChar);
		}
	}
	
	Invalidate();
}	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::MouseDown(int x, int y, MouseButton theButton)
{
	Component::MouseDown(x,y,theButton);
	if(theButton==MouseButton_Left)
	{
		CheckSel(x,y,(GetKeyMod() & KEYMOD_SHIFT)?false:true);

		if(mAllowSelection)
			mSelecting = true;

		RequestFocus();	
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::MouseUp(int x, int y, MouseButton theButton)
{
	Component::MouseUp(x,y,theButton);

	if(theButton==MouseButton_Left)
		mSelecting = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::MouseExit()
{
	Component::MouseExit();
	mSelecting = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::MouseDrag(int x, int y)
{
	CheckSel(x,y,false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::SetCursorBlinkRate(int theOnTime, int theOffTime) // off time defaults to on time
{
	if(theOffTime<=0)
		theOffTime = theOnTime;

	mCursorOnTime = theOnTime;
	mCursorOffTime = theOffTime;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool InputBox::TimerEvent(int theDelta)
{
	mCursorDelay+=theDelta;
	if(mCursorOn && mCursorDelay>mCursorOnTime)
	{
		mCursorDelay%=mCursorOnTime;
		mCursorOn = false;
		Invalidate();
	}
	else if(!mCursorOn && mCursorDelay>mCursorOffTime)
	{
		mCursorDelay%=mCursorOffTime;
		mCursorOn = true;
		Invalidate();		
	}

	if(mSelecting)
	{
		int x,y;
		GetMousePos(x,y);
		if(!(x>=0 && x<=Width() && y>=0 && y<=Height()))
		{
			int aCharPos = mBackwardsSel?mSelStartChar:mSelEndChar;

			if(x>Width() && aCharPos!=mText.length())
			{
				mHorzOffset += x-Width();
				if(mHorzOffset<0)
					mHorzOffset = 0;
			}
			else if(x<0 && aCharPos!=0)
				mHorzOffset += x;

			CheckSel(x,y,false);

			aCharPos = mBackwardsSel?mSelStartChar:mSelEndChar;
			if(aCharPos==mText.length())
			{
				int aTextWidth = mFont->GetStringWidth(mText);
				if(aTextWidth + mHorzOffset > mTextRect.Width())
					mHorzOffset = aTextWidth-mTextRect.Width()+mFont->GetStringWidth("M");
			}
			if(aCharPos==0)
				mHorzOffset = 0;

			if(mHorzOffset<0)
				mHorzOffset = 0;

		/*
			int aCharPos = mBackwardsSel?mSelStartChar:mSelEndChar;

			if(x>Width())
				aCharPos += (x-Width())/5 + 1;
			else if(x<0)
				aCharPos += x/5 - 1;

			if(mBackwardsSel)
				SetSel(mSelEndChar,aCharPos);
			else
				SetSel(mSelStartChar,aCharPos);*/
		}
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::AdjustPosition()
{
	Font* aFM = mFont;
	int aCursorPos = mBackwardsSel?mSelStartChar:mSelEndChar;

	int aPreWidth = aFM->GetStringWidth(mText, 0, aCursorPos);
	int anM = aFM->GetCharWidth('M');

	if(aPreWidth - mHorzOffset < 0)
	{
		mHorzOffset = aPreWidth - 10*anM;
		if(mHorzOffset<0)
			mHorzOffset = 0;
	}

	if(aPreWidth - mHorzOffset >= mTextRect.width)
	{
		if(mTextRect.width!=0 && aPreWidth-mHorzOffset!=0)
			mHorzOffset = aPreWidth - mTextRect.width + anM;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::SetSel(int theStartPos, int theEndPos)
{

	if(theStartPos<0) theStartPos = mText.length();
	if(theEndPos<0) theEndPos = mText.length();
	if(theStartPos>mText.length()) theStartPos = mText.length();
	if(theEndPos>mText.length()) theEndPos = mText.length();

	mBackwardsSel = theStartPos>theEndPos;
	if(mBackwardsSel)
		std::swap(theStartPos,theEndPos);

	mSelStartChar = theStartPos;
	mSelEndChar = theEndPos;
	AdjustPosition();
	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::CopySelection()
{
	Clipboard::Set(mText.substr(mSelStartChar,mSelEndChar - mSelStartChar));	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool InputBox::KeyChar(int theKey)
{
	const GUIString &aText = GetText();
	GUIString aPasteStr;

	if(theKey==22) 	// CTRL-V
		aPasteStr = Clipboard::Get();
	else if(theKey==3)	// CTRL-C
	{
		CopySelection();
		return true;
	}
	else if(theKey==24)	// CTRL-X
	{
		CopySelection();
//		int aSelEndChar = mSelEndChar;
//		mSelEndChar = mSelStartChar;
//		SetTextPrv(aText.substr(0,mSelStartChar) + aText.substr(aSelEndChar));
	}
	else if(theKey>31)
		aPasteStr.Fill(theKey,0,1);
	else
		return true;
	
	SetSelText(aPasteStr);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::SetSelText(const GUIString &theText, bool sendEvent)
{	
	const GUIString &aText = GetText();
	GUIString aPasteStr = theText;
	// validate the new characters
	for(int i=0; i<aPasteStr.length(); i++)
	{
		int aChar = aPasteStr.at(i);
		if(aChar<32)
		{
			aPasteStr.resize(i);
			break; 
		}
	}
	if(mTransform && !mTransform(this,aPasteStr))
		return;
	
	// Enforce max chars
	if(mMaxChars>0 && aText.length()+aPasteStr.length()-(mSelEndChar-mSelStartChar)>mMaxChars)
	{
		int aNumCharsAllowed = mMaxChars-aText.length()+(mSelEndChar-mSelStartChar);
		if(aNumCharsAllowed<0)
			aNumCharsAllowed = 0;

		aPasteStr.resize(aNumCharsAllowed);
	}

	// Add string to text
	if(aPasteStr.length()>0 || mSelStartChar!=mSelEndChar)
	{
		int aSelStartChar = mSelStartChar, aSelEndChar = mSelEndChar;
		mSelStartChar+=aPasteStr.length();
		mSelEndChar = mSelStartChar;
		GUIString aNewStr;
		aNewStr.append(aText,0,aSelStartChar);
		aNewStr.append(aPasteStr);
		aNewStr.append(aText,aSelEndChar);
		SetTextPrv(aNewStr, sendEvent);
	}
	
	mCursorDelay = 0;
	mCursorOn = true;
	AdjustPosition();
	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int InputBox::MoveCursor(bool left, bool wholeWord, const GUIString &theStr, int thePos)
{			
	int dx = left ? -1 : 1;
	
	thePos+=dx;
	if(wholeWord)
	{		
		while(thePos>=0 && thePos<theStr.length())
		{
			if((theStr.at(thePos)!=' '))
				thePos+=dx;
			else
				break; 
		}		

		while(thePos>=0 && thePos<theStr.length())
		{
			if((theStr.at(thePos)==' '))
				thePos+=dx;
			else
				break; 
		}		
	
		if(left)
			thePos++;
	}
	
	if(thePos<0)
		thePos = 0;
	if(thePos>theStr.length())
		thePos = theStr.length();

	return thePos;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::AddInputHistory(const GUIString &theText, bool clearTraversalState)
{
	if(theText.empty())
		return;

	int aHistoryPos = mInputHistoryPos;
	while(mInputHistory.size()>=mMaxInputHistory)
	{
		mInputHistory.pop_front();
		aHistoryPos--;
	}
		
	if(theText.length()>0 && mMaxInputHistory>0)
	{
		if(mInputHistory.empty() || mInputHistory.back()!=theText) // don't insert the same one twice
			mInputHistory.push_back(theText);
	}
	if(aHistoryPos>=0 && aHistoryPos<mInputHistory.size() && mInputHistory[aHistoryPos]==theText)
		mInputHistoryPos = aHistoryPos;
	else
		mInputHistoryPos = mInputHistory.size();

	if(clearTraversalState)
		mHaveTraversedHistory = false;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::SetInputHistory(const GUIString &theText, bool clearTraversalState)
{
	if(theText.empty())
		return;


	int aPos = mInputHistoryPos;
	if(aPos>=0 && aPos<mInputHistory.size())
		mInputHistory[aPos] = theText;
	else
	{
		AddInputHistory(theText, clearTraversalState);
		mInputHistoryPos = aPos;
	}

	if(clearTraversalState)
		mHaveTraversedHistory = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static int MyMin(int a, int b)
{
	if(a<=b)
		return a;
	else 
		return b;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool InputBox::KeyDown(int theKey)
{
	const GUIString &aText = GetText();

	int aKeyMod = GetKeyMod();
	bool shift = (aKeyMod & KEYMOD_SHIFT)?true:false;
	bool ctrl = (aKeyMod & KEYMOD_CTRL)?true:false; 

	if(!mAllowSelection)
		shift = false;
	
	switch(theKey)
	{		
		case KEYCODE_RETURN: // Add text to history
/*				if(aText.length()>0 && mHistory.size()<mResources[Constants.TEXTBOX_HISTORY_SIZE])
			{
				mHistory.addElement(aText);
				if(mHistory.size()>=mResources[Constants.TEXTBOX_HISTORY_SIZE])
					mHistory.removeElementAt(0);
					
				mInputHistoryPos=mHistory.size();
			}						*/
			
			//  Notify parent
			if(aText.length()>0)
				FireEvent(ComponentEvent_InputReturn);
			return true;

		case KEYCODE_UP:	
		case KEYCODE_DOWN:
		{
//			int aSize = min(mMaxInputHistory,mInputHistory.size());
			int aSize = MyMin(mMaxInputHistory,mInputHistory.size());
			if(aSize>0)
			{
				if(!mHaveTraversedHistory && mInputHistoryPos<mInputHistory.size())
					mHaveTraversedHistory = true;
				else
				{
					mHaveTraversedHistory = true;
					SetInputHistory(GetText());
					if(theKey==KEYCODE_UP)
						mInputHistoryPos--;
					else
						mInputHistoryPos++;
				}
				
				if(mInputHistoryPos<0)
					mInputHistoryPos = 0;
			
				if(mInputHistoryPos>=aSize)
				{
					SetText("");
					mInputHistoryPos = mInputHistory.size();
				}
				else
				{
					SetText(mInputHistory[mInputHistoryPos]);
					SetSel(GetText().length());
				}
			}
		}
		break;
		
		case KEYCODE_BACK: 
		case KEYCODE_DELETE:  
		{
			GUIString left, right;
			if(mSelStartChar!=mSelEndChar) // If selection exists, just delete it
			{
				left.assign(aText,0,mSelStartChar);
				right.assign(aText,mSelEndChar);
			}
			else // No selection.  Need to get rid of one character
			{
				if(theKey==KEYCODE_BACK) // backspace -> Get rid of character behind the cursor
				{
					if(mSelStartChar>0)
						left.assign(aText,0,mSelStartChar-1);

					right.assign(aText,mSelEndChar);
				}
				else  // delete -> Get rid of character in front of the cursor
				{
					left.assign(aText,0,mSelStartChar);
					
					if(mSelEndChar<aText.length())
						right.assign(aText,mSelEndChar+1);
				}
			}	
								
			mSelStartChar = mSelEndChar = left.length();
			GUIString total = left;
			total.append(right);
			SetTextPrv(total);
		}
		break;
						

		case KEYCODE_HOME: // Go to first character
			if(!shift)
				mSelEndChar = 0;
			else if(!mBackwardsSel)
				mSelEndChar = mSelStartChar;				
			
			mSelStartChar = 0;
			mBackwardsSel =  mSelStartChar!=mSelEndChar;				
			break;
			
		case KEYCODE_END: // Go to last character
			if(!shift)
				mSelStartChar = aText.length();
			else if(mBackwardsSel)
				mSelStartChar = mSelEndChar;

			mBackwardsSel = false;
			mSelEndChar = aText.length();
			break;

			
		case KEYCODE_LEFT:
		case KEYCODE_RIGHT: // move cursor left or right by one character or one word
			if(!shift && mSelStartChar!=mSelEndChar) // if selected, get rid of selection in direction of arrow key
			{
				if(theKey==KEYCODE_LEFT)
					mSelEndChar = mSelStartChar;
				else
					mSelStartChar = mSelEndChar;
			}
			else if(mBackwardsSel)
			{
				mSelStartChar = MoveCursor(theKey==KEYCODE_LEFT,ctrl,mText,mSelStartChar);
				
				if(!shift)
					mSelEndChar = mSelStartChar;
			}
			else
			{
				mSelEndChar = MoveCursor(theKey==KEYCODE_LEFT,ctrl,mText,mSelEndChar);
				
				if(!shift)
					mSelStartChar = mSelEndChar;					
			}						

			if(mSelStartChar>mSelEndChar)
			{
				mBackwardsSel = !mBackwardsSel;
				std::swap(mSelStartChar, mSelEndChar);
			}
			else if(mSelStartChar==mSelEndChar)
				mBackwardsSel = false;
			
			break;
							
		default:  
			return false;
	}
	
	mCursorDelay = 0;
	mCursorOn = true;
	AdjustPosition();
	Invalidate();	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const GUIString& InputBox::GetText() const
{
	if(mPasswordChar==0)
		return mText;
	else
		return mActualText;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::SetPasswordChar(char theChar)
{
	mPasswordChar = theChar;
	SetText(mText,false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::SetText(const GUIString &theText, bool sendEvent)
{
	SetTextPrv(theText,sendEvent);
	mSelStartChar = mSelEndChar = 0;//mText.length();
	AdjustPosition();
	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::Clear(bool sendEvent)
{
	SetText(GUIString::EMPTY_STR,sendEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::SetTextPrv(const GUIString &theText, bool sendEvent)
{
	if(mPasswordChar==0)
		mText = theText;
	else
	{
		mActualText = theText;
		mText.Fill(mPasswordChar,0,theText.length());
	}

/*	if(mMaxChars>0 && mText.length()>mMaxChars)
	{
		mText = mText.substr(0,mMaxChars);
		if(mActualText.length()>mMaxChars)
			mActualText = mActualText.substr(0,mMaxChars);

		if(mSelStartChar>mText.length()) mSelStartChar = mText.length();
		if(mSelEndChar>mText.length()) mSelEndChar = mText.length();
		if(mSelStartChar==mSelEndChar) mBackwardsSel = false;
	}*/

	if(sendEvent)
		FireEvent(ComponentEvent_InputTextChanged);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InputBox::Paint(Graphics &g)
{
	Component::Paint(g);

	int aTextColor;
	if(Disabled() && mUseDisabledColors)
	{
		mDisabledBackground.Paint(g,0,0,Width(),Height(),mHorzOffset,0);
		aTextColor = mDisabledTextColor;
	}
	else
	{
		mBackground.Paint(g,0,0,Width(),Height(),mHorzOffset,0);
		aTextColor = mTextColor;
	}

	if(mIcon.get()!=NULL)
		g.DrawImage(mIcon,4,(Height()-mIcon->GetHeight())/2);

	g.Translate(mTextRect.x , mTextRect.y);
	g.PushClipRect(0,0,mTextRect.width, mTextRect.height);
	g.Translate(-mHorzOffset,0);
	
	g.SetFont(mFont);
	g.SetColor(aTextColor);
	g.DrawString(mText, 0, 0);
	int aCursorHeight = mFont->GetHeight();//mTextRect.height;

	if(HasFocus())
	{
		if(mSelStartChar!=mSelEndChar)
		{
			Font *aFM = mFont;
			int x = aFM->GetStringWidth(mText, 0, mSelStartChar);
			int aWidth = aFM->GetStringWidth(mText, mSelStartChar, mSelEndChar - mSelStartChar);
			g.SetColor(mSelColor.GetBackgroundColor(g));
			g.FillRect(x, 0, aWidth, aCursorHeight);
			g.SetColor(mSelColor.GetForegroundColor(g,aTextColor));
			g.PushClipRect(x, 0, aWidth, aCursorHeight);
			g.DrawString(mText, 0, 0);
			g.PopClipRect();
			if(mCursorOn && !mUnderlineCursor)
			{
				if(mBackwardsSel)
					g.DrawLine(x,0,x,aCursorHeight);
				else
					g.DrawLine(x+aWidth-1,0,x+aWidth-1,aCursorHeight);
			}
		}
		else if(mCursorOn)
		{
			int aCursorPos = mSelStartChar;
			Font *aFM = mFont;
			int anXPos = aFM->GetStringWidth(mText, 0, aCursorPos);
			if(mUnderlineCursor)
			{
				int aWidth;
				if(aCursorPos < mText.length())
					aWidth = mFont->GetCharWidth(mText.at(aCursorPos));
				else
					aWidth = mFont->GetCharWidth('M');
				g.DrawLine(anXPos, aCursorHeight-1, anXPos+aWidth-1, aCursorHeight-1);
			}
			else
				g.DrawLine(anXPos, 0, anXPos, aCursorHeight);
		}
	}
	
	g.Translate(mHorzOffset,0);
	g.PopClipRect();
	g.Translate(-mTextRect.x, -mTextRect.y);

}
