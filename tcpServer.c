#include "tcpServer.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <sys/socket.h>

#include "linkList.h"
#include "udpServer.h"

typedef struct 
{
    in_addr_t ip;   /*!< TCP 客户端的IP */
    int sockfd;     /*!< TCP 客户端的套字节描述符 */
}tcpServerClient_t;

linkList_t *tcpServerClientList;          /*!< 服务器下的客户端链表 */
sem_t tcpServerSem, tcpServerClientSem;   /*!< 线程创建成功信号量 */
pthread_mutex_t tcpServerLock;            /*!< 发送互斥锁  */


/**
 * @brief  判断本地IP是否符合
 * @param  key 比较的键值
 * @param  src 在链表的值
 * @return 0 成功，其他失败
 */
int tcpServerCmp(void *key, void *src)
{
    tcpServerClient_t* list1 = (tcpServerClient_t *)key;
    tcpServerClient_t* list2 = (tcpServerClient_t *)src;

    if (list1->ip == list2->ip) {
        return 0;
    }
    return -1;
}

/**
 * @brief  TCP 客户端发送线程
 * @param  buf 发送的数据
 * @param  len 发送的数据长度
 * @return 成功返回字符串长度
 */
int tcpServerSend(void *buf, int len)
{
    pthread_mutex_lock(&tcpServerLock);
    int num = linkListGetNum(tcpServerClientList);
    for (int i = 0; i < num; i++) {
        tcpServerClient_t *virtue  = (tcpServerClient_t *)linkListIndexFind(i, tcpServerClientList);
        if (virtue->sockfd > 0) {
            send(virtue->sockfd, buf, len, 0);
        }
    }
    pthread_mutex_unlock(&tcpServerLock);

    return len;
}

/**
 * @brief  TCP 打印客户端的IP
 * @return 
 */
void tcpServerClientPrintf(void)
{
    struct sockaddr_in clientAddr;
    int num = linkListGetNum(tcpServerClientList);
    if (num > 0) {
        printf("TCP 服务器上连接的IP\r\n");
        for (int i = 0; i < num; i++) {
            tcpServerClient_t *virtue  = (tcpServerClient_t *)linkListIndexFind(i, tcpServerClientList);
            clientAddr.sin_addr.s_addr = virtue->ip;
            printf("**** %s ****\r\n", inet_ntoa(clientAddr.sin_addr));
        }
    }
}

/**
 * @brief  TCP服务器的客户端线程
 * @param  arg 线程参数 客户端参数
 * @return 
 */
void *tcpServerClientThread(void *arg)
{
    tcpServerClient_t cVirtue = *((tcpServerClient_t*)arg);

    //说明获取到正确的fd了
    sem_post(&tcpServerClientSem);

    uint8_t *buf = (uint8_t*)malloc(1027);
    if (buf == NULL) {
        pthread_exit(NULL);
        return NULL;
    }

    int len = 0;
    while(1) {
        len = recv(cVirtue.sockfd, buf, 1024, 0);
        if (len <= 0) {
            struct in_addr  v;
            v.s_addr = cVirtue.ip;
            printf("tcpServerClientThread %s \r\n", inet_ntoa(v));
            break;
        } else {
            udpServerTcpClientSend(buf, len);
        }
    }

    //删除当前的
    linkListDel(&cVirtue, tcpServerCmp, tcpServerClientList);

    free(buf);
    pthread_exit(NULL);
}

/**
 * @brief  TCP服务器 线程创建
 * @param  arg 线程参数 服务器参数
 * @return 
 */
void tcpServerThread(void *arg)
{
    int sockfd = *((int*)arg);

    //说明获取到正确的fd了
    sem_post(&tcpServerSem);
    pthread_t id;

    tcpServerClient_t cVirtue;

    int client;
    struct sockaddr_in server_addr;
    struct sockaddr_in clientAddr;
    int addr_len = sizeof(clientAddr);

    while (1) {
        client = accept(sockfd, (struct sockaddr*)&clientAddr, (socklen_t*)&addr_len);
        if(client < 0) {
            perror("accept");
            sleep(1);
            continue;
        }

        cVirtue.ip = clientAddr.sin_addr.s_addr;
        cVirtue.sockfd = client;

        //不存在是进行更新数据
        if (!linkListFind(&cVirtue, tcpServerCmp, tcpServerClientList)) {
            //不进行数据保存

            if (pthread_create(&id, NULL, (void*)tcpServerClientThread, &cVirtue) < 0) {
                continue;
            }
            //等待线程同步结束
            sem_wait(&tcpServerClientSem);
            linkListAppend(&cVirtue, tcpServerClientList);
        }
    }
}

/**
 * @brief  TCP服务器创建
 * @param  ip 加入的IP
 * @param  port 端口
 * @return 0 成功
 */
int tcpServerThreadCreate(int port)
{
    sem_init(&tcpServerSem, 0, 0);
    sem_init(&tcpServerClientSem, 0, 0);
    pthread_mutex_init(&tcpServerLock, NULL);
    tcpServerClientList = linkListCreate(sizeof(tcpServerClient_t));

    int server = 0;
    struct sockaddr_in server_addr;
    pthread_t id;

    if((server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return 0;
    }

    int reuse = 1;
    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt error \r\n");
        return -1;
    } 

    bzero(&server_addr, sizeof(server_addr));
    //初始化服务器端的套接字，并用htons和htonl将端口和地址转成网络字节序
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr =  htonl(INADDR_ANY);

    //对于bind，accept之类的函数，里面套接字参数都是需要强制转换成(struct sockaddr *)
    //bind三个参数：服务器端的套接字的文件描述符，
    if(bind(server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) { 
        perror("bind");
        printf("bind err %s \r\n", inet_ntoa(server_addr.sin_addr));
        return 0;
    }

    //设置客户端数量
    if(listen(server, 10) < 0) {
        perror("listen");
        return 0;
    }

    if (pthread_create(&id, NULL, (void*)tcpServerThread, &server) < 0) {
        return 0;
    }

    //等待线程同步结束
    sem_wait(&tcpServerSem);
    return 0;
}
