#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <winsock2.h>
#include "tello.h"

static struct sockaddr_in recv_addr, cmd_addr, send_addr;
static int sock, cmdsock;

void udp_open (void) {
	fprintf(stderr, "udp_open()\n");
	// start winsock and open a socket
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,0), &wsaData);
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    cmdsock = socket(AF_INET, SOCK_DGRAM, 0);

    // receiver
    u_long val=1;
    ioctlsocket(sock, FIONBIO, &val); // non-blocking
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(9000);
    recv_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sock, (struct sockaddr *)&recv_addr, sizeof(recv_addr));

    // receiver for commands
    val=1;
    ioctlsocket(cmdsock, FIONBIO, &val); // non-blocking
    cmd_addr.sin_family = AF_INET;
    cmd_addr.sin_port = htons(65008);
    cmd_addr.sin_addr.s_addr = INADDR_ANY;
    bind(cmdsock, (struct sockaddr *)&cmd_addr, sizeof(cmd_addr));

    // sender
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(8889);
    send_addr.sin_addr.s_addr = inet_addr("192.168.10.1");
}

void udp_close (void) {
	fprintf(stderr, "udp_close()\n");
    close(sock);
    WSACleanup();
}

void udp_send (const char *cmd) {
    sendto(sock, cmd, strlen(cmd), 0, (struct sockaddr *)&send_addr, sizeof(send_addr));
}

void cmd_parse(const char *str, int size) {
	int i, prevp = 0;
	for (i=0 ; i<size ; i++) {
		if (str[i] == ';') {
			char marker[100], buf[100];
			int cursize = i-prevp;
			int id, tx, ty, tz;
			memcpy(marker, str+prevp, cursize);
			marker[cursize] = '\0';

			sscanf(marker, "%d,%d,%d,%d", &id, &tx, &ty, &tz);

			tello_marker_detected(id, tx, ty, tz);
			prevp = i+1;
		}
	}
}

void cmd_recv(void) {
	char buf[2048] = {0};
	int size;
	size = recv(cmdsock, buf, sizeof(buf), 0);
	if (size > 0) {
		cmd_parse(buf, size);
	}
}

void udp_poll (void) {
    char buf[2048];
    int size;
    memset(buf, 0, sizeof(buf));
    size = recv(sock, buf, sizeof(buf), 0);
    if (size > 0) {
    	tello_recv( buf );
    }
    cmd_recv();
}



