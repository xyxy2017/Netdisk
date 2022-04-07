#include "head.h"
int get_filename(char *cmd,int cmd_len,char *filename){
    char *str=cmd;
    char str1[CMDLEN]={0};
    int j,k=0;
    for(j=0;j<cmd_len;++j){
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
            while (str1[j]!='\n' && str1[j] != ' ' && str1[j]) {
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
int get_file(int sfd,char *filepath){
    int ret;
    int pipe_size=32768;
    //创建文件
    int fd = open(filepath,O_RDWR|O_CREAT,0666);//为文件名
    ERROR_CHECK(fd,-1,"open");
    //接收文件长度
    off_t file_size,download_size=0,last_size=0;
    ret = recv(sfd,&file_size,sizeof(file_size),0);//接收文件长度
    printf("filesize=%ld\n",file_size);
    time_t start,end;
    start = time(NULL);
    int fds[2];
    ret = pipe(fds);
    ERROR_CHECK(ret,-1,"pipe");
    while(download_size<file_size){
        ret = splice(sfd,NULL,fds[1],NULL,pipe_size,SPLICE_F_MOVE|SPLICE_F_MORE);//32
        ERROR_CHECK(ret,-1,"splice");
        download_size+=ret;
        ret = splice(fds[0],NULL,fd,NULL,pipe_size,SPLICE_F_MOVE|SPLICE_F_MORE);
        ERROR_CHECK(ret,-1,"splice");
    }   
    end = time(NULL);
    printf("use time=%ld\n",end-start);
    close(fd);
    return 0;
}
int tran_file(int sfd,char *filename){
    int fd,ret;                                                                   
    fd=open(filename,O_RDONLY);//打开文件
    ERROR_CHECK(fd,-1,"open");
    struct stat statbuf;
    ret = fstat(fd,&statbuf);
    off_t data_len = statbuf.st_size;//获得文件长度
    ret = send(sfd,&data_len,sizeof(data_len),MSG_NOSIGNAL);//发送文件长度
    ERROR_CHECK(ret,-1,"send");
    //发送文件内容，使用sendfile,一次性发送  
    ret = sendfile(sfd,fd,NULL,statbuf.st_size);
    ERROR_CHECK(ret,-1,"sendfile");
    close(fd);
}
