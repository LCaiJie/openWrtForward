/*
 * own_queue.h
 *
 *  Created on: Jul 17, 2020
 *      Author: caijie
 */

#ifndef LINK_LIST_H_
#define LINK_LIST_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>

//定义函数指针
typedef void (linkListOp_t)(void *);
typedef int (linkListCmp_t)(void *, void *);


/**
 *  链表节点信息结构体
 */
struct linkNode{
    void *data;              /*!< 外部用户传入的数据 */
    struct linkNode *next;   /*!< 指向下一个节点地址 */
    struct linkNode *prev;   /*!< 指向上一个节点地址 */
};

typedef struct {
    struct linkNode head;   /*!< 外部用户传入的数据 */
    int size;               /*!< 类型 数据类型 */
    int num;                /*!< 大小 数据个数 */
}linkList_t;
typedef struct linkNode linkNode_t;

/**
 * @brief  初始化链表
 * @param  size 数据大小
 * @return 
 */
linkList_t *linkListCreate(int size);

/**
 * @brief  创建一个节点
 * @param  handle 链表句柄
 * @return 
 */
linkNode_t *linkListNodeCreate(linkList_t *handle);

/**
 * @brief  创建一个节点
 * @param  node 节点
 * @return 
 */
void linkListNodeDel(linkNode_t *node);

/**
 * @brief  把数据加入链表
 * @param  data 数据
 * @param  handle 链表句柄
 * @return 
 */
int linkListAppend(void *data, linkList_t *handle);

/**
 * @brief  插入指定位置
 * @param  index 插入的位置
 * @param  data  插入的数据
 * @param  handle 链表句柄
 * @return 
 */
int linkListIndexInsert(int index, void *data, linkList_t *handle);

/**
 * @brief  删除第一个满足条件
 * @param  key 删除的键值
 * @param  cmp 比较函数，返回0成功，其他失败
 * @param  handle 链表句柄
 * @return 
 */
int linkListDel(void *key, linkListCmp_t *cmp, linkList_t *handle);

/**
 * @brief  删除指定位置
 * @param  index 插入的位置
 * @param  handle 链表句柄
 * @return 
 */
int linkListIndexDel(int index, linkList_t *handle);

/**
 * @brief  删除所有符合条件的
 * @param  key 删除的键值
 * @param  cmp 比较函数，返回0成功，其他失败
 * @param  handle 链表句柄
 * @return 
 */
int linkListAllDel(void *key, linkListCmp_t *cmp, linkList_t *handle);

/**
 * @brief  按照关键字查找
 * @param  key 比较的键值
 * @param  cmp 比较函数，返回0成功，其他失败
 * @param  handle 链表句柄
 * @return 
 */
void *linkListFind(void *key, linkListCmp_t *cmp, linkList_t *handle);

/**
 * @brief  按照位置查找
 * @param  index 查找的位置
 * @param  handle 链表句柄
 * @return 
 */
void *linkListIndexFind(int index, linkList_t *handle);

/**
 * @brief  按照关键字查找
 * @param  key 比较的键值
 * @param  cmp 比较函数，返回0成功，其他失败
 * @param  handle 链表句柄
 * @return 
 */
void *linkListAllFind(void *key, linkListCmp_t *cmp, linkList_t *handle);

/**
 * @brief  排序
 * @param  cmp 排序比较
 * @param  handle 链表句柄
 * @return 
 */
void *linkListSort(linkListCmp_t *cmp, linkList_t *handle);

/**
 * @brief  遍历整个列表
 * @param  handle 链表句柄
 * @param  op 输出函数
 * @param  flag 0 逆序，1正序
 * @return 
 */
void linkListTravel(linkList_t *handle, linkListOp_t *op, int flag);

/**
 * @brief  销毁链表
 * @param  handle 链表句柄
 * @return 
 */
void linkListDestroy(linkList_t *handle);

/**
 * @brief  清空链表
 * @param  handle 链表句柄
 * @return 
 */
void linkListClran(linkList_t *handle);




#endif /*  */
