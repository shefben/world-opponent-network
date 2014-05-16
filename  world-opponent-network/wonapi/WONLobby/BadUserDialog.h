#ifndef __BADUSERDIALOG_H__
#define __BADUSERDIALOG_H__


#include "WONGUI/Dialog.h"

namespace WONAPI
{
struct RoutingBadUser;
typedef std::list<RoutingBadUser> RoutingBadUserList;

WON_PTR_FORWARD(ListArea);
WON_PTR_FORWARD(MultiListArea);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class BadUserDialog : public Dialog
{
protected:
	MultiListAreaPtr mUserList;
	ListAreaPtr mSimpleUserList;
	ComponentPtr mUserListBox;
	ComponentPtr mSimpleUserListBox;
	bool mIsSimple;
	
public:
	typedef std::list<unsigned long> IdList;

public:
	BadUserDialog();
	virtual void InitComponent(ComponentInit &theInit);

	void SetUserList(const RoutingBadUserList &theList, bool isSimple);

	void GetIds(IdList &theList);
};
typedef SmartPtr<BadUserDialog> BadUserDialogPtr;

}; // namespace WONAPI

#endif