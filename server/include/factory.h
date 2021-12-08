#ifndef __FACTORY__
#define __FACTORY__
#include "head.h"
#include "work_que.h"
#include "md5.h"
#define FILENAME "file1"//加双引号
MYSQL *conn;//数据库全局变量
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
typedef struct{
    char username[16];
    char salt[12];
    char ciphertext[128];
}passwd_t,*ppasswd_t;
//typedef struct{
//    int cmd_len;
//    char cmd[1000];
//}task_t;
int factory_init(factory_t*,int,int);
void* thread_func(void *p);
int factory_start(factory_t*);
int tcp_init(int*,char*[]);
//登录账户函数
void gen_salt(char *salt);
int account_log(pnode_t);
//命令处理与实现
int task_handle(pnode_t);
int do_cd(pnode_t);
int do_ls(pnode_t);
int tran_file(pnode_t p,char *filepath);
int get_file(pnode_t p,char *filepath);
int do_puts(pnode_t);
int do_gets(pnode_t);
int do_remove(pnode_t);
int do_pwd(pnode_t);
int do_mkdir(pnode_t);
int recvn(int sfd,void *pstart,int len);
int sendn(int sfd,void *send_buf,int send_len);
int get_filename(task_t *p,char *filename);
//int tran_file(int);
int sql_conn(MYSQL **conn);
int sql_ls(MYSQL *conn,pnode_t p);
int sql_cd(MYSQL *conn,pnode_t p);
int sql_insert(MYSQL *conn,pnode_t p,char *md5);
int sql_mkdir(MYSQL *conn,pnode_t p);
int sql_getmd5(MYSQL *conn,pnode_t p,char *md5);
int judge_owner(MYSQL *conn,char *md5);//返回此文件共享数目
int sql_delete(MYSQL *conn,pnode_t p);
int sql_regist(MYSQL *conn,ppasswd_t passwd);
int sql_salt(MYSQL *conn,ppasswd_t passwd);
int sql_ciphertext(MYSQL *conn,ppasswd_t passwd);
int sql_close(MYSQL *conn);
#endif
