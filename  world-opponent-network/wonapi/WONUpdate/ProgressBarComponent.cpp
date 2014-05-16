//----------------------------------------------------------------------------------
// ProgressBarComponent.cpp
//----------------------------------------------------------------------------------
#include "ProgressBarComponent.h"
#include "WONGUI/ColorScheme.h"
#include "WONGUI/WONGUIConfig/ContainerConfig.h"

using namespace WONAPI;


//----------------------------------------------------------------------------------
// Init: Initialize the progress bar settings.
//----------------------------------------------------------------------------------
void ProgressBarComponent::Init(void)
{
	mStartPos = 0;
	mEndPos = 0;
	mCurPos = 0;

	mForeground = ColorScheme::GetColorRef(StandardColor_Hilight);
	mBackground = ColorScheme::GetColorRef(StandardColor_Back);
}

//----------------------------------------------------------------------------------
// Paint: Render the progress bar.
//----------------------------------------------------------------------------------
void ProgressBarComponent::Paint(Graphics &g)
{
	Component::Paint(g);

	int Wd = 0;
	if (mCurPos > mStartPos)
		Wd = (int)(((double)(mCurPos - mStartPos)) * (double)Width() / 
				   (double)(mEndPos - mStartPos));

	mBackground.Paint(g,0,0,Width(),Height());

	g.PushClipRect(0, 0, Wd, Height());
	mForeground.Paint(g,0,0,Width(),Height());
	g.PopClipRect();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ProgressComponentConfig::SetComponent(Component *theComponent)
{
	ComponentConfig::SetComponent(theComponent);
	m_pProgressBar = (ProgressBarComponent*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ProgressComponentConfig::HandleInstruction(const std::string& theInstruction)
{
	if(theInstruction=="BACKGROUND")
		m_pProgressBar->SetBackground(ReadBackground());
	else if(theInstruction=="FOREGROUND")
		m_pProgressBar->SetForeground(ReadBackground());
	else
		return ComponentConfig::HandleInstruction(theInstruction);

	return true;
}

