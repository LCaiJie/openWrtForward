/*
 * udpServer.h
 *
 *      Author: caijie
 */

#ifndef UDP_SERVER_H_
#define UDP_SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/**
 * @brief  udp接收到的IP转发到TCP客户端
 * @param  buf 发送的数据
 * @param  len 发送数据长度
 * @return 发送的数据长度
 */
int udpServerTcpClientSend(void *buf, int len);

/**
 * @brief  UDP服务器创建
 * @param  port 端口
 * @return 
 */
int udpServerCreate(int port);

/**
 * @brief  udp接收到的IP然后连接诶上TCP ip
 * @return 
 */
void udpServerTcpClientPintf(void);

#endif /* UDP_SERVER_H_ */
