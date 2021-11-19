//管道默认大小
#define pipe_size 32768
#include "process.h"
int recvn(int sfd,void *pstart,int len){
    int total=0,ret;
    char *p = (char*)pstart;
    while(total<len){
        //printf("in recvn\n");
        ret = recv(sfd,p+total,len-total,MSG_WAITALL);//ret为0时说明对端已断开
        if(ret==0){
            return -1;
        }
        total += ret;
    }
    return 0;
}
int sendn(int sfd,void *pstart,int send_len){
    int total=0,num;
    char *p = (char*)pstart;
    while(total<send_len){
        num = send(sfd,p+total,send_len-total,0);
        total+=num;
    }
    return total;
}
int main(int argc,char *argv[])
{
    ARGS_CHECK(argc,3);
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(sfd,-1,"socket");
    struct sockaddr_in sev_addr;
    bzero(&sev_addr,sizeof(sev_addr));
    sev_addr.sin_family = AF_INET;
    sev_addr.sin_port = htons(atoi(argv[2]));
    sev_addr.sin_addr.s_addr = inet_addr(argv[1]);
    int ret;
    ret = connect(sfd,(struct sockaddr*)&sev_addr,sizeof(sev_addr));
    ERROR_CHECK(sfd,-1,"accept");
    int read_len,write_len,send_len,recv_len;
    char send_buf[1000] = {0};
    //开始发送命令
    while(1){
        bzero(send_buf,sizeof(send_buf));
        read(STDIN_FILENO,send_buf,sizeof(send_buf));
        send_len = strlen(send_buf);//暂时用strlen判断指令长度，之后要用trim_space函数来判断指令长度
        //printf("send_len=%d\n",send_len);
        send(sfd,&send_len,sizeof(int),0);//发送命令长度
        sendn(sfd,send_buf,send_len);//发送命令
        if(!strncmp(send_buf,"cd",2)){
            printf("get cd\n");

        }else if(!strncmp(send_buf,"ls",2)){
            printf("get ls\n");

        }else if(!strncmp(send_buf,"puts",4)){
            printf("get puts\n");

        }else if(!strncmp(send_buf,"gets",4)){
            printf("get gets\n");

        }else if(!strncmp(send_buf,"remove",6)){
            printf("get remove\n");

        }else if(!strncmp(send_buf,"pwd",3)){
            printf("get pwd\n");

        }else if(!strncmp(send_buf,"\n",1)){
            continue;
        }else {
            printf("error cmd.\n");
            continue;
        }   
    }
    return 0;
}
