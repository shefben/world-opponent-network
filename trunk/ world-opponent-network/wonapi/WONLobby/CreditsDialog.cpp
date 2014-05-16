#include "CreditsDialog.h"
#include "WONGUI/Button.h"
#include "WONGUI/GUIConfig.h"
#include "WONGUI/ChildLayouts.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace WONAPI
{
	class CreditLineInfo
	{
	protected:
		GUIString mLine;
		DWORD     mColor;

	public:
		CreditLineInfo(const GUIString& theLine, DWORD theColor)
		{
			mLine = theLine;
			mColor = theColor;
		}

		GUIString GetLine()
		{
			return mLine;
		}

		DWORD GetColor()
		{
			return mColor;
		}
	};
	typedef std::list<CreditLineInfo> CreditsList;

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	class ScrollingTextComponent : public Component
	{
	protected:
		FontPtr     mFont;
		CreditsList mLines;
		DWORD       mLastColor;
		int         mTimeSinceLastScroll;
		int         mScrollSpeed;
		int         mScrollTop;
		int         mTotalTextHeight;

	public:
		ScrollingTextComponent();
		~ScrollingTextComponent();

		void Start();
		void Stop();
		void Pause();
		void Reset();
		void Clear();
		void AddLine(const GUIString &theText, DWORD theColor = -1);
		void SetNextLineColor(DWORD theColor);

		virtual bool TimerEvent(int theDelta);
		virtual void Paint(Graphics &g);
	};
}


///////////////////////////////////////////////////////////////////////////////
// ScrollingTextComponent: Constructor.
///////////////////////////////////////////////////////////////////////////////
ScrollingTextComponent::ScrollingTextComponent()
	: Component(), mLastColor(0), mScrollSpeed(60), mTimeSinceLastScroll(0),
	  mScrollTop(0), mTotalTextHeight(0)
{
	SetComponentFlags(ComponentFlag_Clip,true);
}

///////////////////////////////////////////////////////////////////////////////
// ScrollingTextComponent: Destructor.
///////////////////////////////////////////////////////////////////////////////
ScrollingTextComponent::~ScrollingTextComponent()
{
}

///////////////////////////////////////////////////////////////////////////////
// Start: Start the scrolling.
///////////////////////////////////////////////////////////////////////////////
void ScrollingTextComponent::Start()
{
	RequestTimer(true);
	Reset();
}

///////////////////////////////////////////////////////////////////////////////
// Stop: Stop the scrolling.
///////////////////////////////////////////////////////////////////////////////
void ScrollingTextComponent::Stop()
{
	RequestTimer(false);
}

///////////////////////////////////////////////////////////////////////////////
// Pause: Pause (or un-pause) the scrolling.
///////////////////////////////////////////////////////////////////////////////
void ScrollingTextComponent::Pause()
{
	RequestTimer(! WantTimer());
}

///////////////////////////////////////////////////////////////////////////////
// Reset: Starts the text back at the beginning.
///////////////////////////////////////////////////////////////////////////////
void ScrollingTextComponent::Reset()
{
	mScrollTop = Height();
}

///////////////////////////////////////////////////////////////////////////////
// Clear: Clear the credits list.
///////////////////////////////////////////////////////////////////////////////
void ScrollingTextComponent::Clear()
{
	mLines.clear();
}

///////////////////////////////////////////////////////////////////////////////
// AddLine: Add the supplied line of text to the credits.  If the color is -1
// use the last color.  If not, use the supplied color.
///////////////////////////////////////////////////////////////////////////////
void ScrollingTextComponent::AddLine(const GUIString &theText, DWORD theColor)
{
	// Adjust or save the color, as appropriate.
	if (theColor != -1)
		mLastColor = theColor;

	// Add the text (and color) to the creduts.
	mLines.push_back(CreditLineInfo(theText, mLastColor));
}

///////////////////////////////////////////////////////////////////////////////
// SetNextLineColor: Set the color for lines that are added after this call.
///////////////////////////////////////////////////////////////////////////////
void ScrollingTextComponent::SetNextLineColor(DWORD theColor)
{
	mLastColor = theColor;
}

///////////////////////////////////////////////////////////////////////////////
// TimerEvent: Handle a new timer event.
///////////////////////////////////////////////////////////////////////////////
bool ScrollingTextComponent::TimerEvent(int theDelta)
{
	mTimeSinceLastScroll += theDelta;
	if (mTimeSinceLastScroll >= mScrollSpeed)
	{
		if (--mScrollTop < -mTotalTextHeight)
			mScrollTop = Height();
		Invalidate();
		mTimeSinceLastScroll = 0;
	}

	return Component::TimerEvent(theDelta);
}

///////////////////////////////////////////////////////////////////////////////
// DimColor: Dim the 'brightness' of the color to nPercent of the original.
///////////////////////////////////////////////////////////////////////////////
DWORD DimColor(DWORD crOrig, int nPercent)
{
	if (nPercent <= 0)
		return 0xFFFFFF;

	if (nPercent >= 100)
		return crOrig;

	int nRed = (crOrig & 0xFF0000) >> 16;
	int nGreen = (crOrig & 0xFF00) >> 8;
	int nBlue = crOrig & 0xFF;

	nRed = 255 - ((255 - nRed) * nPercent / 100);
	nGreen = 255 - ((255 - nGreen) * nPercent / 100);
	nBlue = 255 - ((255 - nBlue) * nPercent / 100);

	DWORD crNew = (nRed << 16) | (nGreen << 8) | nBlue;
	return crNew;
}

///////////////////////////////////////////////////////////////////////////////
// Paint: Draw the credits.
///////////////////////////////////////////////////////////////////////////////
void ScrollingTextComponent::Paint(Graphics &g)
{
	Component::Paint(g);

	// Draw the background.
	g.SetFont(GetDefaultFont());
	g.SetColor(0xFFFFFF);
	g.FillRect(2,2,Width()-4,Height()-4);

	// Draw the frame.
	g.ApplyColorSchemeColor(StandardColor_3DHilight);
	g.DrawRect(0,0,Width(),Height());
	g.DrawRect(1,1,Width()-1,Height()-1);
	g.ApplyColorSchemeColor(StandardColor_3DShadow);
	g.DrawRect(0, 0,Width()-1,Height()-1);

	// Draw the text.
	int nTop = mScrollTop;
	CreditsList::iterator Itr = mLines.begin();

	while (Itr != mLines.end())
	{
		CreditLineInfo& LineInfo = *Itr;
		int nStrHt = g.GetFont()->GetHeight();

		if (nTop < Height() / 3)
			g.SetColor(DimColor(LineInfo.GetColor(), nTop * 100 / (Height() / 3)));
		else if (Height() - nTop <= nStrHt)
			g.SetColor(0xFFFFFF);
		else if (nTop > (Height() * 2 / 3) - nStrHt)
			g.SetColor(DimColor(LineInfo.GetColor(), ((Height() - (nTop + nStrHt)) * 100 / (Height() / 3))));
		else
			g.SetColor(LineInfo.GetColor());

		if (g.GetColor() != 0xFFFFFF && nTop > 2 && nTop < Height() - nStrHt - 2)
		{
			int nStrWd = g.GetFont()->GetStringWidth(LineInfo.GetLine());
			g.DrawString(LineInfo.GetLine(), (Width() - nStrWd) / 2, nTop);
		}
		nTop += nStrHt;
		++Itr;
	}
	mTotalTextHeight = nTop - mScrollTop;
}

///////////////////////////////////////////////////////////////////////////////
// CreditsDialog:: Constuctor.
///////////////////////////////////////////////////////////////////////////////
CreditsDialog::CreditsDialog()
{
	mScrollingText = new ScrollingTextComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreditsDialog::AddLine(const GUIString &theText, Component* theComponent, DWORD theColor)
{
	int aStringWidth = GetDefaultFont()->GetStringWidth(theText) + 20;
	if(aStringWidth > theComponent->Width())
		theComponent->SetWidth(aStringWidth);

	mScrollingText->AddLine(theText,theColor);
}


///////////////////////////////////////////////////////////////////////////////
// Init: Construct the components that are part of the dialog.
//
// Note: The CreditsButton (in the config file) is a placeholder for the 
// scrolling text component.  We use the button to position the other object, 
// then hide the button.  Cheesy, but easy.
///////////////////////////////////////////////////////////////////////////////
void CreditsDialog::InitComponent(ComponentInit &theInit)
{
	Dialog::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mCreditsButton,"CreditsButton");
		WONComponentConfig_Get(aConfig,mCloseButton,"CancelButton");

		// Adjust for the container that is not obvious to this dialog.
		AddChildLayout(mScrollingText, CLI_SameLeft | CLI_SameTop | CLI_SameWidth | CLI_SameHeight, mCreditsButton, mCreditsButton->GetParent()->Left(), mCreditsButton->GetParent()->Top());

		AddChild(mScrollingText);

		mCreditsButton->SetVisible(false);

		UpdateCredits();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreditsDialog::Start() 
{ 
	mScrollingText->Start(); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreditsDialog::Stop()  
{ 
	mScrollingText->Stop(); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreditsDialog::Pause() 
{ 
	mScrollingText->Pause(); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreditsDialog::Reset() 
{ 
	mScrollingText->Reset(); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static GUIString Unencrypt(char* sEncryptedString)
{
	GUIString sString;
	for (int i = 0; i < strlen(sEncryptedString); i++)
		sString.append((char)(((int)sEncryptedString[i]) - 0x70));

	return sString;
}

///////////////////////////////////////////////////////////////////////////////
// UpdateCredits: Add all of us to the scrolling text.
///////////////////////////////////////////////////////////////////////////////
void CreditsDialog::UpdateCredits()
{
	// moved to config
}

///////////////////////////////////////////////////////////////////////////////
// HandleComponentEvent: Respond to events as needed.
///////////////////////////////////////////////////////////////////////////////
void CreditsDialog::HandleComponentEvent(ComponentEvent *theEvent)
{
//	if(theEvent->mType==ComponentEvent_ButtonPressed && theEvent->mComponent==mPasswordCheck)
//	{
//		mPasswordInput->SetVisible(mPasswordCheck->IsChecked());
//		mPasswordLabel->SetVisible(mPasswordCheck->IsChecked());
//	}
//	else
		Dialog::HandleComponentEvent(theEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CreditsDialogConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="ADDLINE")
	{
		GUIString aText = ReadString(); 
		int aColor = -1;
		
		if(!EndOfString())
		{
			EnsureComma();
			aColor = ReadColor();
		}

		mCredits->AddLine(aText,GetChildComponent("DialogComponent"),aColor);
	}
	else
		return DialogConfig::HandleInstruction(theInstruction);

	return true;
}
