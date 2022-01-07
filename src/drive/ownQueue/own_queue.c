/*
 * own_queue.c
 *
 *  Created on: Jul 17, 2020
 *      Author: caijie
 */
#include "own_queue.h"


/**
 * @name: 创建队列
 * @param {queue 队列，data 存储指针，data_len 数据长度，number 数据数量}
 * @return: 0成功，1失败
 */
 int own_queue_init(own_queue_t *queue, void *data, uint16_t data_len, uint16_t number)
{
     queue->buff = data;
     if (queue->buff == NULL) {
        return 1;
    }
    queue->data_len = data_len;
    queue->front = 0;
    queue->rear = 0;
    queue->max_size = number;
    queue->flag = 1;
    
    pthread_mutex_init(&queue->olock, NULL);
    pthread_mutex_init(&queue->elock, NULL);

    return 0;
}
/**
 * @name: 销毁队列
 * @param {queue 队列}
 * @return:
 */
void own_queue_free(own_queue_t *queue)
{
    if (!queue || queue->flag == 1) {
        queue->buff = NULL;
        queue->flag = 0;
    }
}
/**
 * @name: 清空队列
 * @param {queue 队列}
 * @return:
 */
void own_queue_empty(own_queue_t *queue)
{
    if (queue) {
        queue->rear = queue->front;
    }
}
/**
 * @name: 输出队列里剩余长度
 * @param {queue 队列}
 * @return:
 */
uint16_t own_queue_is_full(own_queue_t *queue)
{
    if (!queue || queue->buff == NULL) {
        return 0;
    }
    return queue->max_size - (queue->front - queue->rear);
}
/**
 * @name: 输出队列里的数据长度
 * @param {queue 队列}
 * @return:
 */
uint16_t own_queue_size(own_queue_t *queue)
{
    if (!queue || queue->buff == NULL) {
        return 0;
    }
    return queue->front - queue->rear;
}
/**
 * @name: 输出队列里队头位置
 * @param {queue 队列}
 * @return:
 */
uint16_t own_queue_front(own_queue_t *queue)
{
    return (queue->front%queue->max_size);
}
/**
 * @name: 输出队列里队尾位置
 * @param {queue 队列}
 * @return:
 */
uint16_t own_queue_rear(own_queue_t *queue)
{
    return (queue->rear%queue->max_size);
}
/**
 * @name: 数组复制
 * @param {queue 队列}
 * @return:
 */
void own_queue_cpy(uint8_t *destin, uint8_t *source, uint32_t n)
{
    for(uint32_t i = 0; i < n; i++) {
        *destin++ = *source++;
    }
}
/**
 * @name: 入队列
 * @param {queue 队列， buf 数组指针，len 数据长度}
 * @return: 0成功，1没有初始化，2队列已满,，3队列已满但已经放入一部分数据
 */
int own_queue_enter(own_queue_t *queue, void *buf, uint16_t len)
{
    uint16_t trace_len = 0, offset = 0;
    int ret = 0;
    pthread_mutex_lock(&queue->elock);

    if (!queue || queue->flag != 1) {
        pthread_mutex_unlock(&queue->elock);
        return 1;
    }

    trace_len = own_queue_is_full(queue);
    if (trace_len == 0) {
        pthread_mutex_unlock(&queue->elock);
        return 2;
    }
    if (len <= trace_len) {
        trace_len = len;
    } else {
        ret = 3;
    }

    if (queue->front > 9223372036854775807U) {
        queue->front = own_queue_front(queue);
        queue->rear = own_queue_rear(queue);
    }

    if ((queue->max_size - own_queue_front(queue)) < trace_len) {
        memcpy((queue->buff + own_queue_front(queue) * queue->data_len),
                buf,
                (queue->data_len * (queue->max_size - own_queue_front(queue))));

        offset = queue->max_size - own_queue_front(queue);
        trace_len = trace_len - (queue->max_size - own_queue_front(queue));
        queue->front += (queue->max_size - own_queue_front(queue));
    }

    memcpy((queue->buff + own_queue_front(queue)*queue->data_len),
            (buf + offset * queue->data_len),
            (trace_len * queue->data_len));

    queue->front +=trace_len;

    pthread_mutex_unlock(&queue->elock);
    return ret;
}
/**
 * @name: 出队列
 * @param {queue 队列， buf 数组指针，len 数据长度}
 * @return: 0没有初始化或者没有数据
 */
uint16_t own_queue_out(own_queue_t *queue,  void *buf, uint16_t len)
{
    uint16_t trace_len = 0;
    uint16_t offset = 0;
    pthread_mutex_lock(&queue->olock);
    
    if (!queue || queue->flag != 1) {
        pthread_mutex_unlock(&queue->olock);
        return 0;
    }

    trace_len = own_queue_size(queue);
    if (trace_len > len) {
        trace_len = len;
    }

    if (trace_len == 0) {
        pthread_mutex_unlock(&queue->olock);
        return 0;
    }
    if ((queue->max_size - own_queue_rear(queue)) < trace_len) {
        if (buf != NULL) {
            memcpy(buf,
                    (queue->buff + own_queue_rear(queue) * queue->data_len),
                    ((queue->max_size - own_queue_rear(queue)) * queue->data_len));
        }
        offset = (queue->max_size - own_queue_rear(queue));
        trace_len = trace_len - offset;
        queue->rear += (queue->max_size - own_queue_rear(queue));
    }
    if (buf != NULL) {
        memcpy((buf+offset * queue->data_len),
                (queue->buff + own_queue_rear(queue) * queue->data_len),
                (trace_len * queue->data_len));
    }
    queue->rear += trace_len;
    pthread_mutex_unlock(&queue->olock);
    return trace_len + offset;
}
