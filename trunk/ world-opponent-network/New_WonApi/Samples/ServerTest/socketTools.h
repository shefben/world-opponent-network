#ifndef _SOCKET_TOOLS_H_
#define _SOCKET_TOOLS_H_
#include <Socket\WONWS.h>
#include "stdlib.h"
#include "malloc.h"
#include <process.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//#include <winbase.h>
//#include "afxsock.h"



#define INADDR_ANY              (u_long)0x00000000
#define INADDR_LOOPBACK         0x7f000001
#define INADDR_BROADCAST        (u_long)0xffffffff
#define INADDR_NONE             0xffffffff

#define ADDR_ANY                INADDR_ANY
int hexa2Buffer(unsigned char * buffer, char * hexa);
int hexa_2Buffer(unsigned char * buffer, char * hexa);
long readSocket(SOCKET socket, int size,int * error=0);

void sendSocket(SOCKET socket,char * hexa);

void PrintData (char* data , int Size);

DWORD getCurrentIp(char * buffer=0);

SOCKET connectSocket(char * ip, int port);
	
SOCKET connectSocket(sockaddr_in clientService);

DWORD getHostIp(char * name);
void PrintUnknownPacket(const unsigned char *, unsigned int);
void initWinsock();

unsigned long reverseBytes(unsigned long value, int len=4);

ULONGLONG getNanosecondsSinceTime0();
void hexaPrintf(unsigned char b,char * buff);
void hexaPrintf(char * b,int l,int full=1);

ULONGLONG htoi(char* hex,int len);
int strToPorts(char * str, DWORD * ports);
char * itoip(DWORD dwIP, char * buffer);

int recvSafe(SOCKET socket, char * data, DWORD len, int flag);
int sendSafe(SOCKET socket, char * data, DWORD len, int flag);
int setFileSize(char * path, DWORD len);
unsigned int jenkinsHash( unsigned  char *k,unsigned int length, unsigned int initval );
int folderExists(char * path);
#endif