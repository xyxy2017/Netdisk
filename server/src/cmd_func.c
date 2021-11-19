//MSG_NOSIGNAL忽略信号
#include "factory.h"
#define CMDLEN 1000//命令最长长度
int task_handle(pnode_t p){
    int ret;
    int new_fd = p->new_fd;
    task_t t;//任务结构体
    bzero(&t,sizeof(t));
    ret=recv(new_fd,&t.cmd_len,sizeof(int),0);//接收命令长度，必须用recv
    if(ret==0||t.cmd_len==0){
        printf("client exit.\n");
        close(p->new_fd);
        //pthread_exit(NULL);
        return -1;
    }
    //printf("cmd_len=%d\n",t.cmd_len);
    recvn(new_fd,t.cmd,t.cmd_len);//接收命令
    p->t=&t;
    if(!strncmp(t.cmd,"cd",2)){
        do_cd(p);
    }else if(!strncmp(t.cmd,"ls",2)){
        do_ls(p);
    }else if(!strncmp(t.cmd,"puts",4)){
        do_puts(p);
    }else if(!strncmp(t.cmd,"gets",4)){
        do_gets(p);
    }else if(!strncmp(t.cmd,"remove",6)){
        do_remove(p);
    }else if(!strncmp(t.cmd,"pwd",3)){
        do_pwd(p);
    }else {
        printf("error cmd.\n");
    }
    return 0;
}
int do_cd(pnode_t p){
    printf("do cd\n");
    int ret;
    char *filename;
    ret=get_filename(p->t,filename);
    if(ret==-1){
        return -1;
    }

    
    return 0;
}
int do_ls(pnode_t p){
    printf("do ls\n");
    return 0;
}
int do_puts(pnode_t p){
    printf("do puts\n");
    return 0;
}
int do_gets(pnode_t p){
    printf("do gets\n");
    return 0;
}
int do_remove(pnode_t p){
    printf("do remove\n");
    return 0;
}
int do_pwd(pnode_t p){
    printf("do pwd\n");
    return 0;
}
//new_fd为要发送到的文件
int tran_file(pnode_t pnode)
{
    int new_fd = pnode->new_fd;
    int ret,fd;
    fd = open(FILENAME,O_RDONLY);//打开要发送的文件
    ERROR_CHECK(fd,-1,"open");
    train_t t;
    bzero(&t,sizeof(t));
    t.data_len = strlen(FILENAME);
    strcpy(t.buf,FILENAME);
    ret = send(new_fd,&t,4+t.data_len,MSG_NOSIGNAL);//发送文件名
    ERROR_CHECK(ret,-1,"send filename");
    //printf("len=%d,filename=%s\n",t.data_len,t.buf);
    //获得并发送文件大小
    struct stat statbuf;
    ret = fstat(fd,&statbuf);
    t.data_len = sizeof(statbuf.st_size);
    memcpy(t.buf,&statbuf.st_size,t.data_len);
    ret = send(new_fd,&t,4+t.data_len,MSG_NOSIGNAL);
    ERROR_CHECK(ret,-1,"send file length");
    //发送文件内容，使用sendfile,一次性发送  
    ret = sendfile(new_fd,fd,NULL,statbuf.st_size);
    ERROR_CHECK(ret,-1,"sendfile");
    return 0;
}
int recvn(int sfd,void *pstart,int len){
    int total=0,ret;
    char *p = (char*)pstart;
    while(total<len){
        ret = recv(sfd,p+total,len-total,MSG_WAITALL);//ret为0时说明对端已断开            
        if(ret==0){
            return -1; 
        }   
        total += ret;
    }   
    return 0;
}
int get_filename(task_t *p,char *filename){
    char *str=p->cmd;
    char str1[CMDLEN]={0};
    int j,k=0;
    for(j=0;j<p->cmd_len;++j){
        if (str[j] != ' ') {
            str1[k] = str[j];
            ++k;
        }
        else {
            while (str[j] == str[j + 1]) {
                ++j;

            }
            str1[k] = str[j];
            ++k;
        }
    }
    int num=0,i;
    char buf[CMDLEN]={0};
    for (j=0; j<k; ++j) {
        if (str1[j] != ' ') {
            i=0;
            bzero(buf,sizeof(buf));
            while ((str1[j] != ' ')&&str1[j]) {
                printf("%c", str1[j]);
                buf[i++]=str1[j];
                ++j;
            }
            ++num;
            if(num==2){
                strcpy(filename,buf);
            }
            printf("\n");
        }
    }
    if(num>2){
        printf("wrong cmd,too many file.\n");
        return -1;
    }else{
        printf("filename=%s\n",filename);
        return 0;
    }
}
