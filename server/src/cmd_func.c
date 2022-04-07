//MSG_NOSIGNAL忽略信号
#include "factory.h"
#define CMDLEN 1000//命令最长长度
int task_handle(pnode_t p){
    int ret;
    int new_fd = p->new_fd;
    task_t t;//任务结构体
    bzero(&t, sizeof(t));
    ret=recv(new_fd, &t.cmd_len, sizeof(int), 0);//接收命令长度，必须用recv
    if(ret==0||t.cmd_len==0){
        printf("client exit.\n");
        close(p->new_fd);
        //pthread_exit(NULL);
        return -1;
    }
    recvn(new_fd, t.cmd, t.cmd_len);//接收命令
    p->t=&t;
    if(!strncmp(t.cmd, "cd", 2)){
        do_cd(p);
    }else if(!strncmp(t.cmd, "ls", 2)){
        do_ls(p);
    }else if(!strncmp(t.cmd, "puts", 4)){
        do_puts(p);
    }else if(!strncmp(t.cmd, "gets", 4)){
        do_gets(p);
    }else if(!strncmp(t.cmd, "remove", 6)){
        do_remove(p);
    }else if(!strncmp(t.cmd, "pwd", 3)){
        do_pwd(p);
    }else if(!strncmp(t.cmd, "mkdir", 5)){
        do_mkdir(p);
    }else {
        printf("error cmd.\n");
    }
    return 0;
}
int do_cd(pnode_t p){
    printf("do cd\n");
    int ret;
    ret=sql_cd(conn, p);
    if(ret==-1){//命令错误，找不到目录
        return 0;
    }
    p->parent_id=ret;//修改父id，即进入新目录
    printf("parent_id=%d\n", p->parent_id);
    return 0;
}
int do_ls(pnode_t p){
    printf("do ls\n");
    int ret=sql_ls(conn, p);
    return 0;
}
//client puts file, use func get_file
int do_puts(pnode_t p){
    int ret, file_exit_flag;
    //printf("do puts\n");
    char md5_str[MD5_STR_LEN+1];
    char file_path[300]="../filesystem/";//服务器接收文件放在../filesystem/中
    ret=recv(p->new_fd, md5_str, sizeof(md5_str), 0);//接收md5值
    ERROR_CHECK(ret, -1, "recv");
    strcat(file_path, md5_str);
    printf("file_path=%s\n", file_path);
    if(access(file_path, F_OK)!=-1){//文件存在
        file_exit_flag=1;
        send(p->new_fd, &file_exit_flag, sizeof(int), 0);//实现秒传
    }else{
        file_exit_flag=0;
        send(p->new_fd, &file_exit_flag, sizeof(int), 0);//发送接收flag
        get_file(p, file_path);
    }
    sql_insert(conn, p, md5_str);
    return 0;
}
//
int do_gets(pnode_t p){
    printf("do gets\n");
    char md5_str[MD5_STR_LEN+1];
    sql_getmd5(conn, p, md5_str);//找到文件名字对应的md5值
    char file_path[300]="../filesystem/";//服务器接收文件放在../filesystem/中
    strcat(file_path, md5_str);//获得文件路径
    printf("file_path=%s\n", file_path);
    tran_file(p, file_path);
    return 0;
}
//判断是否有用户在共享此文件，没有则删除filesystem内的文件
int do_remove(pnode_t p){
    printf("do remove\n");
    char md5_str[MD5_STR_LEN+1];
    sql_getmd5(conn, p, md5_str);//找到文件名字对应的md5值
    sql_delete(conn, p);//删除该用户此文件记录
    int ret=judge_owner(conn, md5_str);
    if(ret==0){
        char file_path[300]="../filesystem/";
        strcat(file_path, md5_str);
        ret=remove(file_path);
        ERROR_CHECK(ret, -1, "remove");
    }
    return 0;
}
int do_pwd(pnode_t p){
    printf("do pwd\n");
    printf("%s\n", p->path);
    char send_buf[1000]={0};
    strcpy(send_buf, p->path);
    int send_len=strlen(send_buf);
    send(p->new_fd, &send_len, sizeof(int), 0);
    sendn(p->new_fd, send_buf, send_len);
    return 0;
}
int do_mkdir(pnode_t p){
    sql_mkdir(conn, p);
    printf("finish mkdir\n");
    return 0;
}
//new_fd为要发送到的文件
int tran_file(pnode_t p, char *filepath)
{
    int fd, ret;                                                                       
    fd=open(filepath, O_RDONLY);//打开文件
    ERROR_CHECK(fd, -1, "open");
    struct stat statbuf;                                                                  
    ret = fstat(fd, &statbuf);
    off_t data_len = statbuf.st_size;//获得文件长度
    ret = send(p->new_fd, &data_len, sizeof(data_len), MSG_NOSIGNAL);//发送文件长度
    ERROR_CHECK(ret, -1, "send");
    //发送文件内容，使用sendfile, 一次性发送  
    ret = sendfile(p->new_fd, fd, NULL, statbuf.st_size);
    ERROR_CHECK(ret, -1, "sendfile");
    close(fd);
    return 0;
}
int get_file(pnode_t p, char *filepath){
    int ret;
    int pipe_size=32768;
    //创建文件
    int fd = open(filepath, O_RDWR|O_CREAT, 0666);//以文件md5为文件名
    ERROR_CHECK(fd, -1, "open");
    //接收文件长度
    off_t file_size, download_size=0, last_size=0;
    ret = recvn(p->new_fd, &file_size, sizeof(file_size));//接收文件长度
    printf("filesize=%ld\n", file_size);
    time_t start, end;
    start = time(NULL);
    int fds[2];
    ret = pipe(fds);
    ERROR_CHECK(ret, -1, "pipe");
    while(download_size<file_size){
        ret = splice(p->new_fd, NULL, fds[1], NULL, pipe_size, SPLICE_F_MOVE|SPLICE_F_MORE);//32768是
        ERROR_CHECK(ret, -1, "splice");
        download_size+=ret;
        ret = splice(fds[0], NULL, fd, NULL, pipe_size, SPLICE_F_MOVE|SPLICE_F_MORE);
        ERROR_CHECK(ret, -1, "splice");
    }
    end = time(NULL);
    printf("use time=%ld\n", end-start);
    close(fd);
    return 0;
}

int recvn(int sfd, void *pstart, int len){
    int total=0, ret;
    char *p = (char*)pstart;
    while(total<len){
        ret = recv(sfd, p+total, len-total, MSG_WAITALL);//ret为0时说明对端已断开            
        if(ret==0){
            return -1; 
        }   
        total += ret;
    }   
    return 0;
}
int sendn(int sfd, void *pstart, int send_len){                                   
    int total=0, num;
    char *p = (char*)pstart;
    while(total<send_len){
        num = send(sfd, p+total, send_len-total, 0);
        total+=num;

    }   
    return total;

}
int get_filename(task_t *p, char *filename){
    char *str = p->cmd;
    char str1[CMDLEN]={0};
    int j, k = 0;
    for(j = 0; j < p->cmd_len; ++j){
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
    int num = 0, i;
    char buf[CMDLEN] = {0};
    for (j = 0; j < k; ++j) {
        if (str1[j] != ' ') {
            i = 0;
            bzero(buf, sizeof(buf));
            while (str1[j] != '\n' && str1[j] != ' ' && str1[j]) {
                printf("%c", str1[j]);
                buf[i++] = str1[j];
                ++j;
            }
            ++num;
            if(num == 2){
                strcpy(filename, buf);
            }
            printf("\n");
        }
    }
    if(num > 2){
        printf("wrong cmd, too many file.\n");
        return -1;
    }else{
        printf("filename=%s\n", filename);
        return 0;
    }
}

