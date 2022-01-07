#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "own_queue.h"
#include "linkList.h"
#include "localIp.h"
#include "tcpServer.h"
#include "udpServer.h"

#define PORT_REMOTE_DEFAULT 8080

int main(int argc, char *argv[])
{
    int port = PORT_REMOTE_DEFAULT;
    if (argc > 1) {
        int val = atoi(argv[1]);
        if (val > 100 && val < 65535) {
            port = val;
        }
    }

    int time = 0;

    localIpUpdate();
    udpServerCreate(port);
    tcpServerThreadCreate(port);

    while(1) {
        if (time++ >=5 ) {
            time = 0;
            localIpUpdate();
            tcpServerClientPrintf();
            udpServerTcpClientPintf();
        }
        sleep(1);
    }
    return 0;
}


