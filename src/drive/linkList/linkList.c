#include "linkList.h"
#include "stdio.h"
#include <stdlib.h>

/**
 * @brief  初始化链表
 * @param  size 数据大小
 * @return 
 */
linkList_t *linkListCreate(int size)
{
    linkList_t *handle = NULL;

    handle = (linkList_t*)malloc(sizeof(linkList_t));

    if (handle == NULL) {
        return handle;
    }

    handle->head.data = NULL;
    handle->head.next = &handle->head;
    handle->head.prev = &handle->head;
    handle->size = size;        
    handle->num = 0;

    return handle;
}

/**
 * @brief  创建一个节点
 * @param  handle 链表句柄
 * @return 
 */
linkNode_t *linkListNodeCreate(linkList_t *handle)
{
    linkNode_t *node = NULL;

    node = (linkNode_t *)malloc(sizeof(linkNode_t));
    if (node == NULL) {
        return NULL;
    }

    node->data = (void *)malloc(handle->size);
    if (node->data == NULL) {
        free(node);
        return NULL;
    }

    return node;
}

/**
 * @brief  创建一个节点
 * @param  node 节点
 * @return 
 */
void linkListNodeDel(linkNode_t *node)
{
    free(node->data);
    free(node);
}

/**
 * @brief  把数据加入链表
 * @param  data 数据
 * @param  handle 链表句柄
 * @return 
 */
int linkListAppend(void *data, linkList_t *handle)
{
    linkNode_t *node = NULL;

    node = linkListNodeCreate(handle);
    if (node == NULL) {
        return -1;
    }

    memcpy(node->data, data, handle->size);

    node->next = &handle->head;
    node->prev = handle->head.prev;
    handle->head.prev->next = node;
    handle->head.prev = node;
    handle->num++;

    return 0;
}

/**
 * @brief  插入指定位置
 * @param  index 插入的位置
 * @param  data  插入的数据
 * @param  handle 链表句柄
 * @return 
 */
int linkListIndexInsert(int index, void *data, linkList_t *handle)
{
    linkNode_t *tail = NULL;
    linkNode_t *node = NULL;

    node = linkListNodeCreate(handle);
    if (node == NULL) {
        return -1;
    }

    memcpy(node->data, data, handle->size);

    for (tail = &handle->head; tail->next != &handle->head && index--; tail = tail->next);

    //新的节点添加到tail后面
    node->next = tail->next;
    node->prev = tail;
    tail->next->prev = node;
    tail->next = node;
    handle->num++;

    return 0;
}

/**
 * @brief  删除第一个满足条件
 * @param  key 删除的键值
 * @param  cmp 比较函数，返回0成功，其他失败
 * @param  handle 链表句柄
 * @return 
 */
int linkListDel(void *key, linkListCmp_t *cmp, linkList_t *handle)
{
    linkNode_t *tail = NULL;

    for (tail = handle->head.next; tail != &handle->head; tail = tail->next) {
        if (!cmp(key, tail->data)) {
            tail->next->prev = tail->prev;
            tail->prev->next = tail->next;
            linkListNodeDel(tail);
            handle->num--;

            return 0;
        }
    }
    return -1;
}

/**
 * @brief  删除指定位置
 * @param  index 插入的位置
 * @param  handle 链表句柄
 * @return 
 */
int linkListIndexDel(int index, linkList_t *handle)
{
    linkNode_t *tail = NULL;

    if (index < 0 || index >= handle->num) {
        return -1;
    }

    for (tail = handle->head.next; tail != &handle->head && index--; tail = tail->next)
       ;

    tail->next->prev = tail->prev;
    tail->prev->next = tail->next;
    linkListNodeDel(tail);
    handle->num--;

    return 0;
}

/**
 * @brief  删除所有符合条件的
 * @param  key 删除的键值
 * @param  cmp 比较函数，返回0成功，其他失败
 * @param  handle 链表句柄
 * @return 
 */
int linkListAllDel(void *key, linkListCmp_t *cmp, linkList_t *handle)
{
    linkNode_t *tail = NULL;
    linkNode_t *save = NULL;

    for (tail = handle->head.next; tail != &handle->head; tail = save) {
        save = tail->next;
        if (!cmp(key, tail->data)) {
            tail->next->prev = tail->prev;
            tail->prev->next = tail->next;
            linkListNodeDel(tail);
            handle->num--;
        }
    }

    return 0;
}

/**
 * @brief  按照关键字查找
 * @param  key 比较的键值
 * @param  cmp 比较函数，返回0成功，其他失败
 * @param  handle 链表句柄
 * @return 
 */
void *linkListFind(void *key, linkListCmp_t *cmp, linkList_t *handle)
{
    linkNode_t *tail = NULL;

    for (tail = handle->head.next; tail != &handle->head; tail = tail->next) {
        if (cmp(key, tail->data) == 0) {
            return tail->data;
        }
    }
    return NULL;
}

/**
 * @brief  按照位置查找
 * @param  index 查找的位置
 * @param  handle 链表句柄
 * @return 
 */
void *linkListIndexFind(int index, linkList_t *handle)
{
    linkNode_t *tail = NULL;

    if (index < 0 || index >= handle->num) {
        return NULL;
    }

    for (tail = handle->head.next; tail != &handle->head && index--; tail = tail->next)
        ;
    
    return tail->data;
}

/**
 * @brief  按照关键字查找
 * @param  key 比较的键值
 * @param  cmp 比较函数，返回0成功，其他失败
 * @param  handle 链表句柄
 * @return 
 */
void *linkListAllFind(void *key, linkListCmp_t *cmp, linkList_t *handle)
{
    linkList_t *ind = NULL;
    linkNode_t *tail = NULL;

    ind = linkListCreate(handle->size);
    if (ind == NULL) {
        return NULL;
    }

    for (tail = handle->head.next; tail != &handle->head; tail = tail->next) {
        if (cmp(key, tail->data) == 0) {
            linkListAppend(tail->data, ind);
        }
    }
    
    return tail->data;
}

/**
 * @brief  排序
 * @param  cmp 排序比较
 * @param  handle 链表句柄
 * @return 
 */
void *linkListSort(linkListCmp_t *cmp, linkList_t *handle)
{
    linkNode_t *val1 = NULL, *val2 = NULL;
    void *tmp = NULL;

    tmp = (void *)malloc(handle->size);

    for (val1 = handle->head.next; val1 != &handle->head; val1 = val1->next) {
        for (val2 = val1->next; val2 != &handle->head; val2 = val2->next) {
            if (cmp(val1->data, val2->data) > 0) {
                #if 0
                    tmp = val1->data;
                    val1->data = val2->data;
                    val2->data = tmp; 
                #else
                    memmove(tmp, val1->data, handle->size);
                    memmove(val1->data, val2->data, handle->size);
                    memmove(val2->data, tmp, handle->size);
                #endif
            }
        }
    }
    free(tmp);
}

/**
 * @brief  遍历整个列表
 * @param  handle 链表句柄
 * @param  op 输出函数
 * @param  flag 0 逆序，1正序
 * @return 
 */
void linkListTravel(linkList_t *handle, linkListOp_t *op, int flag)
{
    linkNode_t *tail = NULL;

    //逆序
    if (!flag) {
        for (tail = handle->head.next; tail != &handle->head; tail = tail->next) {
            //打印数据
            op(tail->data);
        }
    } else {
        for (tail = handle->head.prev; tail != &handle->head; tail = tail->prev) {
            op(tail->data);
        }
    }
}

/**
 * @brief  销毁链表
 * @param  handle 链表句柄
 * @return 
 */
void linkListDestroy(linkList_t *handle)
{
    linkNode_t *tail = NULL, *save = NULL;

    for (tail = handle->head.next; tail != &handle->head; tail = save) {
        save = tail->next;
        memset(tail->data, 0, handle->size);
        linkListNodeDel(tail);
    }
    memset(handle, 0, sizeof(linkList_t));
    free(handle);
}

/**
 * @brief  清空链表
 * @param  handle 链表句柄
 * @return 
 */
void linkListClran(linkList_t *handle)
{
    linkNode_t *tail = NULL, *save = NULL;

    for (tail = handle->head.next; tail = &handle->head; tail = save) {
        save = tail->next;
        memset(tail->data, 0, handle->size);
        linkListNodeDel(tail);
    }

    // memset(handle, 0, sizeof(handle));
    // handle->head.next = NULL;
    // handle->head.prev = NULL;

    // free(handle);
    handle->head.data = NULL;
    handle->head.next = &handle->head;
    handle->head.prev = &handle->head;  
    handle->num = 0;
}

/**
 * @brief  获取链表数量
 * @param  handle 链表句柄
 * @return 
 */
int linkListGetNum(linkList_t *handle)
{
    return handle->num;
}


