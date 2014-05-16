#include "ChooseNetworkAdapterDialog.h"
#include "LobbyTypes.h"

#include "WONGUI/Button.h"
#include "WONGUI/GUIConfig.h"
#include "WONGUI/MultiListBox.h"
#include "WONSocket/IPAddr.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class NetworkAdapterListItem : public MultiListItem
{
public:
	long mHost;

	NetworkAdapterListItem(long theHost) : mHost(theHost) { }

	virtual int Compare(const ListItem *theItem, ListArea *theListArea) const;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int NetworkAdapterListItem::Compare(const ListItem *theItem, ListArea *theListArea) const
{
	NetworkAdapterListItem* aNetItem = (NetworkAdapterListItem*)theItem;
	if (mHost == aNetItem->mHost)
		return 0;
	else if (mHost == INADDR_ANY)
		return -1;
	else if (aNetItem->mHost == INADDR_ANY)
		return 1;
	else
		return MultiListItem::Compare(theItem, theListArea);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChooseNetworkAdapterDialog::ChooseNetworkAdapterDialog()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChooseNetworkAdapterDialog::~ChooseNetworkAdapterDialog()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChooseNetworkAdapterDialog::InitComponent(ComponentInit &theInit)
{
	Dialog::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mAdapterList,"AdapterList");
		WONComponentConfig_Get(aConfig,mActivateButton,"ActivateButton");
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChooseNetworkAdapterDialog::Reset()
{
	mAdapterList->BeginMultiChange();
	
	mAdapterList->Clear(false);
	
	mAdapterList->InsertItem(new NetworkAdapterListItem(INADDR_ANY));
	mAdapterList->SetString(0,0,ChooseNetworkAdapterDialog_Default_String);
	if(IPAddr::GetLocalHost()==INADDR_ANY)
		mAdapterList->SetSelItem(0,false);

	IPAddr::NetworkAdapterList aList;
	IPAddr::GetNetworkAdapterList(aList,true);
	IPAddr::NetworkAdapterList::iterator anItr;
	for(anItr = aList.begin(); anItr != aList.end(); ++anItr)
	{
		IPAddr::NetworkAdapter &anAdapter = *anItr;
		IPAddr anAddr(anAdapter.mHost,0);
		GUIString aName = anAdapter.mName;
		if(aName.empty())
			aName = ChooseNetworkAdapterDialog_Unknown_String;

		mAdapterList->InsertItem(new NetworkAdapterListItem(anAdapter.mHost));
		mAdapterList->SetString(mAdapterList->GetNumItems()-1,0,aName);	
		mAdapterList->SetString(mAdapterList->GetNumItems()-1,1,anAddr.GetHostString());
		if(IPAddr::GetLocalHost()==anAdapter.mHost)
			mAdapterList->SetSelItem(mAdapterList->GetNumItems()-1);
	}

	mAdapterList->EndMultiChange();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
long ChooseNetworkAdapterDialog::GetSelHost()
{
	NetworkAdapterListItem *anItem = (NetworkAdapterListItem*)mAdapterList->GetSelItem();
	if(anItem==NULL)
		return IPAddr::GetLocalHost();
	else
		return anItem->mHost;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChooseNetworkAdapterDialog::HandleComponentEvent(ComponentEvent *theEvent)
{
	Dialog::HandleComponentEvent(theEvent);
}

