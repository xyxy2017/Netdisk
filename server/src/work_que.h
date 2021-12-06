#ifndef __WORK_QUE__
#define __WORK_QUE__
#include "head.h"
typedef struct{                                                                         
    int cmd_len;
    char cmd[1000];
}task_t,*ptask_t;
//还缺少此客户端用户id
typedef struct tag_node{//一次连接,一个客户端
    int new_fd;//客户端fd
    int parent_id;//客户端目前所在文件夹id
    char owner[16];
    char path[1024];//客户端pwd路径
    ptask_t t;//每次传送的任务命令
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
