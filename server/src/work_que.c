#include "work_que.h"
//初始化队列
int que_init(pque_t pq,int capacity)
{
    bzero(pq,sizeof(que_t));
    pq->que_capacity = capacity;
    pthread_mutex_init(&pq->mutex,NULL);
    return 0;
}
//从队列中获取元素
int que_get(pque_t pq,pnode_t *ppcur)
{
    if(pq->que_size){
        //头部删除法从队列里取元素
        *ppcur = pq->que_head;
        pq->que_head = pq->que_head->pnext;
        pq->que_size--;
        return 0;//成功获取元素
    }else{
        return 1;//没有拿到元素
    }
}
//插入元素到队列中
int que_set(pque_t pq,pnode_t pcur){
    if(!pq->que_size){
        pq->que_head = pcur;
        pq->que_tail = pcur;
    }else{
        pq->que_tail->pnext = pcur;
        pq->que_tail = pcur;
    }
    pq->que_size++;
    return 0;
}


