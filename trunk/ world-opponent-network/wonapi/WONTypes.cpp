#define __WON_MASTER_CPP__

#include "WONTypes.h"
#include <iostream>
#include "WONCommon/StringUtil.h"

namespace {
	using std::ostream;
	using namespace WONTypes;
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ostream& operator<<(ostream& os, const BoolList& aList)
{
	BoolList::const_iterator anItr = aList.begin();
	if (anItr != aList.end())
	{
		os << (*anItr ? "true" : "false");
		for (++anItr; anItr != aList.end(); ++anItr)
			os << ',' << (*anItr ? "true" : "false");
	}
	return os;
}

ostream& operator<<(ostream& os, const IntList& aList)
{
	IntList::const_iterator anItr = aList.begin();
	if (anItr != aList.end())
	{
		os << *anItr;
		for (++anItr; anItr != aList.end(); ++anItr)
			os << ',' << *anItr;
	}
	return os;
}

ostream& operator<<(std::ostream& os, const StringList& aList)
{
	StringList::const_iterator anItr = aList.begin();
	if (anItr != aList.end())
	{
		os << *anItr;
		for (++anItr; anItr != aList.end(); ++anItr)
			os << ',' << *anItr;
	}
	return os;
}

ostream& operator<<(std::ostream& os, const WStringList& aList)
{
	WStringList::const_iterator anItr = aList.begin();
	if (anItr != aList.end())
	{
		os << WONAPI::WStringToString(*anItr);
		for (++anItr; anItr != aList.end(); ++anItr)
			os << ',' << WONAPI::WStringToString(*anItr);
	}
	return os;
}

ostream& operator<<(ostream& os, const BoolListList& aList)
{
	BoolListList::const_iterator anItr = aList.begin();
	if (anItr != aList.end())
	{
		os << '(' << *anItr << ')';
		for (++anItr; anItr != aList.end(); ++anItr)
			os << ",(" << *anItr << ')';
	}
	return os;
}

ostream& operator<<(ostream& os, const IntListList& aList)
{
	IntListList::const_iterator anItr = aList.begin();
	if (anItr != aList.end())
	{
		os << '(' << *anItr << ')';
		for (++anItr; anItr != aList.end(); ++anItr)
			os << ",(" << *anItr << ')';
	}
	return os;
}

ostream& operator<<(ostream& os, const StringListList& aList)
{
	StringListList::const_iterator anItr = aList.begin();
	if (anItr != aList.end())
	{
		os << '(' << *anItr << ')';
		for (++anItr; anItr != aList.end(); ++anItr)
			os << ",(" << *anItr << ')';
	}
	return os;
}

ostream& operator<<(ostream& os, const WStringListList& aList)
{
	WStringListList::const_iterator anItr = aList.begin();
	if (anItr != aList.end())
	{
		os << '(' << *anItr << ')';
		for (++anItr; anItr != aList.end(); ++anItr)
			os << ",(" << *anItr << ')';
	}
	return os;
}
