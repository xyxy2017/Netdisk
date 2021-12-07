#include "head.h"

int tcp_init(int *sfd,char *argv[])
{
    printf("ip=%s,port=%s\n",argv[1],argv[2]);
    *sfd = socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(*sfd,-1,"socket");
    struct sockaddr_in ser_addr;
    bzero(&ser_addr,sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr(argv[1]);
    ser_addr.sin_port = htons(atoi(argv[2]));
    int resue = 1;
    int ret;
    //设置地址端口重用
    ret = setsockopt(*sfd,SOL_SOCKET,SO_REUSEADDR,&resue,sizeof(int));
    ERROR_CHECK(ret,-1,"setsockopt");
    ret = bind(*sfd,(struct sockaddr*)&ser_addr,sizeof(ser_addr));
    ERROR_CHECK(ret,-1,"bind");
    listen(*sfd,10);
    return 0;
}

