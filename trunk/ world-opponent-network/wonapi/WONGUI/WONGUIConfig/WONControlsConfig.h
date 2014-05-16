#ifndef __WON_WONCONTROLSCONFIG_H__
#define __WON_WONCONTROLSCONFIG_H__

#include "ComponentConfig.h"
#include "InputBoxConfig.h"
#include "ButtonConfig.h"
#include "ComboBoxConfig.h"
#include "ScrollbarConfig.h"
#include "ScrollAreaConfig.h"
#include "TabCtrlConfig.h"
#include "MultiListBoxConfig.h"

#include "WONGUI/WONControls.h"

namespace WONAPI
{

void WONControlsConfig_Init(ResourceConfigTable *theTable);
void WONControlsConfig_InitSkin(WONSkin *theSkin);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONSimpleButtonConfig : public ButtonConfig
{
protected:
	WONSimpleButton *mButton;

	virtual bool HandleInstruction(const std::string &theInstruction);
	void HandleButtonState();
	void HandleButtonStates();

public:
	virtual void SetComponent(Component *theComponent) { mButton = (WONSimpleButton*)theComponent; ButtonConfig::SetComponent(theComponent); }
	static ComponentConfig* CfgFactory() { return new WONSimpleButtonConfig; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONButtonConfig : public ButtonConfig
{
protected:
	WONButton *mButton;

	virtual bool HandleInstruction(const std::string &theInstruction);
	void HandleSetButtonState();

public:
	virtual void SetComponent(Component *theComponent) { mButton = (WONButton*)theComponent; ButtonConfig::SetComponent(theComponent); }
	static ComponentConfig* CfgFactory() { return new WONButtonConfig; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONComboBoxConfig : public ComboBoxConfig
{
protected:
	WONComboBox* mComboBox;
	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { mComboBox = (WONComboBox*)theComponent; ComboBoxConfig::SetComponent(theComponent); }
	static ComponentConfig* CfgFactory() { return new WONComboBoxConfig; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONTabCtrlConfig : public TabCtrlConfig
{
protected:
	WONTabCtrl *mTabCtrl;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { TabCtrlConfig::SetComponent(theComponent); mTabCtrl = (WONTabCtrl*)theComponent; }
	static ComponentConfig* CfgFactory() { return new WONTabCtrlConfig; }	
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONFrameComponentConfig : public ComponentConfig
{
protected:
	WONFrameComponent *mFrameComponent;

	virtual bool HandleInstruction(const std::string &theInstruction);
	virtual bool GetNamedIntValue(const std::string &theName, int &theValue);

public:
	virtual void SetComponent(Component *theComponent) { ComponentConfig::SetComponent(theComponent); mFrameComponent = (WONFrameComponent*)theComponent; }
	static ComponentConfig* CfgFactory() { return new WONFrameComponentConfig; }	
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONScrollbarConfig : public ScrollbarConfig
{
protected:
	WONScrollbar* mScrollbar;

public:
	virtual void SetComponent(Component *theComponent) { ScrollbarConfig::SetComponent(theComponent); mScrollbar = (WONScrollbar*)theComponent; }
	virtual bool HandleInstruction(const std::string &theInstruction);

	static ComponentConfig* CfgFactory() { return new WONScrollbarConfig; }	
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONScrollContainerConfig : public ScrollbarScrollerConfig
{
protected:
	WONScrollContainer* mScrollContainer;

	virtual bool GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, Component* &theComponent);

public:
	virtual void SetComponent(Component *theComponent) { ScrollbarScrollerConfig::SetComponent(theComponent); mScrollContainer = (WONScrollContainer*)theComponent; }
	virtual bool HandleInstruction(const std::string &theInstruction);
	static ComponentConfig* CfgFactory() { return new WONScrollContainerConfig; }	
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONHeaderControlConfig : public MultiListHeaderConfig
{
protected:
	WONHeaderControl *mHeader;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { MultiListHeaderConfig::SetComponent(theComponent); mHeader = (WONHeaderControl*)theComponent; }

	static ComponentConfig* CfgFactory() { return new WONHeaderControlConfig; }	
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONListCtrlConfig : public WONScrollContainerConfig
{
protected:
	WONListCtrl *mListCtrl;

	virtual bool GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, Component* &theComponent);
	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { WONScrollContainerConfig::SetComponent(theComponent); mListCtrl = (WONListCtrl*)theComponent; }

	static ComponentConfig* CfgFactory() { return new WONListCtrlConfig; }	
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONSkinConfig : public ComponentConfig
{
protected:
	virtual bool PreParse(ConfigParser &theParser);
	virtual bool HandleInstruction(const std::string &theInstruction);
	bool ReadVert();
	void HandleButtonClickSound();

public:

	static ComponentConfig* CfgFactory() { return new WONSkinConfig; }
};

};

#endif