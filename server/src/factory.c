#include "factory.h"

int factory_init(factory_t *p,int thread_num,int capacity)
{
    que_init(&p->que,capacity);//初始化队列
    pthread_cond_init(&p->cond,NULL);//初始化条件变量
    p->pthread_num = thread_num;
    p->pthid = (pthread_t*)calloc(thread_num,sizeof(pthread_t));//申请thread_num个线程id大小空间
    p->download_thread = thread_func;//线程入口函数
    p->start_flag = 0;//初始化
    return 0;
}
//线程函数
void* thread_func(void *p){
    factory_t *pfac=(factory_t*)p;//强转为工厂结构体指针
    pque_t pq=&pfac->que;//重新定义一个结构体指针，简化代码
    pnode_t pcur;//保存拿到的任务指针
    int get_success_flag;
    while(1)
    {
        pthread_mutex_lock(&pq->mutex);
        if(!pq->que_size){
            pthread_cond_wait(&pfac->cond,&pq->mutex);
        }
        get_success_flag=que_get(pq,&pcur);//从队列获取任务
        pthread_mutex_unlock(&pq->mutex);
        //printf("%d\n",pcur->new_fd);
        if(get_success_flag==0){
            int ret;
            ret=account_log(pcur);//登录
            if(ret!=-1){
                while(1){
                    ret=task_handle(pcur);
                    if(ret==-1){//返回-1时说明对方断开
                        break;
                    }
                }
            }
            free(pcur);
            pcur=NULL;
        }
    }
}
//启动子线程
int factory_start(factory_t *p){
    if(!p->start_flag){
        int i;
        for(i=0;i<p->pthread_num;++i){
            pthread_create(p->pthid+i,NULL,p->download_thread,p);
        }
        p->start_flag=1;
    }
    return 0;
}

