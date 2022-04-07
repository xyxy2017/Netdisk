#include "factory.h"
#define STR_LEN 8
int account_log(pnode_t p){
    int flag;
    int success_flag;
    passwd_t pa;
    ppasswd_t passwd=&pa;
    char ciphertext[128]={0};
    //char username[16]={0}, plaintext[32]={0}, salt[12]={0};
    //char ciphertext[128]={0};
    while(1){
        bzero(passwd, sizeof(pa));
        bzero(ciphertext, sizeof(ciphertext));
        recv(p->new_fd, &flag, sizeof(int), 0);//注册or登录
        recv(p->new_fd, passwd->username, sizeof(passwd->username), 0);//用户名
        strcpy(p->owner, passwd->username);//赋值用户名
        //printf("passwd->username=%s\n", passwd->username);
        if(flag==1){//用户注册
            gen_salt(passwd->salt);
            send(p->new_fd, passwd->salt, strlen(passwd->salt), 0);//发送盐值
            recv(p->new_fd, passwd->ciphertext, sizeof(passwd->ciphertext), 0);//接收密文
            sql_regist(conn, passwd);
            strcpy(p->owner, passwd->username);
            break;
        }else if(flag==2){
            sql_salt(conn, passwd);
            send(p->new_fd, passwd->salt, strlen(passwd->salt), 0);
            recv(p->new_fd, ciphertext, sizeof(ciphertext), 0);
            //printf("recv text=%s\n", ciphertext);
            sql_ciphertext(conn, passwd);
            if(!strcmp(ciphertext, passwd->ciphertext)){
                printf("登录成功\n");
                success_flag=0;
                send(p->new_fd, &success_flag, sizeof(int), 0);
                strcpy(p->owner, passwd->username);
                break;
            }else{
                printf("密码错误\n");
                success_flag=-1;
                send(p->new_fd, &success_flag, sizeof(int), 0);
            }
        }
    }
    return 0;
}
//生成salt值
void gen_salt(char *salt)
{
    char str[STR_LEN+1]={0};
    int i, flag;
    srand(time(NULL));//通过时间函数设置随机数种子，使得每次运行结果随机。
    for(i = 0; i < STR_LEN; i ++)
    {
        flag = rand()%3;
        switch(flag)
        {
        case 0:
            str[i] = rand()%26 + 'a';
            break;
        case 1:
            str[i] = rand()%26 + 'A';
            break;
        case 2:
            str[i] = rand()%10 + '0';
            break;
        }
    }
    strcpy(salt, "$6$");//使用SHA-512加密算法
    strcat(salt, str);
    printf("str=%s, salt=%s\n", str, salt);//输出生成的随机数。
}
int check_log(char *salt){

}

