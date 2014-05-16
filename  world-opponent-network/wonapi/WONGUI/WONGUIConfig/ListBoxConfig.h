#ifndef __WON_LISTBOXCONFIG__
#define __WON_LISTBOXCONFIG__
#include "ScrollAreaConfig.h"
#include "WONGUI/ListBox.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ListAreaConfig : public ScrollAreaConfig
{
protected:
	ListArea* mListArea;

	virtual bool HandleInstruction(const std::string &theInstruction);
	ListSortType ReadSortType();

public:
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new ListArea; }
	static ComponentConfig* CfgFactory() { return new ListAreaConfig; }

	static void ReadListFlags(ComponentConfig *theConfig, ListArea *theListArea);
};

}; // namespace WONAPI

#endif