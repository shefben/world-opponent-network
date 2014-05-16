#include "MultiListBoxConfig.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiListAreaConfig::SetComponent(Component *theComponent)
{
	ListAreaConfig::SetComponent(theComponent);
	mMultiListArea = (MultiListArea*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MultiListAreaConfig::HandleInstruction(const std::string& theInstruction)
{
	if(theInstruction=="SORTCOLUMN")
	{
		int aCol = ReadInt(); EnsureComma();
		ListSortType aType = ReadSortType();
		mMultiListArea->SetSortColumn(aCol,aType);
	}
	else if(theInstruction=="COLUMNWIDTH")
	{
		int aCol = ReadInt(); EnsureComma();
		int aWidth = ReadInt();
		mMultiListArea->SetColumnWidth(aCol,aWidth);
	}
	else if(theInstruction=="NUMCOLUMNS")
		mMultiListArea->SetNumColumns(ReadInt());
	else if(theInstruction=="DYNAMICSIZECOLUMN")
		mMultiListArea->SetDynamicSizeColumn(ReadInt());
	else if(theInstruction=="SORTCOLUMNFUNC")
	{
		int aColumn = ReadInt(); EnsureComma();
		GUIString aName = ReadString();
		ListArea::SortFunc aFunc = NULL;
		if(aName.compareNoCase("NUMERIC")==0)
			aFunc = ListArea::NumberSortFunc;
		else
			throw ConfigObjectException("Unknown sort func: " + (std::string)aName);
		
		mMultiListArea->SetSortColumnFunc(aColumn,aFunc);
	}
	else if(theInstruction=="AUTOGROWCOLUMNS")
		mMultiListArea->SetAutoGrowColumns(ReadBool());
	else if(theInstruction=="COLUMNHALIGN")
	{
		int aCol = ReadInt(); EnsureComma();
		HorzAlign anAlign = ReadHorzAlign();
		mMultiListArea->SetColumnHAlign(aCol,anAlign);
	}
	else
		return ListAreaConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MultiListHeaderConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="SETCOLUMN")
	{
		int aColumn = ReadInt(); EnsureComma();
		Component *aComponent = ReadComponent();
		mHeader->SetColumnItem(aColumn,aComponent);
	}
	else if(theInstruction=="MULTILISTAREA")
	{
		MultiListArea *aList = SafeConfigGetComponent<MultiListArea*>(ReadComponent(),"MultiListArea");
		mHeader->SetMultiListArea(aList);
	}
	else
		return ContainerConfig::HandleInstruction(theInstruction);

	return true;
}