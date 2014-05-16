// Server.h: interface for the Server class.
//
//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////// 
#include "ServerManager.h"
#include "socketTools.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#ifndef Server_h__
#define Server_h__

#define LOCAL_IP 0x0100007F
#define dirPort 15101
#define auPort 7002

class Server
{
public:
	virtual int start()
	{
		printf("Start() function not supported");
		return 1;
	}
};
#endif // Server_h__