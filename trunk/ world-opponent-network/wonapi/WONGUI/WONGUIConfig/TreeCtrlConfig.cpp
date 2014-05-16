#include "TreeCtrlConfig.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TreeAreaConfig::HandleInstruction(const std::string& theInstruction)
{
	if(theInstruction=="LISTSORTTYPE")
		mTreeArea->SetSortType(ReadSortType());
	else
		return ListAreaConfig::HandleInstruction(theInstruction);
	
	return true;
}

