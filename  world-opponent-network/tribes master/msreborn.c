/*
    Copyright 2005,2006 Luigi Auriemma
	Modified further by Kigen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

    http://www.gnu.org/licenses/gpl.txt
*/

// Cut out excess MFC crap. - Kigen
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MSENGINEVER         "0.7"
#define MSBUFFSZ            2048
#define MSMAXSERVERS        512
#define MSCHKTIME           2
#define MSIPFMT             "%s:%hu"
#define MSPORT				28000       // master server query/hearbeat port
#define MSTIMEOUT			170         // 2 minutes plus some
#define MSTHREADS			4			// Number of threads to spawn instantly (incoming connection threads)
#define MSBACKUP			30			// Backup the list of servers every 30 seconds.
#define MSMAXPERIP			25			// Maximum number of servers per IP.
#define MAXTHREADS			21			// Incoming connections threads + timeout thread.

#ifdef WIN32
    #include <winsock.h>
    #include "winerr.h"

    #define close   closesocket
    #define sleep   Sleep
    #define ONESEC  1000
	#define snprintf sprintf_s
    DWORD   tid[MAXTHREADS];
    HANDLE  th[MAXTHREADS];
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <sys/param.h>
    #include <pthread.h>

    #define ONESEC  1
    pthread_t   tid[MAXTHREADS];
    int         th[MAXTHREADS];
#endif


#define BUFFSZ      1024

#define PUTSTR(x)   *p++ = sizeof(x) - 1; \
                    memcpy(p, x, sizeof(x) - 1); \
                    p += sizeof(x) - 1;
#define SWAP16(x)   ((x >> 8) | (x << 8))
#define SERVERSPCK  63

#ifdef WIN32
#define MSTHREAD(NAME,ARG)  DWORD WINAPI NAME(ARG)
#define MSTHREADX(TH,TID,THR,PAR) \
        TH = CreateThread(NULL, 0, (void *)&THR, (void *)PAR, 0, &TID); \
        if(!TH) { \
            fputs("\nError: Unable to create thread\n", stdout); \
            exit(1); \
        }
#define LOCK EnterCriticalSection
#define UNLOCK LeaveCriticalSection
#else
#define MSTHREAD(NAME,ARG)  void *NAME(ARG)
#define MSTHREADX(TH,TID,THR,PAR) \
        TH = pthread_create(&TID, NULL, (void *)&THR, (void *)PAR); \
        if(TH) { \
            fputs("\nError: Unable to create thread\n", stdout); \
            exit(1); \
        }
#define LOCK pthread_mutex_lock
#define UNLOCK pthread_mutex_unlock
#endif



MSTHREAD(client, int sock);
MSTHREAD(timeout_thread, int sock);
MSTHREAD(heartbeat_thread, void);
int heartbeat(u_char *buff, int len, u_int ip, u_short *port, u_int *crc);
void remove_server(int curr);
void add_server(u_int ip, u_short port, u_int crc, int sock);
u_int crc32(u_char *data, int size);
int timeout(int sock, int secs);
u_int resolv(char *host);
void process_buffer( int sock, u_char ibuff[BUFFSZ], struct sockaddr_in peer, int len );
void std_err(void);



#pragma pack(2)     // pack saves a lot of memory
struct ms_ipport {
    u_int	ip;     // server IP
    u_short	port;   // server port
    time_t	timex; // heartbeat time
	u_short	validated; // Used to make sure server is live. - Kigen
	u_short key;
} *server;
struct msr_ipport 
{
    u_int   ip;     // server IP (Network byte order)
    u_short port;   // server port
	u_short key;
	char	*host;  // Hostname or IP (in IPv4 format)
	short int	totalpack;
	short int	packrecv;
	time_t  timex;  // Last check time.
} *master;
#pragma pack()

int     maxservers = MSMAXSERVERS,
		maxmasters = 20,
		backuptime = MSBACKUP,
		maxserversperip = MSMAXPERIP,
	    tot,
		ttol,
        show       = 1,
		threads	   = MSTHREADS,
		tms		   = 0,
		hattempts  = 0,
		ipackets   = 0,
		iqpackets  = 0,
		impackets  = 0;
u_short msport;
char motd[256]; // Now a global variable. :D - Kigen
struct stat motdstat;

#ifdef WIN32
CRITICAL_SECTION cs;
CRITICAL_SECTION ms;
#else
pthread_mutex_t cs;
pthread_mutex_t ms;
#endif

int main(int argc, char *argv[]) 
{
#ifdef WIN32
	struct WSAData wsadata;
#endif
    struct  sockaddr_in peer;
    int     sdl,
            on            = 1,
            psz,
			i,
			ti,
			found;
    u_char  *p;
	u_int	ip;
	u_short port;
	char    buff[BUFFSZ];
	FILE	*fp;

    msport   = 28000;
	memset(buff, '\0', BUFFSZ);

#ifdef WIN32
	InitializeCriticalSection(&cs);
	InitializeCriticalSection(&ms);
	WSAStartup(MAKEWORD(1,0), &wsadata);
#else
	(void)pthread_mutex_init(&cs, NULL);
	(void)pthread_mutex_init(&ms, NULL);
#endif

    setbuf(stdout, NULL);

	srand((u_int)(floor((double)time(NULL)))/2);

    fputs("\n"
        "Tribes 1 Master Server v"MSENGINEVER"\n"
        "Originally by Luigi Auriemma\n"
		"Modified by Max Krivanek\n"
        "E-mail: aluigi@autistici.org\n"
        "Web:    aluigi.org\n"
        "\n", stdout);

    printf("\n"
        "Usage: %s [options] [server1] ... [serverN]\n"
        "\n"
        "Options:\n"
        "-m NUM  max servers supported in the database (default %d)\n" // Max servers
        "-p PORT overrides the default master server query port (%d)\n" // Port
		"-t NUM  Number of threads to create for incoming connections (%d)\n"
        "-q      quiet, the clients and the new servers will not be showed\n"
        "        Timeout checks are made every %d seconds and check if the dynamic\n" // timeout sec
        "        servers have sent no heartbeats for %d minutes\n" // timeout minutes
        "\n"
        "Servers:\n"
        "  IP:PORT\n"
        "IP can be an IP address or a hostname.\n"
        "\n",
        argv[0], maxservers, msport, threads, MSCHKTIME, MSTIMEOUT / 60);

    for(i = 1; i < argc; i++) {
        if(argv[i][0] != '-') break;
        switch(argv[i][1]) {
            case 'm': maxservers = atoi(argv[++i]); break;
            case 'p': msport = atoi(argv[++i]); break;
			case 't': threads = atoi(argv[++i])+1; break;
            case 'q': show = 0; break;
            default: {
                printf("\nError: wrong command-line argument (%s)\n\n", argv[i]);
                exit(1);
            }
        }
    }

	if ( maxservers < 5 || maxservers > 1024 )
	{
		printf("Error: Maxservers out of range, must be between 5 and 1024.\n");
		exit(1);
	}
	if ( msport < 1024 )
	{
		printf("Error: Master server port out of range, must be between 1024 and 65535.\n");
		exit(1);
	}
	if ( threads < 2 || threads > MAXTHREADS )
	{
		printf("Error: Number of threads out of range, must be between 1 and %d.\n", MAXTHREADS-1);
		exit(1);
	}

    printf("- database memory allocation (max %d servers)\n", maxservers);
    server = malloc(maxservers * sizeof(struct ms_ipport));
	master = malloc(maxmasters * sizeof(struct msr_ipport));
    if(!server || !master) std_err();

    if(show) fputs("- command-line fixed servers:\n", stdout);
    for(tot = 0; (i < argc) && (tot < maxservers); i++, tot++) {
        p = strchr(argv[i], ':');
        if(!p) {
            fputs("\n"
                "Error: you must specify the game port after the IP like 1.2.3.4:1234\n"
                "\n", stdout);
            exit(1);
        }
        *p++ = 0;

        server[tot].ip    = resolv(argv[i]);
        server[tot].port  = htons(atoi(p));
        server[tot].timex = -1; // fixed IP has the maximum amount of time
		server[tot].validated = 1;

        if(show) {
            printf("  "MSIPFMT"\n",
                inet_ntoa(*(struct in_addr *)&server[tot].ip),
                ntohs(server[tot].port));
        }
    }
	ttol = (tot);
    if(show) printf("\n  Total fixed servers: %d\n", tot);

#ifdef WIN32
	fopen_s(&fp, "masterserverlist.txt", "r");
#else
	fp = fopen("masterserverlist.txt", "r");
#endif
	if ( fp == NULL )
		printf("Unable to open masterserverlist.txt.\n");
	while( fp != NULL && !feof(fp))
	{
		if ( fgets(buff,BUFFSZ,fp) == NULL )
			continue;
		p = strchr(buff, ':');
		if(!p) 
			continue;

		*p++ = 0;

		ip = resolv(buff);
		port = htons(atoi(p));

		if ( ip == INADDR_NONE || port == 0 )
			continue;

		found = 0;
		for(ti = 0; ti < tot; ti++) 
			if(master[ti].ip == ip && master[ti].port == port) // Duplicate entry.
				found = 1;

		if ( found != 0 )
			continue;

		master[tms].ip	 = ip;
		master[tms].port = port;
		master[tms].host = malloc(BUFFSZ);
		memcpy(master[tms].host,buff,BUFFSZ);
		master[tms].key  = 0;
		master[tms].packrecv = 0;
		master[tms].totalpack = 0;
		master[tms].timex = time(NULL)-400;
		tms++;

		if ( tms == maxmasters )
		{
			printf("  Max of %d master servers reached.\n", maxmasters);
			break;
		}
	}
	if ( fp != NULL )
		fclose(fp);

	memset(motd,'\0',sizeof(motd));

#ifdef WIN32
	fopen_s(&fp, "motd.txt", "r");
#else
	fp = fopen("motd.txt", "r");
#endif
	if ( fp != NULL )
	{
		// bugs_ suggestion and code. :) - Kigen
		fread(motd,sizeof(motd)-1,1,fp);
		fclose(fp);
		stat("motd.txt", &motdstat);
	}

    peer.sin_addr.s_addr = INADDR_ANY;
    peer.sin_port        = htons(msport);
    peer.sin_family      = AF_INET;
    psz                  = sizeof(peer);

    printf("\n"
        "- bind UDP port %hu\n"
        "- max %d servers supported\n",
        msport, maxservers);

    sdl = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sdl < 0) std_err();
	if(setsockopt(sdl, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))
	  < 0) std_err();
    if(bind(sdl, (struct sockaddr *)&peer, sizeof(peer))
	  < 0) std_err();

    fputs("- Ready\n", stdout);

	MSTHREADX(th[0], tid[0], timeout_thread, sdl); // Timeout thread. :)

	// Thread pooling instead. :) - Kigen
	for(i=1;i<threads;i++)
	{
		MSTHREADX(th[i], tid[i], client, sdl);
	}

#ifdef WIN32
	fopen_s(&fp, "backup.txt", "r");
#else
	fp = fopen("backup.txt", "r");
#endif
	if ( fp == NULL )
		printf("Unable to open backup.txt.\n");
	while( fp != NULL && !feof(fp))
	{
		if ( fgets(buff,BUFFSZ,fp) == NULL )
			continue;
		p = strchr(buff, ':');
		if(!p) 
			continue;

		*p++ = 0;

		ip = resolv(buff);     // Done like this to insure proper IP is recieved.
		port = htons(atoi(p));

		if ( ip == INADDR_NONE || port == 0 )
			continue;

		found = 0;
		LOCK(&cs);
		for(ti = 0; ti < tot; ti++) 
			if((server[ti].ip == ip) && (server[ti].port == port))
				found = 1;

		if ( found == 0 )
			add_server(ip, port, 0, sdl);
		UNLOCK(&cs);
	}
	if ( fp != NULL )
		fclose(fp);

	while( 1 )
	{
#ifdef WIN32
		scanf_s("%s", buff);
#else
		scanf("%s", buff);
#endif
		switch( buff[0] )
		{
			case 'a':
			case 'A':
			{
				printf("  There have been %d invalid packets,\n"
					   "  %d invalid query packets,\n"
					   "  %d invalid master server reply packets,\n"
					   "  and %d hacking attempts.\n",
					   ipackets,
					   iqpackets,
					   impackets,
					   hattempts);
				break;
			}
			case 'c':
			case 'C':
			{
				LOCK(&cs);
				printf("  There are %d visible servers in the list.\n"
					   "  There are %d servers waiting to be verified.\n"
					   "  There are %d servers total.\n", 
					   ttol, (tot - ttol), tot);
				UNLOCK(&cs);
				break;

			}
			case 'm':
			case 'M':
			{
				printf("  There are a total of %d master servers in the list.\n", tms);
				break;
			}
			case 'q':
			case 'Q':
			{
				printf("  Exiting....\n");
				close(sdl);
				sleep(ONESEC); // Figure it out for yourself. :P - Kigen
#ifdef WIN32
				DeleteCriticalSection(&cs);
				DeleteCriticalSection(&ms);
#endif
				return(0);
				break;
			}
			case 'h':
			case 'H':
			case '?':
			{
				printf("  Command help:\n"
					   "  a - Invalid packets and hacking attempts stats.\n"
					   "  c - Server count.\n"
					   "  m - Master server count.\n"
					   "  q - Quit\n");
				break;
			}
			default:
			{
				printf("  Unknown command %s.\n", buff);
				printf("  Type 'h' for help.\n");
				break;
			}
		}
	}

    close(sdl);
#ifdef WIN32
	DeleteCriticalSection(&cs);
	return 0;
#else
	pthread_exit(0);
#endif
}


MSTHREAD(client, int sock) {
    struct  sockaddr_in peer;
    int     psz,
            len,
			sda;
    u_char  buff[BUFFSZ];
	fd_set  fd_read;

	sda = sock;

	while( 1 )
	{
	    peer.sin_addr.s_addr = INADDR_ANY;
	    peer.sin_port        = htons(msport);
	    peer.sin_family      = AF_INET;
	    psz                  = sizeof(peer);
        FD_ZERO(&fd_read);
        FD_SET(sock, &fd_read);
        if(select(sda + 1, &fd_read, NULL, NULL, NULL)
          < 0) std_err();
		len = recvfrom(sda, buff, BUFFSZ, 0, (struct sockaddr *)&peer, &psz);
		if(len <= 0) continue; // Socket was closed.  Not always?
		if (len < 2 || len > 512)
		{
			printf("  Invalid packet received from "MSIPFMT".\n",
				inet_ntoa(peer.sin_addr),
				ntohs(peer.sin_port));
			LOCK(&ms);
			ipackets++;
			UNLOCK(&ms);
			continue;
		}
		process_buffer(sda, buff, peer, len);
	}
    return(0);
}

void process_buffer( int sock, u_char ibuff[BUFFSZ], struct sockaddr_in peer, int len )
{
    int     i,
			ti,
			mi;
	u_long  ip;
    u_short *pck, port, found;
    u_char	buff[BUFFSZ],
			*pbuff,
            *p;

	memcpy(buff, ibuff, BUFFSZ);

	if(buff[0] != 0x61 && buff[1] == 0x03) 
	{
		if ( len != 8 ) // Incoming query should be exactly 8 bytes.
		{
			printf("  Invalid query request from "MSIPFMT".\n",
				inet_ntoa(peer.sin_addr),
				ntohs(peer.sin_port));
			LOCK(&ms);
			iqpackets++;
			UNLOCK(&ms);
			return;
		}
		if(show)
			printf("  Query request from "MSIPFMT".\n",
				inet_ntoa(peer.sin_addr),
				ntohs(peer.sin_port));

		LOCK(&cs);
		buff[0] = 0x10;
		buff[1] = 0x06;
		buff[2] = 1;    // counter
		buff[3] = (ttol / SERVERSPCK) + ((ttol % SERVERSPCK) ? 1 : 0); // Number of packets that will be sent.
		// 4 and 5 are identifiers
		buff[6] = 0;
		buff[7] = 0x66;
		p = buff + 8;
		PUTSTR("Tribes Master");
		if ( strlen(motd) < 1 )
		{
			PUTSTR("No MOTD"); // No MOTD
		}
		else
		{
			PUTSTR(motd); // MOTD
		}
		pck = (u_short *)p;
		p += 2;
		ti = 0;
		for(i = 0; i < tot;i++) 
		{
			if ( server[i].validated != 0 )
			{
				*p++ = 6;
				*(u_int *)p = server[i].ip;
				p += 4;
				*(u_short *)p = SWAP16(server[i].port);
				p += 2;
				ti++;

				if(!(ti % SERVERSPCK)) {
					*pck = htons(SERVERSPCK);
					sendto(sock, buff, p - buff, 0, (struct sockaddr *)&peer, sizeof(peer));
					buff[2]++;
					pck = (u_short *)(buff + 8);
					p = buff + 10;
				}
			}
		}
		if(ti %= SERVERSPCK) {
			*pck = htons(ti);
			sendto(sock, buff, p - buff, 0, (struct sockaddr *)&peer, sizeof(peer));
		}
		UNLOCK(&cs);
	} 
	else if (buff[0] != 0x61 && buff[1] == 0x05) // Tribes server heartbeat.
	{
		LOCK(&cs);
		found = 0;
		for(i = 0; i < tot; i++) 
			if((server[i].ip == peer.sin_addr.s_addr) && (server[i].port == peer.sin_port) && server[i].validated != 1) // Check if the server should be replying to a GameSpy ping.
					found = 1;
		if ( found == 0 )
			add_server(peer.sin_addr.s_addr, peer.sin_port, 0, sock);
		UNLOCK(&cs);
	}
	else if(buff[0] == 0x61) // GameSpy ping response.
	{
		LOCK(&cs);
		found = 0;
		for(i = 0; i < tot; i++) 
		{
			if((server[i].ip == peer.sin_addr.s_addr) && (server[i].port == peer.sin_port)) // Check to see if the server is in the list.
			{
				pbuff = (unsigned char *)&port; // Use port variable.  Don't mind it's name. :P - Kigen
				pbuff[0] = buff[1];
				pbuff[1] = buff[2];
				if ( port == server[i].key )
					add_server(peer.sin_addr.s_addr, peer.sin_port, 0, sock);
				else
					printf("  Invalid key from "MSIPFMT". Key: %hu : %hu\n",
					inet_ntoa(peer.sin_addr),
					ntohs(peer.sin_port),
					port,
					server[i].key);
				break;
			}
		}
		UNLOCK(&cs);
	} 
	else if ( buff[0] == 0x10 && buff[1] == 0x06 )
	{
		LOCK(&ms);
		if ( len < 10 || len > 1024 )
		{
			printf("  Invalid master server reply from "MSIPFMT".\n",
				inet_ntoa(peer.sin_addr),
				ntohs(peer.sin_port));
			impackets++;
			UNLOCK(&ms);
			return;
		}
		found = 0;
		for(mi=0;mi<tms;mi++)
		{
			if ( master[mi].ip == peer.sin_addr.s_addr && master[mi].port == peer.sin_port )
			{
				if ( master[mi].totalpack == 0 ) // No query was sent.
					break;

				pbuff = (unsigned char *)&port;
				pbuff[0] = buff[4];
				pbuff[1] = buff[5];

				if ( port != master[mi].key )
					break; // Hacking attempt.

				if ( master[mi].totalpack == -1 ) // Was awaiting reply.
				{
					master[mi].packrecv = 0;
					master[mi].totalpack = (int)buff[3];
				}

				if ( master[mi].totalpack < (int)buff[2] )
					break;

				if ( show )
					printf("  Master %s at "MSIPFMT" replied to query, %d of %d.\n",
						master[mi].host,
						inet_ntoa(peer.sin_addr),
						ntohs(peer.sin_port),
						(int)buff[2],
						(int)buff[3]);

				master[mi].packrecv++;

				if ( master[mi].packrecv >= master[mi].totalpack ) // Master has sent all of its packets.
					master[mi].totalpack = 0;

				if ( buff[2] == 1 )
				{
					if ( (int)buff[8]+8 >= len )
					{
						printf("  Master server reply from "MSIPFMT" ended prematurely.\n",
							inet_ntoa(peer.sin_addr),
							ntohs(peer.sin_port));
						impackets++;
						found = 1;
						break;
					}
					i=8+(int)buff[8]+(int)buff[((int)buff[8]+8)]; // Makes a difference.
					if ( i >= len ) // Verify we have all of the packet and are not going into the unknown. :)
					{
						printf("  Master server reply from "MSIPFMT" ended prematurely.\n",
							inet_ntoa(peer.sin_addr),
							ntohs(peer.sin_port));
						impackets++;
						found = 1;
						break;
					}
				}
				else
					i=8;

				if ( len > BUFFSZ )
					break;

				for(;i<len;)
				{
					if ( buff[i++] == 0x06 ) // Next 4 are IP and the 2 after are port.
					{	
						if ( (i + 6) > len )
						{
							printf("  Master server reply from "MSIPFMT" ended prematurely.\n",
								inet_ntoa(peer.sin_addr),
								ntohs(peer.sin_port));
							impackets++;
							break;
						}
						pbuff = (unsigned char *)&ip; // Next four bytes were original from a u_int, go figure. - Kigen
						pbuff[0] = buff[i++];
						pbuff[1] = buff[i++];
						pbuff[2] = buff[i++];
						pbuff[3] = buff[i++];
						pbuff = (unsigned char *)&port; // Port bytes are backwards. Result of htons and ntohs? O.o - Kigen
						pbuff[1] = buff[i++];
						pbuff[0] = buff[i++];
						found = 0;
						if ( ip == INADDR_NONE || port == 0 )
							continue;
						LOCK(&cs);
						for(ti = 0; ti < tot; ti++) 
							if((server[ti].ip == ip) && (server[ti].port == port))
								found = 1;

						if ( found == 0 )
							add_server(ip, port, 0, sock);
						UNLOCK(&cs);
					}
				}
				found = 1;
			}
		}
		UNLOCK(&ms);
		if ( found != 1 )
		{
			if ( show )
				printf("HACKING ATTEMPT! "MSIPFMT" attempted to send a master server reply that wasn't called for.\n",
					inet_ntoa(peer.sin_addr),
					ntohs(peer.sin_port));
			hattempts++;
		}
	}
}

MSTHREAD(timeout_thread, int sock) {
    time_t  oldtime, mschktime, backupchk;
    int     i;
	struct sockaddr_in  peer;
	u_char  buff[BUFFSZ], *pbuff;
 	u_int ip;
	FILE *fp;
	struct stat motdnow;

    peer.sin_addr.s_addr = INADDR_ANY;
    peer.sin_port        = htons(msport);
    peer.sin_family      = AF_INET;

	mschktime = time(NULL)-400; // Check immediately.
	backupchk = time(NULL);

    while ( 1 ) {
		sleep(ONESEC); // Put here because client thread pooling is created after this one. - Kigen
		stat("motd.txt", &motdnow);
		if ( motdnow.st_mtime != motdstat.st_mtime )
		{
#ifdef WIN32
			fopen_s(&fp, "motd.txt", "r");
#else
			fp = fopen("motd.txt", "r");
#endif
			if ( fp != NULL )
			{
				// bugs_ suggestion and code. :) - Kigen
				memset(motd,'\0',sizeof(motd));
				fread(motd,sizeof(motd)-1,1,fp);
				fclose(fp);
				stat("motd.txt", &motdstat);
				printf("  MOTD updated.\n");
			}
		}
		LOCK(&ms);
		for(i=0;i<tms;i++)
		{
			if ( ( time(NULL) - master[i].timex ) > 300 ) // Been 5 minutes since last check.
			{
				master[i].timex = time(NULL);
				ip = resolv(master[i].host);
				if ( ip == INADDR_NONE )
					continue;
				if ( ip != master[i].ip )
				{
					master[i].ip = ip;
					printf("  Master %s changed IP.\n", master[i].host);
				}
				master[i].key = (u_short)rand();
				pbuff = (unsigned char *)&master[i].key;
				buff[0] = 0x10;
				buff[1] = 0x03;
				buff[2] = 0xFF;
				buff[3] = 0x00;
				buff[4] = pbuff[0];
				buff[5] = pbuff[1];
				buff[6] = 0x00;
				buff[7] = 0x00;
				peer.sin_addr.s_addr = master[i].ip;
				peer.sin_port = master[i].port;
				master[i].totalpack = -1;
				printf("  Querying master %s at %s:%hu. Key: %d\n", master[i].host, inet_ntoa(peer.sin_addr), ntohs(peer.sin_port), master[i].key);
				sendto(sock, buff, 8, 0, (struct sockaddr *)&peer, sizeof(peer));
				break; // One master at a time.  (Saves from spam fucking the server.)
			}
		}
		UNLOCK(&ms);
		sleep(ONESEC);
		LOCK(&cs);
        oldtime = time(NULL) - MSTIMEOUT;
		for(i = 0; i < tot; i++) 
		{
            if(server[i].timex < oldtime && server[i].timex != -1 && server[i].validated == 1 ) 
			{
				server[i].timex = time(NULL);
				server[i].validated = 2; // Timeout.
				peer.sin_addr.s_addr = server[i].ip;
				peer.sin_port        = server[i].port;
				peer.sin_family      = AF_INET;

				server[i].key = (u_short)rand();
				pbuff = (unsigned char *)&server[i].key;
				buff[0] = 0x60; // Gamespy Ping query.
				buff[1] = pbuff[0];
				buff[2] = pbuff[1];
				sendto(sock, buff, 3, 0, (struct sockaddr *)&peer, sizeof(peer));
            }
			else if ( server[i].validated != 1 && server[i].timex < (time(NULL)-20) )
			{
				remove_server(i); // server has not validated.
			}
        }
		// Perform textfile backup.
		if ( backuptime > 0 && ( time(NULL) - backupchk ) > backuptime )
		{
#ifdef WIN32
			fopen_s(&fp, "backup.txt", "w");
#else
			fp = fopen("backup.txt", "w");
#endif
			if ( fp != NULL )
			{
				for(i=0;i<tot;i++)
				{
					// Used for inet_ntoa - Kigen
					peer.sin_addr.s_addr = server[i].ip;
					snprintf(buff, BUFFSZ, "%s:%d\n", inet_ntoa(peer.sin_addr), ntohs(server[i].port));
					fwrite(buff, 1, strlen(buff), fp);
				}
				fclose(fp);
			}
			else
			{
				if (show)
					printf("  Backup failed, unable to open backup.txt for writing.\n");
			}
		}
		UNLOCK(&cs);
	}
    return(0);
}


void remove_server(int curr) 
{
	tot--;
    if(server[curr].validated != 0) 
	{
		ttol--;
		if (show)
			printf("  Server "MSIPFMT" offline, removed.  %d servers left in the list.\n",
				inet_ntoa(*(struct in_addr *)&server[curr].ip),
				ntohs(server[curr].port), ttol);
    }
	else if (show)
			printf("  Server "MSIPFMT" failed to verify, removed.\n",
				inet_ntoa(*(struct in_addr *)&server[curr].ip),
				ntohs(server[curr].port));

    memmove(&server[curr], &server[tot], sizeof(struct ms_ipport));
	return;
}


void add_server(u_int ip, u_short port, u_int crc, int sock) {
    int     i, count = 0;
	struct sockaddr_in  peer;
	u_char  buff[BUFFSZ], *pbuff;

	peer.sin_addr.s_addr = ip;
    peer.sin_port        = port;
    peer.sin_family      = AF_INET;

    if(tot == maxservers) 
	{
		if(show)
			printf("  Server "MSIPFMT" failed to add because the servers list is full.\n",
			    inet_ntoa(peer.sin_addr),
			    ntohs(port));
        return;
    }

    for(i = 0; i < tot; i++) 
	{
        if(server[i].ip == ip) 
		{
			if (server[i].port == port)
			{
				if ( server[i].validated != 1 )
				{
					if(server[i].validated == 0) 
					{
						ttol++;
						if ( show )
							printf("  Server "MSIPFMT" online and added, %u total servers.\n",
								inet_ntoa(peer.sin_addr),
								ntohs(port), ttol);
					}
					server[i].validated = 1;
				}
				// printf("  Server "MSIPFMT" heartbeat.\n",
				//		inet_ntoa(peer.sin_addr),
				//		ntohs(port));
				if ( server[i].timex != -1 )
					server[i].timex = time(NULL); // Modified to act as a heartbeat. - Kigen
				return;
			}
			count++;
		}
	}
	if ( count >= maxserversperip )
	{
		if(show)
			printf("  Server "MSIPFMT" failed to add because there are %d servers with that IP.\n",
				inet_ntoa(peer.sin_addr),
				ntohs(port),
				maxserversperip);
		return;
	}

	server[tot].ip     = ip;
	server[tot].port   = port;
	server[tot].timex  = time(NULL);
	server[tot].validated = 0;
	server[tot].key = (u_short)rand();
	pbuff = (unsigned char *)&server[tot].key;
	tot++;
	buff[0] = 0x60; // Gamespy Ping query.
	buff[1] = pbuff[0];
	buff[2] = pbuff[1];
	sendto(sock, buff, 3, 0, (struct sockaddr *)&peer, sizeof(peer));

	if(show) {
		printf("  Verifing server "MSIPFMT".\n",
			inet_ntoa(peer.sin_addr),
			ntohs(port));
	}
	return;
}



int timeout(int sock, int secs) {
    struct  timeval tout;
    fd_set  fd_read;

    tout.tv_sec = secs;
    tout.tv_usec = 0;
    FD_ZERO(&fd_read);
    FD_SET(sock, &fd_read);
    if(select(sock + 1, &fd_read, NULL, NULL, &tout)
      <= 0) return(-1);
    return(0);
}



u_int resolv(char *host) {
    struct  hostent *hp;
    u_int   host_ip;

    host_ip = inet_addr(host);
    if(host_ip == INADDR_NONE) {
        hp = gethostbyname(host);
        if(!hp) {
            printf("  Error: Unable to resolve hostname (%s)\n", host);
            return INADDR_NONE;
        } else host_ip = *(u_int *)hp->h_addr;
    }
    return(host_ip);
}



#ifndef WIN32
    void std_err(void) {
        perror("\nError");
        exit(1);
    }
#endif

