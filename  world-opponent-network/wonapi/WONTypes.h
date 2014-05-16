#ifndef __WONTYPES_H__
#define __WONTYPES_H__
#include "WONShared.h"

#include <ostream>
#include <list>

namespace WONAPI
{

typedef std::basic_string<unsigned char> RawBuffer;

}; // namespace WONAPI


namespace WONTypes
{

typedef std::list<bool> BoolList;
typedef std::list<int> IntList;
typedef std::list<std::string> StringList;
typedef std::list<std::wstring> WStringList;

typedef std::list<BoolList> BoolListList;
typedef std::list<IntList> IntListList;
typedef std::list<StringList> StringListList;
typedef std::list<WStringList> WStringListList;

}; // namespace WONTypes


// Output operators
std::ostream& operator<<(std::ostream& os, const WONTypes::BoolList& aList);
std::ostream& operator<<(std::ostream& os, const WONTypes::IntList& aList);
std::ostream& operator<<(std::ostream& os, const WONTypes::StringList& aList);
std::ostream& operator<<(std::ostream& os, const WONTypes::WStringList& aList);
std::ostream& operator<<(std::ostream& os, const WONTypes::BoolListList& aList);
std::ostream& operator<<(std::ostream& os, const WONTypes::IntListList& aList);
std::ostream& operator<<(std::ostream& os, const WONTypes::StringListList& aList);
std::ostream& operator<<(std::ostream& os, const WONTypes::WStringListList& aList);

#endif
