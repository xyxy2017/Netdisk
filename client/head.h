#ifndef __HEAD__
#define __HEAD__
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <syslog.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <memory.h>
#define ARGS_CHECK(argc,num){if(argc!=num){\
    fprintf(stderr,"Args Error!\n");\
    return -1;\
}}
#define ERROR_CHECK(ret,num,msg){if(ret==num){\
    perror(msg);\
    return -1;\
}}
#define RECVN_ERROR_CHECK(ret,num,msg){if(ret==num){\
    return -1;\
}}
#define THREAD_ERROR_CHECK(ret,funcname){if(ret!=0){\
    printf("%s failed %s\n",funcname,strerror(ret));\
    return -1;\
}}
#define FILENAME "file1"
#define CMDLEN 1000
//typedef struct{
//    pid_t pid;
//    int fd;
//    short busy_flag;
//}process_data_t;
//typedef struct{
//    int data_len;
//    char buf[1000];
//}train_t;
//int get_filename(char *cmd,int cmd_len,char *filename);
#endif
