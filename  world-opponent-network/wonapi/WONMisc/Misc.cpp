#define __WON_MASTER_CPP__
#include "DetectFirewallOp.cpp"
#include "FTPGetOp.cpp"

#ifdef WIN32_NOT_XBOX
#include "GameSpySupport.cpp"
#endif

#include "GetMOTDOp.cpp"
#include "GetHTTPDocumentOp.cpp"
#include "HTTPCache.cpp"
#include "HTTPEngine.cpp"
#include "HTTPGetOp.cpp"

#include "MediaMetrix.cpp"
#include "URL.cpp"

#include "PatchTypes.cpp"
#include "GetPatchListOp.cpp"
#include "CheckValidVersionOp.cpp"
#include "DNSTestOp.cpp"   
#include "ReportPatchStatusOp.cpp"

#include "PingManager.cpp"