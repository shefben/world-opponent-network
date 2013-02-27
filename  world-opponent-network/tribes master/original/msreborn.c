/*
    Copyright 2005,2006 Luigi Auriemma

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef WIN32
    #include <winsock.h>
    #include "winerr.h"

    #define close   closesocket
    #define sleep   Sleep
    #define ONESEC  1000
    DWORD   tid1,
            tid2,
            tid3;
    HANDLE  th1,
            th2,
            th3;
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <sys/param.h>
    #include <pthread.h>

    #define ONESEC  1
    pthread_t   tid1,
                tid2,
                tid3;
    int         th1,
                th2,
                th3;
#endif



#define MSENGINEVER         "0.1"
#define MSBUFFSZ            2048
#define MSMAXSERVERS        256
#define MSCHKTIME           15
#define MSIPFMT             "%s:%hu"

#ifdef WIN32
#define MSTHREAD(NAME,ARG)  DWORD WINAPI NAME(ARG)
#define MSTHREADX(TH,TID,THR,PAR) \
        TH = CreateThread(NULL, 0, (void *)&THR, (void *)PAR, 0, &TID); \
        if(!TH) { \
            fputs("\nError: Unable to create thread\n", stdout); \
            exit(1); \
        }
#else
#define MSTHREAD(NAME,ARG)  void *NAME(ARG)
#define MSTHREADX(TH,TID,THR,PAR) \
        TH = pthread_create(&TID, NULL, (void *)&THR, (void *)PAR); \
        if(TH) { \
            fputs("\nError: Unable to create thread\n", stdout); \
            exit(1); \
        }
#endif



MSTHREAD(client, int sock);
MSTHREAD(timeout_thread, void);
MSTHREAD(heartbeat_thread, void);
int heartbeat(u_char *buff, int len, u_int ip, u_short *port, u_int *crc);
void remove_server(int curr);
void remove_request(u_int ip, u_short port);
void add_server(u_int ip, u_short port, u_int crc);
u_int crc32(u_char *data, int size);
int timeout(int sock, int secs);
u_int resolv(char *host);
void std_err(void);



#pragma pack(2)     // pack saves a lot of memory
struct ms_ipport {
    u_int   ip;     // server IP
    u_short port;   // server port
    u_int   timex;  // heartbeat time
    u_int   crc;    // used for tracking gamenames and versions
} *server;
#pragma pack()

u_int   maxservers = MSMAXSERVERS;
int     tot,
        show       = 1,
        crc_check  = 1;
u_short msport,
        mshbport;



#include "tribes1ms.h"



int main(int argc, char *argv[]) {
#ifndef MSTCP
    fd_set  fd_read;
#endif
    struct  sockaddr_in peer;
    int     sdl,
            sda,
            on            = 1,
            psz,
            i,
            nohb          = 0,
            check_timeout = 1;
    u_char  *p,
            *g;

    msport   = MSPORT,
#ifdef MSHBPORT
    mshbport = MSHBPORT;
#else
    mshbport = MSPORT;
#endif

#ifdef WIN32
    WSADATA    wsadata;
    WSAStartup(MAKEWORD(1,0), &wsadata);
#endif


    setbuf(stdout, NULL);

    fputs("\n"
        "MSReborn "MSENGINEVER": "MSNAME" "MSVER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    aluigi.org\n"
        "\n", stdout);

    printf("\n"
        "Usage: %s [options] [server1] ... [serverN]\n"
        "\n"
        "Options:\n"
        "-d      disables heartbeats handling, uses only the static servers\n"
        "        specified in the command-line (-t will be enabled automatically)\n"
        "-c      disables gamename/type/crc/version/data handling (where available)\n"
        "-m NUM  max servers supported in the database (default %d)\n"
        "-p PORT overrides the default master server query port (%d)\n"
        "-P PORT overrides the default master server heartbeat port (%d)\n"
        "-q      quiet, the clients and the new servers will not be showed\n"
        "-t      avoids the timeout check, servers in the list are never removed.\n"
        "        Timeout checks are made every %d seconds and check if the dynamic\n"
        "        servers have sent no heartbeats for %d minutes\n"
        "\n"
        "Servers:\n"
        "  IP:PORT[:gametype]\n"
        "IP can be an IP address or a hostname and gametype is a text string that\n"
        "identifies a specific game (available only if the plugin supports CRC)\n"
        "\n",
        argv[0], MSCHKTIME, MSTIMEOUT / 60, maxservers, msport, mshbport);

    for(i = 1; i < argc; i++) {
        if(argv[i][0] != '-') break;
        switch(argv[i][1]) {
            case 'd': { nohb = 1; check_timeout = 0; } break;
            case 'c': crc_check = 0; break;
            case 'm': maxservers = atoi(argv[++i]); break;
            case 'p': msport = atoi(argv[++i]); break;
            case 'P': mshbport = atoi(argv[++i]); break;
            case 'q': show = 0; break;
            case 't': check_timeout = 0; break;
            default: {
                printf("\nError: wrong command-line argument (%s)\n\n", argv[i]);
                exit(1);
            }
        }
    }

    printf("- database memory allocation (max %d servers)\n", maxservers);
    server = malloc(maxservers * sizeof(struct ms_ipport));
    if(!server) std_err();

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
        server[tot].crc   = 0;

        if(crc_check) {
            g = strchr(p, ':');
            if(g) {
                g++;
                server[tot].crc = crc32(g, strlen(g));
            }
        }

        if(show) {
            printf("  "MSIPFMT"\n",
                inet_ntoa(*(struct in_addr *)&server[tot].ip),
                ntohs(server[tot].port));
        }
    }
    if(show) printf("\n  Total fixed servers: %d\n", tot);

    peer.sin_addr.s_addr = INADDR_ANY;
    peer.sin_port        = htons(msport);
    peer.sin_family      = AF_INET;
    psz                  = sizeof(peer);

    printf("\n"
        "- bind %s port %hu (clients)\n"
        "- max %d servers supported\n",
#ifdef MSTCP
        "TCP",
#else
        "UDP",
#endif
        msport, maxservers);

#ifdef MSTCP
    sdl = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#else
    sdl = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#endif
    if(sdl < 0) std_err();
	if(setsockopt(sdl, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))
	  < 0) std_err();
    if(bind(sdl, (struct sockaddr *)&peer, sizeof(peer))
	  < 0) std_err();
#ifdef MSTCP
    if(listen(sdl, SOMAXCONN)
      < 0) std_err();
#else
    sda = sdl;
#endif

    if(!check_timeout) fputs("- timeout disabled\n", stdout);

#ifdef MSHBPORT
    if(!nohb) {
        printf("- bind UDP port %hu (heartbeats)\n", mshbport);
        MSTHREADX(th1, tid1, heartbeat_thread, NULL);
    }
#endif

    fputs("- Ready\n", stdout);

    if(check_timeout) {
        MSTHREADX(th2, tid2, timeout_thread, NULL);
    }

    for(;;) {
#ifdef MSTCP
        sda = accept(sdl, (struct sockaddr *)&peer, &psz);
        if(sda < 0) std_err();

        if(show) {
            printf("  "MSIPFMT"   query\n",
                inet_ntoa(peer.sin_addr),
                ntohs(peer.sin_port));
        }
#else
        FD_ZERO(&fd_read);
        FD_SET(sda, &fd_read);
        if(select(sda + 1, &fd_read, NULL, NULL, NULL)
          < 0) std_err();
#endif

        MSTHREADX(th3, tid3, client, sda);
    }

    close(sdl);
    return(0);
}



MSTHREAD(timeout_thread, void) {
    time_t  oldtime;
    int     i,
            waitme;

    waitme = MSCHKTIME * ONESEC;
    for(;;) {
        sleep(waitme);
        oldtime = time(NULL) - MSTIMEOUT;
        for(i = 0; i < tot; i++) {
            if(server[i].timex < oldtime) {
                remove_server(i);   // remove timed out servers
            }
        }
    }
    return(0);
}



#ifdef MSHBPORT
MSTHREAD(heartbeat_thread, void) {
    struct sockaddr_in  peer;
    u_int   crc;
    int     sd,
            psz,
            on = 1,
            len;
    u_char  hbbuff[MSBUFFSZ + 1];

    peer.sin_addr.s_addr = INADDR_ANY;
    peer.sin_port        = htons(mshbport);
    peer.sin_family      = AF_INET;
    psz                  = sizeof(peer);

    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sd < 0) std_err();
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))
	  < 0) std_err();
    if(bind(sd, (struct sockaddr *)&peer, sizeof(peer))
      < 0) std_err();

    for(;;) {
        len = recvfrom(sd, hbbuff, MSBUFFSZ, 0, (struct sockaddr *)&peer, &psz);
        if(len < 0) continue;
        hbbuff[len] = 0;

        if(show) {
            printf("  "MSIPFMT"   heartbeat\n",
                inet_ntoa(peer.sin_addr),
                ntohs(peer.sin_port));
        }

        crc = 0;
        if(heartbeat(hbbuff, len, peer.sin_addr.s_addr, &peer.sin_port, &crc) < 0) continue;

        if(!crc_check) crc = 0;
        add_server(peer.sin_addr.s_addr, peer.sin_port, crc);
    }

    close(sd);
    return(0);
}
#endif



void remove_server(int curr) {
    tot--;

    if(show) {
        printf("  "MSIPFMT"   removed\n",
        inet_ntoa(*(struct in_addr *)&server[curr].ip),
        ntohs(server[curr].port));
    }

    memmove(&server[curr], &server[tot], sizeof(struct ms_ipport));
}



void remove_request(u_int ip, u_short port) {
    int     i;

    for(i = 0; i < tot; i++) {
        if((server[i].ip == ip) && (server[i].port == port)) {
            remove_server(i);
            break;
        }
    }
}



void add_server(u_int ip, u_short port, u_int crc) {
    int     i;

    if(tot == maxservers) {
        if(show) fputs("  servers list is full\n", stdout);
        return;
    }

    for(i = 0; i < tot; i++) {
        if((server[i].ip == ip) && (server[i].port == port)) return;
    }

    server[tot].ip     = ip;
    server[tot].port   = port;
    server[tot].timex  = time(NULL);
    server[tot].crc    = crc;
    tot++;
    if(show) printf("  added, %u total servers\n", tot);
}



u_int crc32(u_char *data, int size) {
    const static u_int  crctable[] = {
        0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
        0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
        0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
        0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
        0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
        0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
        0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
        0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
        0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
        0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
        0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
        0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
        0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
        0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
        0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
        0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
        0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
        0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
        0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
        0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
        0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
        0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
        0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
        0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
        0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
        0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
        0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
        0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
        0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
        0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
        0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
        0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
        0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
        0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
        0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
        0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
        0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
        0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
        0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
        0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
        0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
        0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
        0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
        0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
        0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
        0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
        0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
        0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
        0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
        0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
        0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
        0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
        0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
        0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
        0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
        0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
        0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
        0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
        0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
        0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
        0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
        0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
        0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
        0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d };
    u_int   crc = 0;

    while(size--) {
        crc = crctable[(*data ^ crc) & 0xff] ^ (crc >> 8);
        data++;
    }
    return(crc);
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
            printf("\nError: Unable to resolv hostname (%s)\n", host);
            exit(1);
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

