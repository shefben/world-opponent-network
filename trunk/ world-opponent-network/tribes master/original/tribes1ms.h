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

#define MSNAME      "Tribes 1"
#define MSVER       "0.1"       // plugin version
#define MSPORT      28000       // master server query/hearbeat port
#define BUFFSZ      1024
#define MSTIMEOUT   120         // 2 minutes

#define PUTSTR(x)   *p++ = sizeof(x) - 1; \
                    memcpy(p, x, sizeof(x) - 1); \
                    p += sizeof(x) - 1;
#define SWAP16(x)   ((x >> 8) | (x << 8))
#define SERVERSPCK  63



MSTHREAD(client, int sock) {
    struct  sockaddr_in peer;
    int     i,
            psz,
            len;
    u_short *pck;
    u_char  buff[BUFFSZ],
            *p;

    peer.sin_addr.s_addr = INADDR_ANY;
    peer.sin_port        = htons(msport);
    peer.sin_family      = AF_INET;
    psz                  = sizeof(peer);

    len = recvfrom(sock, buff, BUFFSZ, 0, (struct sockaddr *)&peer, &psz);
    if(len <= 0) return(0);

    if(show) {
        printf("  "MSIPFMT"\n",
            inet_ntoa(peer.sin_addr),
            ntohs(peer.sin_port));
    }

    if(buff[1] == 0x03) {
        buff[0] = 0x10;
        buff[1] = 0x06;
        buff[2] = 1;    // counter
        buff[3] = (tot / SERVERSPCK) + ((tot % SERVERSPCK) ? 1 : 0);
        // buff[4] and buff[5] are the ID
        buff[6] = 0;
        buff[7] = 0x66;
        p = buff + 8;
        PUTSTR("Tribes Master");
        PUTSTR("No MOTD");
        pck = (u_short *)p;
        p += 2;

        for(i = 0; i < tot;) {
            *p++ = 6;
            *(u_int *)p = server[i].ip;
            p += 4;
            *(u_short *)p = SWAP16(server[i].port);
            p += 2;
            i++;    // do it now!

            if(!(i % SERVERSPCK)) {
                *pck = htons(SERVERSPCK);
                sendto(sock, buff, p - buff, 0, (struct sockaddr *)&peer, sizeof(peer));
                buff[2]++;
                pck = (u_short *)(buff + 8);
                p = buff + 10;
            }
        }
        if(i %= SERVERSPCK) {
            *pck = htons(i);
            sendto(sock, buff, p - buff, 0, (struct sockaddr *)&peer, sizeof(peer));
        }

    } else if(buff[1] == 0x05) {
        add_server(peer.sin_addr.s_addr, peer.sin_port, 0);
    }

    return(0);
}


