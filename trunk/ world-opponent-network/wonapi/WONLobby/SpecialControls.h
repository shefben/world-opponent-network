#ifndef __WON_SPECIALCONTROLS_H__
#define __WON_SPECIALCONTROLS_H__

#include "WONGUI/MSControls.h"
#include "WONGUI/ImageButton.h"
#include "WONGUI/WONGUIConfig/MSComponentConfig.h"
#include "WONGUI/WONGUIConfig/ImageButtonConfig.h"


namespace WONAPI
{
class SetCrossPromotionEvent;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ImageTextButton : public ImageButton
{
public:
	FontPtr mFont;
	GUIString mText;
	GUIString mBrowseLocation;

	ImageTextButton(const GUIString &theText = "");
	virtual void Paint(Graphics &g);
	virtual bool ActivateHook();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ImageTextButtonConfig : public ImageButtonConfig
{
protected:
	ImageTextButton *mButton;
	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { ImageButtonConfig::SetComponent(theComponent); mButton = (ImageTextButton*)theComponent; }

	static ComponentConfig* CfgFactory() { return new ImageTextButtonConfig; }
	static Component* CompFactory() { return new ImageTextButton; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MaxSplitterButton : public MSArrowButton
{
public:
	MaxSplitterButton(Direction theDirection = Direction_Up);
	virtual void Paint(Graphics &g);
};
typedef SmartPtr<MaxSplitterButton> MaxSplitterButtonPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MaxSplitterButtonConfig : public MSArrowButtonConfig
{
protected:
	MaxSplitterButton *mButton;
	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { MSArrowButtonConfig::SetComponent(theComponent); mButton = (MaxSplitterButton*)theComponent; }

	static ComponentConfig* CfgFactory() { return new MaxSplitterButtonConfig; }
	static Component* CompFactory() { return new MaxSplitterButton; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class CrossPromotionButton : public ImageTextButton
{
protected:
	ImagePtr mDefaultImage;
	GUIString mDefaultBrowseLocation;

public:
	CrossPromotionButton();
	void HandleSetCrossPromotionEvent(SetCrossPromotionEvent *theEvent);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);

	void SetDefaultImage(Image* theImage);
	void SetDefaultLink(GUIString& theString)	{ mDefaultBrowseLocation = theString; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class CrossPromotionButtonConfig : public ImageButtonConfig
{
protected:
	CrossPromotionButton *mButton;
	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { ImageButtonConfig::SetComponent(theComponent); mButton = (CrossPromotionButton*)theComponent; }

	static ComponentConfig* CfgFactory() { return new CrossPromotionButtonConfig; }
	static Component* CompFactory() { return new CrossPromotionButton; }
};

}; // namespace WONAPI

#endif