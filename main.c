#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>

#include "own_queue.h"
#include "linkList.h"


#define PORT_REMOTE 8081

own_queue_t ComputerQueue = {0};
own_queue_t ControlQueue = {0};

//TCP 服务器列表参数
typedef struct
{
    in_addr_t ip;               /*!< IP 服务器 */
}tcp_server_t;

//最多创建8个客户端
tcp_server_t tcp_server[8] = {0};
int tcp_server_num = 0;

typedef struct {
    in_addr_t ip;
    pthread_t id;
}client_virtue_t;

//TCP 客户端
typedef struct
{
    client_virtue_t virtue[4];
    int num;        
}tcp_client_t;
tcp_client_t tcp_client = {0};

/**
*@brief		获取默认接口的IP地址
*@param		无
*@return	无
*/
void getiFaddrs(in_addr_t addr[8])
{
    struct ifaddrs * ifAddrStruct = NULL;
    struct ifaddrs * ifa = NULL;
    void * tmpAddrPtr = NULL;

    getifaddrs(&ifAddrStruct);
    int n = 0;

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa ->ifa_addr->sa_family==AF_INET) { // Check it is
            // a valid IPv4 address
            tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            in_addr_t ip = inet_addr(addressBuffer);
            addr[n++] = ip;
            // printf("IPv4 %s IP Address %s\n", ifa->ifa_name, addressBuffer);
        }
        else if (ifa->ifa_addr->sa_family==AF_INET6) { // Check it is
            // a valid IPv6 address
            tmpAddrPtr = &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            // printf("AF_INET6 %d.%d.%d.%d\r\n", ifa ->ifa_addr->sa_data);
            // printf("IPv6 %s IP Address %s\n", ifa->ifa_name, addressBuffer);
        }
    }
    if (ifAddrStruct != NULL)
        freeifaddrs(ifAddrStruct);
}

/**
*@brief		UDP数据转发
*@param		无
*@return	无
*/
void udpForwardAll(in_addr_t *ip, struct sockaddr_in clientAddr, uint8_t* data, uint16_t len)
{
    struct sockaddr_in addr;

    for (uint8_t i = 0; i < 8; i++) {
        //和本地IP一致时不进行 
        if (ip[i] == clientAddr.sin_addr.s_addr) {
            return;
        }
    }

    for (uint8_t i = 0; i < 8; i++) {
        //127开头的IP不进行转发、IP为0时错误、和发送的IP相同的也不进行转发
        if (((ip[i] & 0x7f000000) == 0x7f000000) || 
            (ip[i] == 0) ||
            (ip[i] & (~0xFF)) == (clientAddr.sin_addr.s_addr & (~0xFF))) {
            continue;
        }

        const int opt = 1;
        int sock;
        if ( (sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("socket");
            continue;
        }

        int nb = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
        if (nb < 0) {
            perror("setsockopt");
            continue;
        }
        bzero(&addr, sizeof(struct sockaddr_in));
        addr.sin_family = AF_INET;
        addr.sin_port = PORT_REMOTE;
        //设置广播
        addr.sin_addr.s_addr = ip[i] | 0xFF;

        //绑定指定的IP
        ssize_t size = sendto(sock, data, len, 0, (struct sockaddr *)(&addr), sizeof(struct sockaddr_in));
        printf("sock sendto %d %s \r\n",  size, inet_ntoa(addr.sin_addr));
        close(sock);
    }
}

/**
*@brief		TCP服务器的客户端线程
*@param		无
*@return	无
*/
void *TcpServerClientSendThread(void *arg)
{
    int fd = *((int*)arg);
    uint8_t buf[512] = {0};
    uint16_t len = 0;
    int ret = 0;

    while (1) {
        len = own_queue_out(&ComputerQueue, buf, 512);
        if (len > 0) {
            ret = send(fd, buf, len, 0);
            if (ret < 0) {
                break;
            }
        } 
        else {
            usleep(1000*10);
        }
    }
    pthread_exit(NULL);
}

/**
*@brief		TCP服务器的客户端线程
*@param		无
*@return	无
*/
void *TcpServerClientThread(void *arg)
{
    int client = *((int*)arg);
    uint8_t *buf = (uint8_t*)malloc(2048);
    int num = 0;
    pthread_t id;

    if (buf == NULL) {
        pthread_exit(NULL);
        return NULL;
    }

    pthread_create(&id, NULL, (void*)TcpServerClientSendThread, &client);

    while (1) {
        num = recv(client, buf, 2048, 0);
        if (num <= 0) {
            break;
        }
        printf("fd %d %d\r\n", client, num);
        own_queue_enter(&ControlQueue, buf, num);
    }

    pthread_cancel(id);
    free(buf);
    pthread_exit(NULL);
}

/**
*@brief		需要往电脑发送数据，数据来源控制板
*@param		无
*@return	无
*/
void *TcpClientSendThread(void *arg)
{
    int fd = *((int*)arg);
    uint8_t buf[512] = {0};
    uint16_t len = 0;
    int ret = 0;

    while (1) {
        len = own_queue_out(&ControlQueue, buf, 512);
        if (len > 0) {
            ret = send(fd, buf, len, 0);
            if (ret < 0) {
                break;
            }
        } 
        else {
            usleep(1000*10);
        }
    }
    pthread_exit(NULL);
}

/**
*@brief		TCP客户端线程 (从电脑获取数据)
*@param		无
*@return	无
*/
void *TcpClientReadThread(void *arg)
{
    int fd;

    client_virtue_t *virtue = (client_virtue_t *)arg;
    tcp_client.num++;

    pthread_t id;
    
    struct sockaddr_in serverAddr;

    if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return NULL;
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = PORT_REMOTE;
    serverAddr.sin_addr.s_addr = virtue->ip;

    if(connect(fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect");
        return NULL;
    }
    pthread_create(&id, NULL, (void*)TcpClientSendThread, &fd);

    printf("连接到主机...\n");
    uint8_t* buf = (uint8_t *)malloc(2048);
    int len = 0;
    while(1) {
        len = recv(fd, buf, 2048, 0);
        if (len <= 0) {
            break;
        } else {
            printf("len : %d \r\n", len);
            own_queue_enter(&ComputerQueue, buf, len);
        }
    }

    pthread_cancel(id);
    free(buf);
    pthread_exit(NULL);
    close(fd);

    virtue->ip = 0;
    virtue->id = 0; 
    return 0;
}

/**
*@brief		TCP服务器，等待客户端连接
*@param		无
*@return	无
*/
void *WaitForDinnerThread(void *arg)
{
    in_addr_t ip = *((in_addr_t*)arg);
    pthread_t id;
    printf("WaitForDinnerThread ip： %x \r\n", ip);

    int server, client;
    struct sockaddr_in server_addr;
    struct sockaddr_in clientAddr;
    int addr_len = sizeof(clientAddr);

    if((server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return NULL;
    }
    
    bzero(&server_addr, sizeof(server_addr));
    //初始化服务器端的套接字，并用htons和htonl将端口和地址转成网络字节序
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT_REMOTE;
    
    //ip可是是本服务器的ip，也可以用宏INADDR_ANY代替，代表0.0.0.0，表明所有地址
    server_addr.sin_addr.s_addr = ip;
    //对于bind，accept之类的函数，里面套接字参数都是需要强制转换成(struct sockaddr *)
    //bind三个参数：服务器端的套接字的文件描述符，
    if(bind(server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) { 
        perror("connect");
        printf("bind err %s \r\n", inet_ntoa(server_addr.sin_addr));
        return NULL;
    }

    //设置客户端数量
    if(listen(server, 3) < 0) {
        perror("listen");
        return NULL;
    }

    while (1) {
        client = accept(server, (struct sockaddr*)&clientAddr, (socklen_t*)&addr_len);
        if(client < 0) {
            perror("accept");
            continue;
        }
        printf("IP %d is %s\n", client, inet_ntoa(clientAddr.sin_addr));    
        pthread_create(&id, NULL, (void*)TcpServerClientThread, &client);
    }
}

/**
*@brief		创建TCP服务器
*@param		无
*@return	无
*/
void CreateTcpServer(in_addr_t *ip, in_addr_t client)
{
    int i = 0, j = 0;
    pthread_t id;

    for (i = 0; i < 8; i++) {
        if (ip[i] == 0 || ip[i] == 0x7F000001) {
            continue;
        }

        int find = 0;
        for (j = 0; j < 8; j++) {
            if (tcp_server[j].ip == 0) {
                continue;
            }
            if (ip[i] == tcp_server[j].ip) {
                find = 1;
                break;
            }
        }

        //全部都新建
        if (find == 0) {
            tcp_server[tcp_server_num].ip = ip[i];
            pthread_create(&id, NULL, (void*)WaitForDinnerThread, &tcp_server[tcp_server_num].ip);
            tcp_server_num++;
        }
    }
}

/**
*@brief		获取(电脑端)远程连接的IP,使用UDP功能，等待连接数据
*@param		无
*@return	无
*/
void *getRemoteIpThread(void)
{
    //TCP服务器
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = PORT_REMOTE;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int sock;
    if ( (sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return 0;
    }

    //port bind to server
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 0;
    }

    char buff[64];
    struct sockaddr_in clientAddr;
    memset(&clientAddr,0,sizeof(clientAddr));
    size_t n;
    socklen_t len = sizeof(clientAddr);
    int find = 0;

    while (1)
    {
        n = recvfrom(sock, buff, 64, 0, (struct sockaddr*)&clientAddr, &len);
        //有客户端连接进来
        if (n > 0) {
            buff[n] = 0;
            // printf("%s %u \n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
            find = 0;
            for (uint8_t i = 0; i < 8 ; i++) {
                if (tcp_server[i].ip == clientAddr.sin_addr.s_addr) {
                    find = 1;
                }
            }
            //本地IP不进行处理
            if (find == 1) {
                continue;
            }

            //获取本地的IP数据
            in_addr_t ip[8] = {0};
            getiFaddrs(ip);

            //创建TCP服务器
            CreateTcpServer(ip, clientAddr.sin_addr.s_addr);

            //UDP转发到所有的另外IP
            udpForwardAll(ip, clientAddr, buff, n);


            find = 0;
            //连接到TCP服务器
            for (uint8_t i = 0; i < 4; i++) {
                if (tcp_client.virtue[i].ip == clientAddr.sin_addr.s_addr) {
                    find = 1;
                }
            }

            //已经存在了，不在进行处理，或者已经满了
            if (find == 1 || tcp_client.num >= 4) {
                continue;
            }
            tcp_client.virtue[tcp_client.num].ip = clientAddr.sin_addr.s_addr;
            pthread_create(&tcp_client.virtue[tcp_client.num].id, NULL, (void*)TcpClientReadThread, &tcp_client.virtue[tcp_client.num]);

        } else {
            perror("recv");
            return 0;
        }
    }
}


int getLocalInfo(void)
{
    int fd;
    int interfaceNum = 0;
    struct ifreq buf[16];
    struct ifconf ifc;
    struct ifreq ifrcopy;
    char mac[16] = {0};
    char ip[32] = {0};
    char broadAddr[32] = {0};
    char subnetMask[32] = {0};

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");

        close(fd);
        return -1;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = (caddr_t)buf;
    if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc))
    {
        interfaceNum = ifc.ifc_len / sizeof(struct ifreq);
        printf("interface num = %dn", interfaceNum);
        while (interfaceNum-- > 0)
        {
            printf("ndevice name: %sn", buf[interfaceNum].ifr_name);

            //ignore the interface that not up or not runing  
            ifrcopy = buf[interfaceNum];
            if (ioctl(fd, SIOCGIFFLAGS, &ifrcopy))
            {
                printf("ioctl: %s [%s:%d]n", strerror(errno), __FILE__, __LINE__);

                close(fd);
                return -1;
            }

            //get the mac of this interface  
            if (!ioctl(fd, SIOCGIFHWADDR, (char *)(&buf[interfaceNum])))
            {
                memset(mac, 0, sizeof(mac));
                snprintf(mac, sizeof(mac), "%02x%02x%02x%02x%02x%02x",
                    (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[0],
                    (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[1],
                    (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[2],

                    (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[3],
                    (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[4],
                    (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[5]);
                printf("device mac: %sn", mac);
            }
            else
            {
                printf("ioctl: %s [%s:%d]n", strerror(errno), __FILE__, __LINE__);
                close(fd);
                return -1;
            }

            //get the IP of this interface  

            if (!ioctl(fd, SIOCGIFADDR, (char *)&buf[interfaceNum]))
            {
                snprintf(ip, sizeof(ip), "%s",
                    (char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_addr))->sin_addr));
                printf("device ip: %sn", ip);
            }
            else
            {
                printf("ioctl: %s [%s:%d]n", strerror(errno), __FILE__, __LINE__);
                close(fd);
                return -1;
            }

            //get the broad address of this interface  

            if (!ioctl(fd, SIOCGIFBRDADDR, &buf[interfaceNum]))
            {
                snprintf(broadAddr, sizeof(broadAddr), "%s",
                    (char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_broadaddr))->sin_addr));
                printf("device broadAddr: %sn", broadAddr);
            }
            else
            {
                printf("ioctl: %s [%s:%d]n", strerror(errno), __FILE__, __LINE__);
                close(fd);
                return -1;
            }

            //get the subnet mask of this interface  
            if (!ioctl(fd, SIOCGIFNETMASK, &buf[interfaceNum]))
            {
                snprintf(subnetMask, sizeof(subnetMask), "%s",
                    (char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_netmask))->sin_addr));
                printf("device subnetMask: %sn", subnetMask);
            }
            else
            {
                printf("ioctl: %s [%s:%d]n", strerror(errno), __FILE__, __LINE__);
                close(fd);
                return -1;

            }
        }
    }
    else
    {
        printf("ioctl: %s [%s:%d]n", strerror(errno), __FILE__, __LINE__);
        close(fd);
        return -1;
    }

    close(fd);

    return 0;
}

typedef struct
{
    uint32_t ip;
}ip_data_t;


typedef void (linkListOp_t)(void *);
void err_printf(void *data)
{
    ip_data_t *ip = (ip_data_t *)data;

    printf("ip: %d \r\n", ip->ip);
}

int delIp(void *data, void *src)
{
    ip_data_t *ip = (ip_data_t *)data;
    ip_data_t *sip = (ip_data_t *)src;

    if (ip->ip == sip->ip) {
        return 0;
    }
    return 1;
}


int main(char *argc, char *argv[])
{
    pthread_t id1;
    pthread_create(&id1, NULL, (void*)getRemoteIpThread, NULL );

    linkList_t *list = linkListCreate(sizeof(ip_data_t));
    ip_data_t ip;
    ip.ip = 1024;

    linkListAppend(&ip, list);

    for (uint8_t i = 0; i < 15; i++) {
        ip.ip = 1024 + i;
        linkListAppend(&ip, list);
    }
    linkListTravel(list, err_printf, 0);

    printf("*****************\r\n");

    for (uint8_t i = 5; i < 10; i++) {
        ip.ip = 1024 + i;
        linkListDel(&ip, delIp, list);
    }
    linkListTravel(list, err_printf, 0);

    printf("list num %d \r\n", list->num);
    linkListDestroy(list);


    uint8_t *buf1 = (uint8_t*)malloc(5120);
    uint8_t *buf2 = (uint8_t*)malloc(5120);
    own_queue_init(&ComputerQueue, buf1, 1, 5120);
    own_queue_init(&ControlQueue, buf1, 1, 5120);
    while(1) {

        
        sleep(1);
    }
    return 0;
}


