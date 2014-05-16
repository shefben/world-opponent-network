#ifndef _ADINTERFACE_H
#define _ADINTERFACE_H
#include "WONShared.h"

#include "AdApi/AdApi.h"

namespace WONAPI
{

// This class is the interface to the AdApi for the WONAPI
class AdInterface : public AdApi
{
// Public Methods
public:
	AdInterface();
	virtual ~AdInterface();
	
	// The public interface to the ad API

	// Initilizes the Ad API
	virtual AdResult Init(	const char* theProductName,				// Name of product to display ads for - Note: Product names should not contain any spaces or any special charters
							const char* theCachePath,				// Sets path in which to put add cache - Set to null to use default
							AdApi::AdLocal theLocal = AdApi::AdLocal_US);	// Localized country

	// Shuts down the Ad API
	virtual AdResult Shutdown();

// Member data
private:
	NetCommInterface* mNetCommInterfaceP;
};

}

#endif // _ADINTERFACE_H