#include "udpServer.h"
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <semaphore.h>

#include "linkList.h"
#include "own_queue.h"
#include "localIp.h"
#include "tcpServer.h"

typedef struct 
{
    in_addr_t ip;   /*!< udp服务器接受到数据的IP 代表远端的IP */
    int port;       /*!< 端口 */
    int sockfd;     /*!< TCP客户端的套字节描述符 */
}tcpClien_t;

linkList_t *tcpClienList;       /*!< 客户端链表 */
sem_t tcpClienSem;              /*!< 线程创建成功信号量，防止在创建客户端的时候线程关闭 */
pthread_mutex_t tcpClienLock;   /*!< 发送互斥锁  */

/**
 * @brief  判断本地IP是否符合
 * @param  key 比较的键值
 * @param  src 在链表的值
 * @return 
 */
static int tcpClienCmp(void *key, void *src)
{
    tcpClien_t* list1 = (tcpClien_t *)key;
    tcpClien_t* list2 = (tcpClien_t *)src;

    if (list1->ip == list2->ip) {
        return 0;
    }
    return -1;
}

/**
 * @brief  udp接收到的IP转发到TCP客户端
 * @param  buf 发送的数据
 * @param  len 发送数据长度
 * @return 发送的数据长度
 */
int udpServerTcpClientSend(void *buf, int len)
{
    pthread_mutex_lock(&tcpClienLock);

    int num = linkListGetNum(tcpClienList);
    for (int i = 0; i < num; i++) {
        tcpClien_t *virtue  = (tcpClien_t *)linkListIndexFind(i, tcpClienList);
        if (virtue->sockfd > 0) {
            send(virtue->sockfd, buf, len, 0);
        }
    }
    
    pthread_mutex_unlock(&tcpClienLock);
    return len;
}

/**
 * @brief  udp接收到的IP然后连接诶上TCP ip
 * @return 
 */
void udpServerTcpClientPintf(void)
{
    struct sockaddr_in clientAddr;


    int num = linkListGetNum(tcpClienList);
    if (num > 0) {
        printf("TCP 客户端 IP\r\n");
        for (int i = 0; i < num; i++) {
            tcpClien_t *virtue  = (tcpClien_t *)linkListIndexFind(i, tcpClienList);
            clientAddr.sin_addr.s_addr = virtue->ip;
            printf("**** %s ****\r\n", inet_ntoa(clientAddr.sin_addr));
        }
    }
}


/**
* @brief    TCP 客户端线程
* @param	arg 线程参数
* @return	无
*/
void *udpServerTcpClientThread(void *arg)
{
    tcpClien_t virtue = *((tcpClien_t *)arg);
    struct sockaddr_in serverAddr;

    if((virtue.sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        sem_post(&tcpClienSem);
        return NULL;
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = virtue.port;
    serverAddr.sin_addr.s_addr = virtue.ip;

    if(connect(virtue.sockfd , (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect");
        sem_post(&tcpClienSem);
        return NULL;
    }

    //加入客户端，保证发送数据
    linkListAppend(&virtue, tcpClienList);
    sem_post(&tcpClienSem);

    printf("连接到主机... %s %d \n", inet_ntoa(serverAddr.sin_addr), virtue.port);
    uint8_t* buf = (uint8_t *)malloc(2048);
    int len = 0;
    while(1) {
        len = recv(virtue.sockfd , buf, 2048, 0);
        if (len <= 0) {
            printf("tcp Client close %s \r\n", inet_ntoa(serverAddr.sin_addr));
            break;
        } else {
            tcpServerSend(buf, len);
        }
    }

    //删除当前的
    linkListDel(&virtue, tcpClienCmp, tcpClienList);
    free(buf);
    pthread_exit(NULL);
    close(virtue.sockfd );
}

/**
 * @brief  UDP广播数据
 * @param  ip 加入的IP
 * @param  port 端口
 * @param  data 发送的数据
 * @param  len 发送数据
 * @return 
 */
void udpServerBroadcastSend(in_addr_t ip, int port, uint8_t* data, uint16_t len)
{
    struct sockaddr_in addr;
    const int opt = 1;

    int sock;
    if ( (sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return;
    }

    //广播
    int nb = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
    if (nb < 0) {
        perror("setsockopt");
        return;
    }
    bzero(&addr, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = port;

    //设置广播
    addr.sin_addr.s_addr = ip | 0xFF;

    //绑定指定的IP
    sendto(sock, data, len, 0, (struct sockaddr *)(&addr), sizeof(struct sockaddr_in));
    // printf("sock sendto %d %s \r\n",  size, inet_ntoa(addr.sin_addr));
    close(sock);
}

/**
 * @brief  UDP服务器线程
 * @param	arg 线程参数
 * @return 
 */
void *udpServerThread(void *arg)
{
    int port = *((int*)arg);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    struct sockaddr_in clientAddr;
    memset(&clientAddr,0,sizeof(clientAddr));
    
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

    socklen_t len = sizeof(clientAddr);
    char buff[256];
    size_t n;
    int num = 0;
    in_addr_t ip;
    tcpClien_t tclien;
    pthread_t id;

    //创建一个防止同时发送锁
    pthread_mutex_init(&tcpClienLock, NULL);

    //创建一个链表，保存数据
    tcpClienList = linkListCreate(sizeof(tcpClien_t));
    //创建一个信号量
    sem_init(&tcpClienSem, 0, 0);

    while(1) {
        n = recvfrom(sock, buff, 256, 0, (struct sockaddr*)&clientAddr, &len);
        if (n > 0) {

            //这个是本地IP
            if (localIpJudge(clientAddr.sin_addr.s_addr) == 0) {
                continue;
            }

            //广播到所有的端口
            num = localIpNum();
            for (int i = 0; i < num; i++) {
                ip = localIpGet(i);

                //转发条件：
                //相同网段不进行转发
                if ((ip & (~0xFF)) == (clientAddr.sin_addr.s_addr & (~0xFF))) {
                    continue;
                }
                
                //转发给所有的,除了相同网段的
                udpServerBroadcastSend(ip, port, buff, n);
            }

            tclien.ip = clientAddr.sin_addr.s_addr;
            tclien.port = port;
            //查找当前的是否存在
            if (!linkListFind(&tclien, tcpClienCmp, tcpClienList)) {
                if (pthread_create(&id, NULL, (void*)udpServerTcpClientThread, &tclien) < 0) {
                    continue;
                }
                //等待线程同步结束
                sem_wait(&tcpClienSem);
            }
        }
    }
}

/**
 * @brief  UDP服务器创建
 * @param  port 端口
 * @return 
 */
int udpServerCreate(int port)
{
    static int portSave;
    pthread_t id;

    portSave = port;
    pthread_create(&id, NULL, (void*)udpServerThread, &portSave);
}
