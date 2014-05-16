#include "IPAddr.h"

using namespace std;
using namespace WONAPI;

long IPAddr::mLocalHost      = INADDR_ANY;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

IPAddr::IPAddr()
{
	memset(&mAddr,0,sizeof(mAddr));
	mIsValid = false;
	mRememberHostString = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
IPAddr::IPAddr(const SOCKADDR_IN &theAddr)
{
	mRememberHostString = false;
	Set(theAddr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
IPAddr::IPAddr(const std::string &theHost, unsigned short thePort)
{
	mRememberHostString = false;
	Set(theHost,thePort);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
IPAddr::IPAddr(const std::string &theHostAndPort)
{
	mRememberHostString = false;
	Set(theHostAndPort);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

IPAddr::IPAddr(const char* theHostAndPort)
{
	mRememberHostString = false;
	Set(theHostAndPort);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
IPAddr::IPAddr(long theHost, unsigned short thePort)
{
	mRememberHostString = false;
	Set(theHost,thePort);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
IPAddr IPAddr::GetLocalAddr()
{
	if(mLocalHost!=INADDR_ANY) // see if user has selected a particular network adapter
		return IPAddr(mLocalHost,0);

#ifdef _XBOX
	return IPAddr();
#else
	char aBuf[1024];
	int aVal = gethostname(aBuf,1024);
	if(aVal==0)
		return IPAddr(aBuf,0);
	else
		return IPAddr();
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
IPAddr IPAddr::GetBroadcastAddr(int thePort)
{
	return IPAddr(0xffffffff,thePort);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool IPAddr::Set(const std::string &theHost, unsigned short thePort)
{
	memset(&mAddr,0,sizeof(mAddr));
	mIsValid = false;

	unsigned long aHost = inet_addr(theHost.c_str());

	if(aHost==INADDR_NONE) 
	{
#ifdef _XBOX
		return false;
#else
		HOSTENT *aHostEnt = gethostbyname(theHost.c_str());
		if(aHostEnt==NULL) 
			return false;
		else
			memcpy(&aHost,aHostEnt->h_addr_list[0],4);
#endif
	}

	if(this->mRememberHostString)
		mHostString = theHost;
	else
		mHostString.erase();

	mAddr.sin_family = AF_INET;
	mAddr.sin_addr.s_addr = aHost;
	mAddr.sin_port = htons(thePort);
	mIsValid = true;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool IPAddr::Set(const std::string &theHostAndPort)
{
	int aColonIdx = theHostAndPort.find(':');
	if(aColonIdx==string::npos)
	{
		mIsValid = false;
		return false;
	}

	return Set(theHostAndPort.substr(0,aColonIdx),atoi(theHostAndPort.substr(aColonIdx+1).c_str()));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const char* IPAddr::GetSixByte() const
{
	return (const char*)&(mAddr.sin_port);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void IPAddr::SetSixByte(const void* theBuf)
{
	memset(&mAddr,0,sizeof(mAddr));
	mAddr.sin_family = AF_INET;
	memcpy(&mAddr.sin_port,theBuf,6);	
	mHostString.erase();
	mIsValid = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void IPAddr::Set(const SOCKADDR_IN &theAddr)
{
	mAddr = theAddr;
	mHostString.erase();
	mIsValid = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void IPAddr::Set(long theHost, unsigned short thePort)
{
	memset(&mAddr,0,sizeof(mAddr));
	mAddr.sin_family = AF_INET;
	mAddr.sin_addr.s_addr = htonl(theHost);
	mAddr.sin_port = htons(thePort);
	mHostString.erase();
	mIsValid = true;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool IPAddr::SetWithDefaultPort(const std::string &theHostAndPort, unsigned short theDefaultPort)
{
	int aColonIdx = theHostAndPort.find(':');
	if(aColonIdx==string::npos)
		return Set(theHostAndPort,theDefaultPort);
	else
		return Set(theHostAndPort);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool IPAddr::SetWithDefaultPortNoDNS(const std::string &theHostAndPort, unsigned short theDefaultPort)
{
	mIsValid = false;

	int aColonIdx = theHostAndPort.find(':');
	unsigned long aHost;
	unsigned short aPort;
	if(aColonIdx!=string::npos)
	{
		aHost = inet_addr(theHostAndPort.substr(0,aColonIdx).c_str());
		aPort = atoi(theHostAndPort.substr(aColonIdx+1).c_str());
	}
	else
	{
		aHost = inet_addr(theHostAndPort.c_str());
		aPort = theDefaultPort;
	}

	if(aHost!=INADDR_NONE)
	{
		memset(&mAddr,0,sizeof(mAddr));
		mAddr.sin_family = AF_INET;
		mAddr.sin_addr.s_addr = aHost;
		mAddr.sin_port = htons(aPort);
		mHostString.erase();
		mIsValid = true;	
	}

	return mIsValid;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void IPAddr::SetThePort(unsigned short thePort)
{
	mAddr.sin_port = htons(thePort);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
long IPAddr::GetHost() const
{
	return ntohl(mAddr.sin_addr.s_addr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
string IPAddr::GetHostString(bool useOriginal) const
{
	if(useOriginal && !mHostString.empty())
		return mHostString;
	
#ifdef _XBOX
	char aBuf[50];
	sprintf(aBuf,"%d.%d.%d.%d",mAddr.sin_addr.S_un.S_un_b.s_b1,mAddr.sin_addr.S_un.S_un_b.s_b2,mAddr.sin_addr.S_un.S_un_b.s_b3,mAddr.sin_addr.S_un.S_un_b.s_b4);
	return aBuf;
#else	
	return inet_ntoa(mAddr.sin_addr);
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string IPAddr::GetHostAndPortString(bool useOriginal) const
{
	long aHost = ntohl(GetHost());
	string aHostStr;
	unsigned char *p = (unsigned char*)&aHost;

	char aBuf[50];
	sprintf(aBuf,"%s:%d",GetHostString(useOriginal).c_str(),GetPort());
	return aBuf;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned short IPAddr::GetPort() const
{
	return ntohs(mAddr.sin_port);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool IPAddr::IsValid() const
{
	return mIsValid;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

const SOCKADDR_IN& IPAddr::GetSockAddrIn() const
{
	return mAddr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool IPAddr::operator <(const IPAddr& theAddr) const
{
	return memcmp(&(mAddr.sin_port),&(theAddr.mAddr.sin_port),6)<0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool IPAddr::operator <=(const IPAddr& theAddr) const
{
	return memcmp(&(mAddr.sin_port),&(theAddr.mAddr.sin_port),6)<=0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool IPAddr::operator >(const IPAddr& theAddr) const
{
	return memcmp(&(mAddr.sin_port),&(theAddr.mAddr.sin_port),6)>0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool IPAddr::operator >=(const IPAddr& theAddr) const
{
	return memcmp(&(mAddr.sin_port),&(theAddr.mAddr.sin_port),6)>=0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool IPAddr::operator ==(const IPAddr& theAddr) const
{
	return memcmp(&(mAddr.sin_port),&(theAddr.mAddr.sin_port),6)==0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool IPAddr::operator !=(const IPAddr& theAddr) const
{
	return memcmp(&(mAddr.sin_port),&(theAddr.mAddr.sin_port),6)!=0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int IPAddr::Compare(const IPAddr &theAddr) const
{
	return memcmp(&(mAddr.sin_port),&(theAddr.mAddr.sin_port),6);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int IPAddr::StringCompare(const IPAddr &theAddr) const
{
	int aResult = GetHost() - theAddr.GetHost();
	if(aResult!=0)
		return aResult;
	else
		return GetPort() - theAddr.GetPort();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#ifndef IP_TYPES_INCLUDED // most people don't have iphlpapi.h 

// Definitions and structures used by getnetworkparams and getadaptersinfo apis

#define MAX_ADAPTER_DESCRIPTION_LENGTH  128 // arb.
#define MAX_ADAPTER_NAME_LENGTH         256 // arb.
#define MAX_ADAPTER_ADDRESS_LENGTH      8   // arb.
#define DEFAULT_MINIMUM_ENTITIES        32  // arb.
#define MAX_HOSTNAME_LEN                128 // arb.
#define MAX_DOMAIN_NAME_LEN             128 // arb.
#define MAX_SCOPE_ID_LEN                256 // arb.

//
// types
//

// Node Type

#define BROADCAST_NODETYPE              1
#define PEER_TO_PEER_NODETYPE           2
#define MIXED_NODETYPE                  4
#define HYBRID_NODETYPE                 8

// Adapter Type

#define IF_OTHER_ADAPTERTYPE            0
#define IF_ETHERNET_ADAPTERTYPE         1
#define IF_TOKEN_RING_ADAPTERTYPE       2
#define IF_FDDI_ADAPTERTYPE             3
#define IF_PPP_ADAPTERTYPE              4
#define IF_LOOPBACK_ADAPTERTYPE         5
#define IF_SLIP_ADAPTERTYPE             6

//
// IP_ADDRESS_STRING - store an IP address as a dotted decimal string
//

typedef struct {
    char String[4 * 4];
} IP_ADDRESS_STRING, *PIP_ADDRESS_STRING, IP_MASK_STRING, *PIP_MASK_STRING;

//
// IP_ADDR_STRING - store an IP address with its corresponding subnet mask,
// both as dotted decimal strings
//

typedef struct _IP_ADDR_STRING {
    struct _IP_ADDR_STRING* Next;
    IP_ADDRESS_STRING IpAddress;
    IP_MASK_STRING IpMask;
    DWORD Context;
} IP_ADDR_STRING, *PIP_ADDR_STRING;

//
// ADAPTER_INFO - per-adapter information. All IP addresses are stored as
// strings
//

typedef struct _IP_ADAPTER_INFO {
    struct _IP_ADAPTER_INFO* Next;
    DWORD ComboIndex;
    char AdapterName[MAX_ADAPTER_NAME_LENGTH + 4];
    char Description[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
    UINT AddressLength;
    BYTE Address[MAX_ADAPTER_ADDRESS_LENGTH];
    DWORD Index;
    UINT Type;
    UINT DhcpEnabled;
    PIP_ADDR_STRING CurrentIpAddress;
    IP_ADDR_STRING IpAddressList;
    IP_ADDR_STRING GatewayList;
    IP_ADDR_STRING DhcpServer;
    BOOL HaveWins;
    IP_ADDR_STRING PrimaryWinsServer;
    IP_ADDR_STRING SecondaryWinsServer;
    time_t LeaseObtained;
    time_t LeaseExpires;
} IP_ADAPTER_INFO, *PIP_ADAPTER_INFO;

#endif // IP_TYPES_INCLUDED

#endif // WIN32

static bool IPAddr_PlatformGetNetworkAdapterList(IPAddr::NetworkAdapterList &theList)
{
#ifdef WIN32_NOT_XBOX
	typedef DWORD(WINAPI *GetAdaptersInfoFunc)(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen);
	static GetAdaptersInfoFunc aGetAdaptersInfoFunc = NULL;
	static bool haveLoaded = false;
	struct AutoUnloadLibrary
	{
		HMODULE mModule;

		AutoUnloadLibrary() { mModule = NULL; }
		~AutoUnloadLibrary() { if(mModule!=NULL) FreeLibrary(mModule); }

	};
	static AutoUnloadLibrary anUnload;

	// Try to dynamically load the function
	if(!haveLoaded)
	{
		HMODULE aModule = LoadLibrary("Iphlpapi.dll");
		if(aModule!=NULL)
		{
			anUnload.mModule = aModule;
			aGetAdaptersInfoFunc  = (GetAdaptersInfoFunc)GetProcAddress(aModule,"GetAdaptersInfo");
		}

		haveLoaded = true;	
	}

	if(aGetAdaptersInfoFunc==NULL)
		return false;

	DWORD aBufSize = 0;
	aGetAdaptersInfoFunc(NULL,&aBufSize);
	if(aBufSize==0 || aBufSize>256000) // sanity check
		return false;

	char *aBuf = new char[aBufSize];
	auto_ptr<char> aDelBuf(aBuf);
	PIP_ADAPTER_INFO anAdapterP = (PIP_ADAPTER_INFO)aBuf;
	aGetAdaptersInfoFunc(anAdapterP,&aBufSize);
	while(anAdapterP!=NULL)
	{
		theList.push_back(IPAddr::NetworkAdapter(anAdapterP->Description,ntohl(inet_addr(anAdapterP->IpAddressList.IpAddress.String))));
		anAdapterP = anAdapterP->Next;
	}
	return true;

#endif

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void IPAddr::GetNetworkAdapterList(NetworkAdapterList &theList, bool getDetails)
{
	theList.clear();

#ifdef _XBOX
	return;
#else
	if(getDetails)
	{
		if(IPAddr_PlatformGetNetworkAdapterList(theList) && !theList.empty())
			return;
	}

	char aBuf[1024];
	int aVal = gethostname(aBuf,1024);
	if(aVal!=0)
		return;

	HOSTENT *aHostEnt = gethostbyname(aBuf);
	if(aHostEnt==NULL)
		return;

	int i = 0;
	while(aHostEnt->h_addr_list[i]!=NULL)
	{
		in_addr aHost;
		memcpy(&aHost.S_un.S_addr,aHostEnt->h_addr_list[i],4);

		theList.push_back(NetworkAdapter("", ntohl(aHost.S_un.S_addr)));
		i++;
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool IPAddr::SetLocalHost(long theHost, bool validate)
{
	if(validate && theHost!=INADDR_ANY)
	{
		NetworkAdapterList aList;
		GetNetworkAdapterList(aList,false);
		NetworkAdapterList::iterator anItr;
		for(anItr = aList.begin(); anItr!=aList.end(); ++anItr)
		{
			if(anItr->mHost==theHost)
				break;
		}
		if(anItr==aList.end())
			return false;
	}

	mLocalHost = theHost;
	return true;
}
