#ifndef __WORK_QUE__
#define __WORK_QUE__
#include "head.h"
typedef struct tag_node{//一个任务
    int new_fd;
    char path[1024];
    struct tag_node *pnext;
}node_t,*pnode_t;
typedef struct queue{
    pnode_t que_head,que_tail;
    int que_capacity;//容量
    int que_size;//接收负载
    pthread_mutex_t mutex;
}que_t,*pque_t;

int que_init(pque_t,int que_capacity);
int que_set(pque_t,pnode_t);
int que_get(pque_t,pnode_t*);
#endif
