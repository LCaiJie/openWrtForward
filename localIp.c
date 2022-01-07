#include "localIp.h"
#include "linkList.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include <pthread.h>

typedef struct
{
    pthread_mutex_t lock;   /*!< 互斥锁 */
    linkList_t *list;       /*!< 链表 */
}localIp_t;

localIp_t localIp;

/**
 * @brief  判断本地IP是否符合
 * @param  key 比较的键值
 * @param  src 在链表的值
 * @return 
 */
int localIpCmp(void *key, void *src)
{
    in_addr_t ip1 = *((in_addr_t *)key);
    in_addr_t ip2 = *((in_addr_t *)src);

    if (ip1 == ip2) {
        return 0;
    }
    return -1;
}

/**
 * @brief  查找接口的IP地址
 * @param  handle 返回链表句柄
 * @return 
 */
void localIpGetList(localIp_t *handle)
{
#if 0
    struct ifaddrs * ifAddrStruct = NULL;
    struct ifaddrs * ifa = NULL;
    void * tmpAddrPtr = NULL;

    getifaddrs(&ifAddrStruct);
    int n = 0;

    printf("localIpFaddrs \r\n");

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
          printf("sa_family %d %d \r\n", ifa->ifa_addr->sa_family, AF_INET);  
        if (ifa->ifa_addr->sa_family == AF_INET) { // Check it is
            // a valid IPv4 address
            tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            in_addr_t ip = inet_addr(addressBuffer);
            
            if ((ip & 0x7f000001) == 0x7f000001) {
                continue;
            }
            //不存在是进行更新数据
            if (!linkListFind(&ip, localIpCmp, handle->list)) {
                pthread_mutex_lock(&handle->lock);
                linkListAppend(&ip, handle->list);
                pthread_mutex_unlock(&handle->lock);
            }
        }
    }

    printf("ifAddrStruct \r\n");
    if (ifAddrStruct != NULL) {
        freeifaddrs(ifAddrStruct);
    }
#else

    int sockfd;
	struct ifconf ifc;
	char buf[1024]={0};
	char ipbuf[20]={0};
	struct ifreq *ifr;
 
	ifc.ifc_len = 1024;
	ifc.ifc_buf = buf;
 
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0)
	{
	    printf("socket error\n");
		return;
	}

	ioctl(sockfd, SIOCGIFCONF, &ifc);
	ifr = (struct ifreq*)buf;
 
	for(int i=(ifc.ifc_len/sizeof(struct ifreq)); i > 0; i--) {

        in_addr_t ip = ((struct sockaddr_in *)&ifr->ifr_addr)->sin_addr.s_addr;
        if ((ip & 0x7f000001) == 0x7f000001) {
            ifr = ifr +1;
            continue;
        }

        //不存在是进行更新数据
        if (!linkListFind(&ip, localIpCmp, handle->list)) {
            pthread_mutex_lock(&handle->lock);
            linkListAppend(&ip, handle->list);
            pthread_mutex_unlock(&handle->lock);
        }

		ifr = ifr +1;
	}
    
#endif
}

/**
 * @brief  本地IP更新
 * @return 
 */
void localIpUpdate(void)
{
    if (localIp.list == NULL) {
        pthread_mutex_init(&localIp.lock, NULL);
        localIp.list = linkListCreate(sizeof(in_addr_t));
    } 
    localIpGetList(&localIp);
}

/**
 * @brief  获取本地IP的数量
 * @return 存在的IP
 */
int localIpNum(void)
{
    int num = 0;

    if (localIp.list) {
        pthread_mutex_lock(&localIp.lock);
        num = linkListGetNum(localIp.list);
        pthread_mutex_unlock(&localIp.lock);
    }
    return num;
}

/**
 * @brief  获取本地IP
 * @param  index 按照位置查找
 * @return ip地址
 */
in_addr_t localIpGet(int index)
{
    in_addr_t ip = 0;

    if (localIp.list) {
        pthread_mutex_lock(&localIp.lock);
        void *data = linkListIndexFind(index, localIp.list);
        if (data != NULL) {
            ip = *((in_addr_t*)data);
        }
        pthread_mutex_unlock(&localIp.lock);
    }

    return ip;
}

/**
 * @brief  判断IP是否是本地IP，返回0是本地IP
 * @return 0 是本地IP
 */
int localIpJudge(in_addr_t ip)
{
    if (linkListFind(&ip, localIpCmp, localIp.list) != NULL) {
        return 0;
    }

    return 1;
}
