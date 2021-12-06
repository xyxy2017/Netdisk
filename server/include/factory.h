#ifndef __FACTORY__
#define __FACTORY__
#include "head.h"
#include "work_que.h"
#define FILENAME "file1"//加双引号
typedef void* (*thread_func_t)(void*);
typedef struct{
    que_t que;//队列
    pthread_cond_t cond;//条件变量
    pthread_t *pthid;//指向pthid的起始地址
    int pthread_num;//线程数目
    thread_func_t download_thread;//下载线程入口函数
    int start_flag;//线程池启动标志，0未启动
}factory_t;
typedef struct{
    int data_len;//火车头，存储车厢上有多少数据
    char buf[1000];//车厢数据
}train_t;
int factory_init(factory_t*,int,int);
void* thread_func(void *p);
int factory_start(factory_t*);
int tran_file(int);
int tcp_init(int*,char*[]);
#endif
