#include "WONNetCommInterface.h"
#include "AdInterface.h"

using namespace WONAPI;

AdInterface::AdInterface() :
	AdApi(),
	mNetCommInterfaceP(NULL)
{
	mNetCommInterfaceP = new WONNetCommInterface();
}

AdInterface::~AdInterface()
{
	delete mNetCommInterfaceP;
	mNetCommInterfaceP = NULL;
}
	
AdApi::AdResult 
AdInterface::Init(	const char* theProductName,				
					const char* theCachePath,
					AdApi::AdLocal theLocal)
{
	
	return AdApi::Init(theProductName, mNetCommInterfaceP, theCachePath, theLocal);
}


AdApi::AdResult 
AdInterface::Shutdown()
{
	return AdApi::Shutdown();
}