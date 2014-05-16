#ifndef __WON_ScrollAreaCONFIG__
#define __WON_ScrollAreaCONFIG__
#include "ContainerConfig.h"
#include "WONGUI/ScrollArea.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ScrollAreaConfig : public ContainerConfig
{
protected:
	ScrollArea* mScrollArea;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new ScrollArea; }
	static ComponentConfig* CfgFactory() { return new ScrollAreaConfig; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ScrollerConfig : public ContainerConfig
{
protected:
	Scroller* mScroller;

public:
	virtual void SetComponent(Component *theComponent);
	virtual bool HandleInstruction(const std::string &theInstruction);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ButtonScrollerConfig : public ScrollerConfig
{
protected:
	ButtonScroller *mButtonScroller;
	ScrollButtonCondition GetScrollButtonCondition();

public:
	virtual void SetComponent(Component *theComponent) { ScrollerConfig::SetComponent(theComponent); mButtonScroller = (ButtonScroller*)theComponent; }
	virtual bool HandleInstruction(const std::string &theInstruction);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ScrollbarScrollerConfig : public ScrollerConfig
{
protected:
	ScrollbarScroller* mScrollbarScroller;

	virtual bool HandleInstruction(const std::string &theInstruction);
	ScrollbarCondition GetScrollbarCondition();

public:
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new ScrollbarScroller; }
	static ComponentConfig* CfgFactory() { return new ScrollbarScrollerConfig; }
};

}; // namespace WONAPI

#endif