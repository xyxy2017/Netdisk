#include "factory.h"
//MYSQL *conn;

int main(int argc,char *argv[])
{
    if(argc!=5){
        printf("usage:./pthread_poll_server IP PROT THREAD_NUM QUEUE_CAPACITY\n");
        return -1;
    }
    int thread_num = atoi(argv[3]);//线程数目
    int capacity = atoi(argv[4]);//队列容量
    factory_t thread_data;//线程池主数据结构
    factory_init(&thread_data,thread_num,capacity);//线程池初始化
    factory_start(&thread_data);//启动子线程
    int sfd,new_fd;
    pnode_t pnew;//
    pque_t pq = &thread_data.que;//队列指针，便于操作
    tcp_init(&sfd,argv);//开启监听
    sql_conn(&conn);//连接数据库
    //sql_ls(conn,"1");//测试功能
    /*
    int ret;
    const char *file_path = "temp";
    char md5_str[MD5_STR_LEN + 1];
    ret = Compute_file_md5(file_path, md5_str);//计算md5接口
    printf("ret=%d\n",ret);
    if (0 == ret)
    {
        printf("[file - %s] md5 value:\n", file_path);
        printf("%s\n", md5_str);
    }*/
    while(1)
    {
        new_fd=accept(sfd,NULL,NULL);
        pnew = (pnode_t)calloc(1,sizeof(node_t));
        pnew->new_fd = new_fd;//新建一个连接客户端
        pnew->parent_id=0;//客户端初始在根目录，根目录为0
        strcpy(pnew->path,"/");//初始化pwd 
        pthread_mutex_lock(&pq->mutex);//加锁
        que_set(pq,pnew);//放连接客户端到队列中
        pthread_mutex_unlock(&pq->mutex);//解锁
        pthread_cond_signal(&thread_data.cond);//signal唤醒一个子线程处理一个客户端
    }
    return 0;
}

