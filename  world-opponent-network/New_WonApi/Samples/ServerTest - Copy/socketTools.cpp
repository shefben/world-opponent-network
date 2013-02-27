#include <time.h>
#include "stdio.h"
#include "socketTools.h"
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "direct.h"

#define RECV_TIMEOUT 1000 // 10 seconds

int recvSafe(SOCKET socket, char * data, DWORD len, int flag)
{
	int nbRetry=RECV_TIMEOUT; 
	DWORD read=0;
	while (read<len && nbRetry>0)
	{
		int r=recv(socket,data+read,len-read,flag);
		if (r<0) return r;
		if (!r) {nbRetry--;Sleep(10);}
		else nbRetry=RECV_TIMEOUT;
		read+=r;
	}
	return read;
}

int sendSafe(SOCKET socket, char * data, DWORD len, int flag)
{
	DWORD sent=0;
	while (sent<len)
	{
		int s=send(socket,data+sent,len-sent,flag);
		if (s<0) return s;
		if (!s) Sleep(10);
		sent+=s;
	}
	return sent;
}

void initWinsock()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	
	wVersionRequested = MAKEWORD( 2, 2 );
	
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		return;
	}
	
	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */
	
	if ( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 2 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		WSACleanup( );
		return; 
	}
	
}


long readSocket(SOCKET socket, int size, int *error)
{
	if (error) *error=0;
	unsigned char * c=(unsigned char*)malloc(size);
	if (recvSafe(socket,(char*)c,size,0)  < 0)
	{
		if (error) *error=1;
		return 0;
	}
	long result=0;
	int pos=0;
	while (pos<size)
	{
		result=(result<<8)+c[pos++];
	}
	free(c);
	return result;
}

unsigned int htoi(char hex)
{
	if (hex>='0' && hex<='9') return hex-'0';
	if (hex>='a' && hex<='f') return hex-'a'+10;
	if (hex>='A' && hex<='F') return hex-'A'+10;
	
	return 0;
}

ULONGLONG htoi(char* hex,int len)
{
	ULONGLONG r=0;
	for (int i=0;i<len;i++)
	{
		if (hex[i]!=' ')
			r=(r<<4)+(ULONGLONG)htoi(hex[i]);
	}
	
	return r;
}

int hexa2Buffer(unsigned char * buffer, char * hexa)
{
	int s=(strlen(hexa)-2)/3+1;
	 
	for (int pos=0;pos<s;pos++)
	{
		buffer[pos]=(int)(htoi(hexa[pos*3])<<4)+(int)htoi(hexa[(pos*3)+1]);
	}
	
	return s;
}
int hexa_2Buffer(unsigned char * buffer, char * hexa)
{
	int s=strlen(hexa)/2;
	 
	for (int pos=0;pos<s;pos++)
	{
		buffer[pos]=(int)(htoi(hexa[pos*2])<<4)+(int)htoi(hexa[(pos*2)+1]);
	}
	
	return s;
}
void sendSocket(SOCKET socket,char * hexa)
{
	int s=(strlen(hexa)-2)/3+1;
	unsigned char * c=(unsigned char*)malloc(s);
	
	int toSend=hexa2Buffer(c,hexa);
	
	int pos=0;
	while (toSend>0)
	{
		int sent=send(socket,(char*)c+pos,toSend,0);
		if (sent>0)
		{
			pos+=sent;
			toSend-=sent;
		}
		else
		{
			printf("Error writing to socket\n");
			return;
		}
	}
	
	free(c);
}

DWORD getCurrentIp(char * buffer)
{
	hostent he;
	hostent * lphe=&he;
	DWORD dwIP;
	char hostname[256];
	gethostname(hostname,256);
	lphe = gethostbyname(hostname);
	if(lphe != NULL)
	{
		dwIP = *(u_long *)lphe->h_addr_list[0];
		if (buffer!=0) wsprintf(buffer, "%d.%d.%d.%d", LOBYTE(LOWORD(dwIP)), HIBYTE(LOWORD(dwIP)), LOBYTE(HIWORD(dwIP)), HIBYTE(HIWORD(dwIP)));
		return dwIP;
	}
	return 0;
}

DWORD getHostIp(char * name)
{
	hostent he;
	hostent * lphe=&he;
	lphe = gethostbyname(name);
	if(lphe != NULL)
	{
		return *(u_long *)lphe->h_addr_list[0];
	}
	return 0;
}


SOCKET connectSocket(char * ip, int port)
{
	sockaddr_in clientService; 
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr( ip );
	clientService.sin_port = htons( port );
	
	return connectSocket(clientService);
}

SOCKET connectSocket(sockaddr_in clientService)
{
	
	SOCKET sock=socket(AF_INET, SOCK_STREAM, IPPROTO_IP);//IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		return 0;
	}
	
	if ( connect( sock, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR) {
		return 0;
	}
	return sock;
}

unsigned long reverseBytes(unsigned long value,int len)
{
	char * b=(char *)&value;
	int result=0;
	char * r=(char *)&result;
	for (int ind=0;ind<len;ind++) r[ind]=b[(len-1)-ind];
	return result;
	
}

ULONGLONG getNanosecondsSinceTime0()
{
	ULONGLONG beforeTime0=62135596800;
	time_t current;
	time(&current);
	ULONGLONG now=current;
	now+=beforeTime0;
	ULONGLONG nano=1000000;
	now=now*nano;
	return now;
}
void hexaPrintf(unsigned char b,char * buff)
{
	char * h="0123456789ABCDEF";
	buff[0]=*(h+(b/16));
	buff[1]=*(h+(b%16));
}
void hexaPrintf(unsigned char b)
{
	char * h="0123456789ABCDEF";
	printf("%c%c",*(h+(b/16)),*(h+(b%16)));
}

void hexaPrintf(char * b,int l,int full)
{
	for (int ind=0;ind<l;ind++)
	{
		if (full) if ((ind%16)==0) printf("%4x  ",ind);
		
		hexaPrintf((unsigned char)*(b+ind));
		if (full) if ((ind%16)==7) printf(" ");
		if ((ind%16)==15) 
		{
			printf("     ");
			for (int a=ind-15;a<=ind;a++) printf("%c",(b[a]<32 ? '.' : (b[a]>127 ? '.' : b[a])));
			printf("\n"); 
		}else printf(" ");
	}
	if (l%16)
	{
		for (int a=(l/16)*16;a<l;a++) printf("%c",(b[a]<32 ? '.' : (b[a]>127 ? '.' : b[a])));
		
	}
	printf("\n");
}
int strToPorts(char * str, DWORD * ports)
{
	int nb=0;
	char allPorts[1000];
	strcpy(allPorts,str);
	for (char * port=0;port=strrchr(allPorts,' '),port>=allPorts;)
	{
		port++;
		ports[nb]=atoi(port);
		nb++;
		*(port-1)=0;
	}
	ports[nb]=atoi(allPorts);
	nb++;
	
	for (int ind=0;ind<nb/2;ind++)
	{
		int tmp=ports[ind];
		ports[ind]=ports[(nb-ind)-1];
		ports[(nb-ind)-1]=tmp;
	}
	return nb;
}

char * itoip(DWORD dwIP, char * buffer)
{
	wsprintf(buffer, "%d.%d.%d.%d", LOBYTE(LOWORD(dwIP)), HIBYTE(LOWORD(dwIP)), LOBYTE(HIWORD(dwIP)), HIBYTE(HIWORD(dwIP)));
	return buffer;
}

int setFileSize(char * path, DWORD len)
{
	int fd=_open( path, _O_RDWR | _O_CREAT | _O_BINARY, _S_IREAD | _S_IWRITE );
	
	if (fd==-1) return -1;
	
	if (_chsize(fd,len))
	{
		_close(fd);
		return -1;
	}
	_close(fd);
	return 0;
}


unsigned int jenkinsHash( unsigned  char *k,unsigned int length, unsigned int initval )
{
	unsigned int a, b;
	unsigned int c = initval;
	unsigned int len = length;
	a = b = 0x9e3779b9; 
	
	while ( len >= 12 ) {
		a += ( k[0] + ( (unsigned)k[1] << 8 )
			+ ( (unsigned)k[2] << 16 )
			+ ( (unsigned)k[3] << 24 ) );
		b += ( k[4] + ( (unsigned)k[5] << 8 )
			+ ( (unsigned)k[6] << 16 )
			+ ( (unsigned)k[7] << 24 ) );
		c += ( k[8] + ( (unsigned)k[9] << 8 )
			+ ( (unsigned)k[10] << 16 )
			+ ( (unsigned)k[11] << 24 ) );
		
		//mix ( a, b, c );
		a -= b; a -= c; a ^= ( c >> 13 ); 
		b -= c; b -= a; b ^= ( a << 8 ); 
		c -= a; c -= b; c ^= ( b >> 13 ); 
		a -= b; a -= c; a ^= ( c >> 12 ); 
		b -= c; b -= a; b ^= ( a << 16 ); 
		c -= a; c -= b; c ^= ( b >> 5 ); 
		a -= b; a -= c; a ^= ( c >> 3 ); 
		b -= c; b -= a; b ^= ( a << 10 ); 
		c -= a; c -= b; c ^= ( b >> 15 ); 
		
		//end mix
		k += 12;
		len -= 12;
	}
	
	c += length;
	
	switch ( len ) {
	case 11: c += ( (unsigned)k[10] << 24 );
	case 10: c += ( (unsigned)k[9] << 16 );
	case 9 : c += ( (unsigned)k[8] << 8 );
		/* First byte of c reserved for length */
	case 8 : b += ( (unsigned)k[7] << 24 );
	case 7 : b += ( (unsigned)k[6] << 16 );
	case 6 : b += ( (unsigned)k[5] << 8 );
	case 5 : b += k[4];
	case 4 : a += ( (unsigned)k[3] << 24 );
	case 3 : a += ( (unsigned)k[2] << 16 );
	case 2 : a += ( (unsigned)k[1] << 8 );
	case 1 : a += k[0];
	}
	
	//mix ( a, b, c );
	a -= b; a -= c; a ^= ( c >> 13 ); 
	b -= c; b -= a; b ^= ( a << 8 ); 
	c -= a; c -= b; c ^= ( b >> 13 ); 
	a -= b; a -= c; a ^= ( c >> 12 ); 
	b -= c; b -= a; b ^= ( a << 16 ); 
	c -= a; c -= b; c ^= ( b >> 5 ); 
	a -= b; a -= c; a ^= ( c >> 3 ); 
	b -= c; b -= a; b ^= ( a << 10 ); 
	c -= a; c -= b; c ^= ( b >> 15 ); 
	
	// end mix
	return c;
}


void PrintData (char* data , int Size)
{
	char a , line[17] , c;
	int j;
	
	//loop over each character and print
	for(int i=0 ; i < Size ; i++)
	{
		c = data[i];
		
		//Print the hex value for every character , with a space. Important to make unsigned
		printf(" %.2x", (unsigned char) c);
		//Add the character to data line. Important to make unsigned
		a = ( c >=32 && c <=128) ? (unsigned char) c : '.';
		
		line[i%16] = a;
		
		//if last character of a line , then print the line - 16 characters in 1 line
		if( (i!=0 && (i+1)%16==0) || i == Size - 1)
		{
			line[i%16 + 1] = '\0';
			
			//print a big gap of 10 characters between hex and characters
			printf("\t");
			
			//Print additional spaces for last lines which might be less than 16 characters in length
			for( j = strlen(line) ; j < 16; j++)
			{
				printf( "   ");
			}
			
			printf("%s \n" , line);
		}
	}
	
	printf("\n");
}