#ifndef __WON_MSCOMPONENTCONFIG_H__
#define __WON_MSCOMPONENTCONFIG_H__

#include "ComponentConfig.h"
#include "InputBoxConfig.h"
#include "ButtonConfig.h"
#include "ComboBoxConfig.h"
#include "ScrollbarConfig.h"
#include "ScrollAreaConfig.h"
#include "TabCtrlConfig.h"
#include "MultiListBoxConfig.h"

#include "WONGUI/MSControls.h"

namespace WONAPI
{

void MSComponentConfig_Init(ResourceConfigTable *theTable);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSButtonConfig : public ButtonConfig
{
protected:
	MSButtonBase* mButton;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	MSButtonConfig();
	virtual void SetComponent(Component *theComponent);

	static Component* ButtonFactory() { return new MSButton; }
	static Component* CheckboxFactory() { return new MSCheckbox; }
	static Component* RadioFactory() { return new MSRadioButton; }
	static Component* LabelFactory() { return new MSLabel; }
	static ComponentConfig* CfgFactory() { return new MSButtonConfig; }

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSInputBoxConfig : public InputBoxConfig
{
public:
	static Component* CompFactory() { return new MSInputBox; }
	static ComponentConfig* CfgFactory() { return new MSInputBoxConfig; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSComboBoxConfig : public ComboBoxConfig
{
protected:
	MSComboBox* mComboBox;
	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { mComboBox = (MSComboBox*)theComponent; ComboBoxConfig::SetComponent(theComponent); }

	static Component* CompFactory() { return new MSComboBox; }
	static ComponentConfig* CfgFactory() { return new MSComboBoxConfig; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSArrowButtonConfig : public ButtonConfig
{
protected:
	MSArrowButton* mArrowButton;

public:
	virtual void SetComponent(Component *theComponent);
	virtual bool HandleInstruction(const std::string &theInstruction);

	static Component* CompFactory() { return new MSArrowButton; }
	static ComponentConfig* CfgFactory() { return new MSArrowButtonConfig; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSCloseButtonConfig : public ButtonConfig
{
public:
	static Component* CompFactory() { return new MSCloseButton; }
	static ComponentConfig* CfgFactory() { return new MSCloseButtonConfig; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSTabButtonConfig : public MSButtonConfig
{
protected:
	MSTabButton* mTabButton;

public:
	virtual void SetComponent(Component *theComponent);
	virtual bool HandleInstruction(const std::string &theInstruction);

	static Component* CompFactory() { return new MSTabButton; }
	static ComponentConfig* CfgFactory() { return new MSTabButtonConfig; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSHeaderButtonConfig : public MSButtonConfig
{
protected:
	MSHeaderButton *mHeaderButton;

public:
	virtual void SetComponent(Component *theComponent) { mHeaderButton = (MSHeaderButton*)theComponent; MSButtonConfig::SetComponent(theComponent); }

	static Component* CompFactory() { return new MSHeaderButton; }
	static ComponentConfig* CfgFactory() { return new MSHeaderButtonConfig; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSScrollbarConfig : public ScrollbarConfig
{
protected:
	MSScrollbar* mScrollbar;

public:
	virtual void SetComponent(Component *theComponent);
	virtual bool HandleInstruction(const std::string &theInstruction);

	static Component* CompFactory() { return new MSScrollbar; }
	static ComponentConfig* CfgFactory() { return new MSScrollbarConfig; }	
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSSeperatorConfig : public ComponentConfig
{
public:
	static Component* CompFactory() { return new MSSeperator; }
	static ComponentConfig* CfgFactory() { return new MSSeperatorConfig; }	
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSDlgBackConfig : public ComponentConfig
{
public:
	static Component* CompFactory() { return new MSDlgBack; }
	static ComponentConfig* CfgFactory() { return new MSDlgBackConfig; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MS3DFrameConfig : public ComponentConfig
{
protected:
	MS3DFrame* mFrame;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new MS3DFrame; }
	static ComponentConfig* CfgFactory() { return new MS3DFrameConfig; }	
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSEtchedFrameConfig : public ComponentConfig
{
public:
	static Component* CompFactory() { return new MSEtchedFrame; }
	static ComponentConfig* CfgFactory() { return new MSEtchedFrameConfig; }	
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSEtchedFrameGroupConfig : public ContainerConfig
{
protected:
	MSEtchedFrameGroup* mFrame;

	virtual bool GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, Component* &theComponent);
	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new MSEtchedFrameGroup; }
	static ComponentConfig* CfgFactory() { return new MSEtchedFrameGroupConfig; }	
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSTabCtrlConfig : public TabCtrlConfig
{
protected:
	MSTabCtrl *mTabCtrl;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { TabCtrlConfig::SetComponent(theComponent); mTabCtrl = (MSTabCtrl*)theComponent; }

	static Component* CompFactory() { return new MSTabCtrl; }
	static ComponentConfig* CfgFactory() { return new MSTabCtrlConfig; }	
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSScrollContainerConfig : public ScrollbarScrollerConfig
{
protected:
	MSScrollContainer* mScrollContainer;

	virtual bool GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, Component* &theComponent);

public:
	virtual void SetComponent(Component *theComponent);
	virtual bool HandleInstruction(const std::string &theInstruction);

	static Component* CompFactory() { return new MSScrollContainer; }
	static ComponentConfig* CfgFactory() { return new MSScrollContainerConfig; }	
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSHeaderControlConfig : public MultiListHeaderConfig
{
protected:
	MSHeaderControl *mHeader;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { mHeader = (MSHeaderControl*)theComponent; MultiListHeaderConfig::SetComponent(theComponent); }

	static Component* CompFactory() { return new MSHeaderControl; }
	static ComponentConfig* CfgFactory() { return new MSHeaderControlConfig; }	
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSListCtrlConfig : public MSScrollContainerConfig
{
protected:
	MSListCtrl *mListCtrl;

	virtual bool GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, Component* &theComponent);
	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { mListCtrl = (MSListCtrl*)theComponent; MSScrollContainerConfig::SetComponent(theComponent); }

	static Component* CompFactory() { return new MSListCtrl; }
	static ComponentConfig* CfgFactory() { return new MSListCtrlConfig; }	

};


} // namespace WONAPI

#endif