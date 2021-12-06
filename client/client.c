#include "func.h"
//管道默认大小
#define pipe_size 32768
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
    int sfd = socket(AF_INET,SOCK_STREAM,0);//sfd为服务器文件描述符
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
    char recv_buf[1000] = {0};
    char username[16]={0},plaintext[32]={0},salt[12]={0};
    char *ciphertext;
    //test Compute_file_md5
    //char *file_path = "temp";
    //char md5_str[MD5_STR_LEN + 1];
    //ret = Compute_file_md5(file_path, md5_str);//计算md5接口                                 
    //printf("ret=%d\n",ret);
    //if (0 == ret)
    //{
    //    printf("[file - %s] md5 value:\n", file_path);
    //    printf("%s\n", md5_str);

    //}
    
    //char ciphertext[128]={0};
    //登录或注册
    while(1){
        printf("请输入：1.注册 2.登录\n");
        int flag;
        int success_flag;
        scanf("%d",&flag);//选择注册或者登录
        bzero(username,sizeof(username));
        bzero(plaintext,sizeof(plaintext));
        bzero(salt,sizeof(salt));
        if(flag==1){//注册
            send(sfd,&flag,sizeof(int),0);
            printf("请输入用户名:\n");
            read(STDIN_FILENO,username,sizeof(username));//输入用户名
            send_len=strlen(username)-1;//不发送回车符
            //send(sfd,&send_len,sizeof(int),0);
            //sendn(sfd,username,send_len);
            send(sfd,username,send_len,0);
            //recv(sfd,&recv_len,sizeof(int),0);
            //recvn(sfd,salt,recv_len);
            recv(sfd,salt,sizeof(salt),0);
            printf("请输入密码:\n");
            read(STDIN_FILENO,plaintext,sizeof(plaintext));//输入密码
            ciphertext=crypt(plaintext,salt);
            //printf("%s,%ld\n",ciphertext,strlen(ciphertext));
            //strcpy(ciphertext,crypt(plaintext,salt));//两种赋值方法
            send(sfd,ciphertext,strlen(ciphertext),0);//发送密文
            break;
        }else if(flag==2){//登录
            send(sfd,&flag,sizeof(int),0);
            printf("输入用户名:\n");
            read(STDIN_FILENO,username,sizeof(username));
            send_len=strlen(username)-1;
            send(sfd,username,send_len,0);//发送用户名
            recv(sfd,salt,sizeof(salt),0);//接收盐值
            printf("请输入密码:\n");
            read(STDIN_FILENO,plaintext,sizeof(plaintext));//输入密码
            ciphertext=crypt(plaintext,salt);//生成密文
            send(sfd,ciphertext,strlen(ciphertext),0);//发送密文
            recv(sfd,&success_flag,sizeof(int),0);
            if(!success_flag){
                printf("登录成功\n");
                break;
            }else{
                printf("密码错误,登录失败\n");
            }
        }else{
            continue;
        }
    }
    //开始发送命令
    while(1){
        bzero(send_buf,sizeof(send_buf));
        bzero(recv_buf,sizeof(recv_buf));
        read(STDIN_FILENO,send_buf,sizeof(send_buf));
        send_len = strlen(send_buf);//暂时用strlen判断指令长度，之后要用trim_space函数来判断指令长度
        //printf("send_len=%d\n",send_len);
        send(sfd,&send_len,sizeof(int),0);//发送命令长度
        sendn(sfd,send_buf,send_len);//发送命令
        if(!strncmp(send_buf,"cd",2)){
            printf("get cd\n");

        }else if(!strncmp(send_buf,"ls",2)){
            printf("get ls\n");
            recv(sfd,&recv_len,sizeof(int),0);
            recvn(sfd,recv_buf,recv_len);
            printf("%s\n",recv_buf);
        }else if(!strncmp(send_buf,"puts",4)){
            printf("get puts\n");
            int file_exit_flag;
            char filename[100]={0};
            get_filename(send_buf,send_len,filename);//获得文件名字
            char file_path[300]={0};
            strcpy(file_path,filename);
            char md5_str[MD5_STR_LEN+1];
            ret = Compute_file_md5(file_path,md5_str);//计算md5值
            if(0==ret){
                printf("[file - %s] md5 value:\n",file_path);
                printf("%s\n",md5_str);
            }//输出md5值
            ret=send(sfd,md5_str,strlen(md5_str),0);//发送md5值，服务器判断
            ERROR_CHECK(ret,-1,"send");
            recv(sfd,&file_exit_flag,sizeof(int),0);//接收flag
            if(file_exit_flag==0){//不存在相同文件，需传送
                tran_file(sfd,filename);
                //int fd;
                //fd=open(filename,O_RDONLY);//打开文件
                //ERROR_CHECK(fd,-1,"open");
                //struct stat statbuf;
                //ret = fstat(fd,&statbuf);
                //off_t data_len = statbuf.st_size;//获得文件长度
                //ret = send(sfd,&data_len,sizeof(data_len),MSG_NOSIGNAL);//发送文件长度
                //ERROR_CHECK(ret,-1,"send");
                ////发送文件内容，使用sendfile,一次性发送  
                //ret = sendfile(sfd,fd,NULL,statbuf.st_size);
                //ERROR_CHECK(ret,-1,"sendfile");
                //close(fd);
            }else{//服务器有相同文件，实现妙传
                printf("秒传，finish puts.\n");
            }

        }else if(!strncmp(send_buf,"gets",4)){
            printf("get gets\n");
            char filename[100]={0};
            get_filename(send_buf,send_len,filename);//获得文件名字
            //get file
            get_file(sfd,filename);

        }else if(!strncmp(send_buf,"remove",6)){
            printf("get remove\n");

        }else if(!strncmp(send_buf,"pwd",3)){
            printf("get pwd\n");
            recv(sfd,&recv_len,sizeof(int),0);
            recvn(sfd,recv_buf,recv_len);
            printf("%s\n",recv_buf);
            
        }else if(!strncmp(send_buf,"\n",1)){
            continue;
        }else {
            printf("error cmd.\n");
            continue;
        }   
    }
    return 0;
}
