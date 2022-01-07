/*
 * udpServer.h
 *
 *      Author: caijie
 */

#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include <netinet/in.h>

/**
 * @brief  TCP 客户端发送线程
 * @param  buf 发送的数据
 * @param  len 发送的数据长度
 * @return 成功返回字符串长度
 */
int tcpServerSend(void *buf, int len);

/**
 * @brief  TCP服务器创建
 * @param  ip 加入的IP
 * @param  port 端口
 * @return 0 成功
 */
int tcpServerThreadCreate(int port);

/**
 * @brief  TCP 打印客户端的IP
 * @return 
 */
void tcpServerClientPrintf(void);

#endif /* TCP_SERVER_H_ */
