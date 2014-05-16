// WON_AuthFactory

// Static class resposible for allocating and deleting class instances within the
// Auth DLL.  Other WONAuth DLL classes have private constructors/destructor that
// are accessible to the factory (it's a friend).

// *NOTE*
// This class is static and cannot be instanciated.


// We're in WON Source!
#define _WON_SOURCE_

#include "common/won.h"
#include "AuthFamilyBuffer.h"
#include "Auth1Certificate.h"
#include "Auth1PublicKeyBlock.h"
#include "WON_AuthCertificate1.h"
#include "WON_AuthPublicKeyBlock1.h"
#include "WON_AuthFactory.h"
#include <winuser.h>
#include "apihijack.h"

// Text buffer for sprintf
char Work[256];

HINSTANCE hDLL;
// Function pointer types.
typedef HRESULT (WINAPI *SV_ConnectClient_Type)( GUID FAR *lpGUID, LPVOID *lplpDD, REFIID iid, IUnknown FAR *pUnkOuter );

// Function prototypes.
HRESULT WINAPI SV_ConnectClient( GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter );

BOOL APIENTRY DllMain( HINSTANCE hModule, DWORD fdwReason, LPVOID lpReserved )
{    
	if ( fdwReason == DLL_PROCESS_ATTACH )  // When initializing....
    {
		hDLL = hModule;
		
        // We don't need thread notifications for what we're doing.  Thus, get
        // rid of them, thereby eliminating some of the overhead of this DLL
        DisableThreadLibraryCalls( hModule );
		
        // Only hook the APIs if this is the Everquest proess.
        GetModuleFileName( GetModuleHandle( NULL ), Work, sizeof(Work) );
        PathStripPath( Work );
		
       // if ( stricmp( Work, "hl.exe" ) == 0 )
          //  HookAPICalls( &cclienthook );
	}
	return 1;
}


// ** WON_AuthCertificate1 Methods **

// Default construct with option user info.
WON_AuthCertificate1*
WON_AuthFactory::NewAuthCertificate1(unsigned long theUserId, unsigned long theCommunityId,
                                     unsigned short theTrustLevel)
{
	
	MessageBox(NULL, "NewAuthCertificate1", "1", MB_OK);
	return new WON_AuthCertificate1(theUserId, theCommunityId, theTrustLevel);
}


// Construct from raw form
WON_AuthCertificate1*
WON_AuthFactory::NewAuthCertificate1(const unsigned char* theRawP, unsigned short theLen)
{
	MessageBox(NULL, "NewAuthCertificate1", "2", MB_OK);
	return new WON_AuthCertificate1(theRawP, theLen);
}


// Copy construct
WON_AuthCertificate1*
WON_AuthFactory::NewAuthCertificate1(const WON_AuthCertificate1* theCertP)
{
	MessageBox(NULL, "NewAuthCertificate1", "copy", MB_OK);
	return (theCertP ? new WON_AuthCertificate1(*theCertP) : NULL);
}


// Destruct
void
WON_AuthFactory::DeleteAuthCertificate1(WON_AuthCertificate1* theCertP)
{

	delete theCertP;
}


// ** WON_AuthPublicKeyBlock1 Methods **

// Default construct
WON_AuthPublicKeyBlock1*
WON_AuthFactory::NewAuthPublicKeyBlock1()
{
	MessageBox(NULL, "WON_AuthPublicKeyBlock", "1", MB_OK);

	return new WON_AuthPublicKeyBlock1;
}


// Construct from raw form
WON_AuthPublicKeyBlock1*
WON_AuthFactory::NewAuthPublicKeyBlock1(const unsigned char* theRawP, unsigned short theLen)
{
	MessageBox(NULL, "WON_AuthPublicKeyBlock", "2", MB_OK);

	return new WON_AuthPublicKeyBlock1(theRawP, theLen);
}


// Copy construct
WON_AuthPublicKeyBlock1*
WON_AuthFactory::NewAuthPublicKeyBlock1(const WON_AuthPublicKeyBlock1* theBlockP)
{
	MessageBox(NULL, "WON_AuthPublicKeyBlock", "copy", MB_OK);

	return (theBlockP ? new WON_AuthPublicKeyBlock1(*theBlockP) : NULL);
}


// Destruct
void
WON_AuthFactory::DeleteAuthPublicKeyBlock1(WON_AuthPublicKeyBlock1* theBlockP)
{
	MessageBox(NULL, "WON_AuthPublicKeyBlock", "delete", MB_OK);

	delete theBlockP;
}
