#include "factory.h"
//数据库连接
int sql_conn(MYSQL **conn){
    const char* server="localhost";
    const char* user="root";
    const char* password="123";
    const char* database="netdisk";//要访问的数据库名称
    MYSQL *conn_tmp;
    conn_tmp=mysql_init(NULL);
    if(!mysql_real_connect(conn_tmp,server,user,password,database,0,NULL,0))
    {
        printf("Error connecting to database:%s\n",mysql_error(conn_tmp));
        return -1;
    }else{
        printf("Connected...\n");
    }
    *conn=conn_tmp;
    return 0;
}
//数据库读取ls
int sql_ls(MYSQL *conn,pnode_t p)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[300]="select filename from filesystem where parent_ID=";
    sprintf(query,"%s%d%s%s%s",query, p->parent_id," and owner='",p->owner,"'");
    puts(query);//打印sql命令
    char send_buf[1000]={0};
    int send_len;
    unsigned int t;
    t=mysql_query(conn,query); 
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        //  printf("Query made...\n");
        res=mysql_use_result(conn);
        if(res)
        {
            while((row=mysql_fetch_row(res))!=NULL)
            {   
                //printf("num=%d\n",mysql_num_fields(res));//列数
                for(t=0;t<mysql_num_fields(res);t++)//mysql_num_fields获取一行有几列
                {
                    printf("%8s ",row[t]);
                    strcat(send_buf,row[t]);
                    strcat(send_buf," ");
                }
                printf("\n");
            }
            //strcat(send_buf,"\n");
            printf("%s\n",send_buf);
            send_len=strlen(send_buf);
            send(p->new_fd,&send_len,sizeof(int),0);
            sendn(p->new_fd,send_buf,send_len);
        }else{
            printf("Don't find data\n");
        }
        mysql_free_result(res);
    }
    return 0;
}
//用于cd，返回cd的目录id值
//缺少cd ..功能和查找失败无文件夹的功能
int sql_cd(MYSQL *conn,pnode_t p){
    MYSQL_RES *res;
    MYSQL_ROW row;
    int parent_id=-1;//找到的id值，即返回的新的父id值,默认为-1，即未改变
    char filename[100]={0};
    //char *filename;
    int ret=get_filename(p->t,filename);
    if(ret==-1){
        printf("error filename.\n");
        return 0;
    }
    char query[300]={0};
    if(!strcmp(filename,"..")){
        strcpy(query,"select parent_id from filesystem where id=");
        sprintf(query,"%s%d%s%s%s",query,p->parent_id," and owner='",p->owner,"'");

    }else{
        strcpy(query,"select ID from filesystem where filetype='d' and parent_ID=");
        sprintf(query,"%s%d%s%s%s%s%s",query,p->parent_id," and filename='",filename,"' and owner='",p->owner,"'");
    }
    puts(query);//打印sql命令
    unsigned int t;
    t=mysql_query(conn,query); 
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        //  printf("Query made...\n");
        res=mysql_use_result(conn);
        if(res)//这里有问题，无数据仍然进入if，而不是else中，应查看mysql函数具体了解
        {
            while((row=mysql_fetch_row(res))!=NULL)
            {   
                //printf("num=%d\n",mysql_num_fields(res));//列数
                for(t=0;t<mysql_num_fields(res);t++)//mysql_num_fields获取一行有几列
                {
                    printf("%8s ",row[t]);
                    parent_id=atoi(row[t]);
                }
                printf("\n");
            }
        }else{
            printf("Don't find data\n");
        }
        //等改正if(res)问题之后将修改pwd放入if(res)中
        //pwd未改正
        if(parent_id!=-1){
            strcat(p->path,filename);
            strcat(p->path,"/");//修改pwd结果
        }
        mysql_free_result(res);
    }
    //printf("%s\n",p->path);
    return parent_id;

}
int sql_getmd5(MYSQL *conn,pnode_t p,char *md5){
    MYSQL_RES *res;
    MYSQL_ROW row;
    char filename[100]={0};
    get_filename(p->t,filename);
    char query[300]="select md5 from filesystem where parent_ID=";
    sprintf(query,"%s%d%s%s%s%s%s",query, p->parent_id," and owner='",p->owner,"' and filename='",filename,"'");
    puts(query);//打印sql命令
    unsigned int t;
    t=mysql_query(conn,query); 
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        //  printf("Query made...\n");
        res=mysql_use_result(conn);
        if(res)
        {
            while((row=mysql_fetch_row(res))!=NULL)
            {   
                //printf("num=%d\n",mysql_num_fields(res));//列数
                for(t=0;t<mysql_num_fields(res);t++)//mysql_num_fields获取一行有几列
                {
                    printf("%s",row[t]);
                    strcpy(md5,row[t]);
                }
                printf("\n");
            }
            //strcat(send_buf,"\n");
        }else{
            printf("Don't find data\n");
        }
        mysql_free_result(res);
    }
    return 0;
}
//在remove一个文件后判断是否有用户用此文件，返回值为用户数量，为0时删除文件
int judge_owner(MYSQL *conn,char *md5){
    MYSQL_RES *res;
    MYSQL_ROW row;
    int num_owner=0;
    char query[300]="select owner from filesystem where md5='";
    sprintf(query,"%s%s%s",query,md5,"'");
    puts(query);//打印sql命令
    unsigned int t;
    t=mysql_query(conn,query); 
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        //  printf("Query made...\n");
        res=mysql_use_result(conn);
        if(res)
        {
            while((row=mysql_fetch_row(res))!=NULL)
            {   
                //printf("num=%d\n",mysql_num_fields(res));//列数
                for(t=0;t<mysql_num_fields(res);t++)//mysql_num_fields获取一行有几列
                {
                    printf("%s",row[t]);
                }
                printf("\n");
                num_owner++;
            }
            //strcat(send_buf,"\n");
        }else{
            printf("Don't find data\n");
        }
        mysql_free_result(res);
    }
    printf("num_owner=%d\n",num_owner);
    printf("mysql_num_rows()=%ld\n",mysql_num_rows(res));
    return num_owner;
}
//数据库插入，用于puts
//INSERT INTO filesystem(parent_ID,filetype,owner,filename) VALUES(0,'f','jxy','file1');
int sql_insert(MYSQL *conn,pnode_t p,char *md5){
    int ret;
    char filename[100]={0};
    ret=get_filename(p->t,filename);
    char query[300]="INSERT INTO filesystem(filetype,parent_id,filename,md5,owner) VALUES('f','";
    sprintf(query,"%s%d%s%s%s%s%s%s%s",query, p->parent_id,"','",filename,"','",md5,"','",p->owner,"')");
    puts(query);//打印sql命令
    //char send_buf[1000]={0};
    //int send_len;
    unsigned int t;
    t=mysql_query(conn,query); 
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        printf("insert success\n");
    }
    return 0;
}
int sql_mkdir(MYSQL *conn,pnode_t p){
    int ret;
    char filename[100]={0};
    ret=get_filename(p->t,filename);
    char query[300]="INSERT INTO filesystem(filetype,parent_id,filename,owner) VALUES('d','";
    sprintf(query,"%s%d%s%s%s%s%s",query, p->parent_id,"','",filename,"','",p->owner,"')");
    puts(query);//打印sql命令
    //char send_buf[1000]={0};
    //int send_len;
    unsigned int t;
    t=mysql_query(conn,query); 
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        printf("insert success\n");
    }
    return 0;
}
//数据库删除，用于remove
int sql_delete(MYSQL *conn,pnode_t p){
    int ret;
    char filename[100]={0};
    ret=get_filename(p->t,filename);
    char query[300]="DELETE FROM filesystem WHERE parent_ID=";
    sprintf(query,"%s%d%s%s%s%s%s",query, p->parent_id," and filename='",filename,"' and owner='",p->owner,"'");
    puts(query);//打印sql命令
    unsigned int t;
    t=mysql_query(conn,query); 
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        printf("insert success\n");
    }
    return 0;

}
//注册用户
//INSERT INTO usertable(name,salt,passwd) VALUES('xiongda','1231312','1321312321');
int sql_regist(MYSQL *conn,ppasswd_t passwd){
    //MYSQL_RES *res;
    //MYSQL_ROW row;
    int ret;
    char query[300]="INSERT INTO usertable(username,salt,ciphertext) VALUES('";
    sprintf(query,"%s%s%s%s%s%s%s",query,passwd->username,"','",passwd->salt,"','",passwd->ciphertext,"')");
    puts(query);//打印sql命令
    unsigned int t;
    t=mysql_query(conn,query); 
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        printf("insert success\n");
    }
    return 0;
}
//登录
//取盐值
int sql_salt(MYSQL *conn,ppasswd_t passwd){
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[300]="select salt from usertable where username='";
    sprintf(query,"%s%s%s",query, passwd->username,"'");
    puts(query);//打印sql命令
    char salt[14]={0};
    unsigned int t;
    t=mysql_query(conn,query); 
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        res=mysql_use_result(conn);
        if(res)
        {
            while((row=mysql_fetch_row(res))!=NULL)
            {   
                for(t=0;t<mysql_num_fields(res);t++)//mysql_num_fields获取一行有几列
                {
                    printf("%8s ",row[t]);
                    strcat(salt,row[t]);
                }
                printf("\n");
            }
            printf("salt=%s\n",salt);
            strcpy(passwd->salt,salt);
        }else{
            printf("Don't find data\n");
        }
        mysql_free_result(res);
    }
    return 0;
}
//取密文
int sql_ciphertext(MYSQL *conn,ppasswd_t passwd){
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[300]="select ciphertext from usertable where username='";
    sprintf(query,"%s%s%s",query,passwd->username,"'");
    puts(query);//打印sql命令
    char ciphertext[128]={0};
    unsigned int t;
    t=mysql_query(conn,query); 
    if(t!=0)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        //  printf("Query made...\n");
        res=mysql_use_result(conn);
        if(res)
        {
            while((row=mysql_fetch_row(res))!=NULL)
            {   
                for(t=0;t<mysql_num_fields(res);t++)//mysql_num_fields获取一行有几列
                {
                    printf("%8s ",row[t]);
                    strcat(ciphertext,row[t]);
                }
                printf("\n");
            }
            printf("passwd->ciphertext=%s\n",ciphertext);
            strcpy(passwd->ciphertext,ciphertext);
        }else{
            printf("Don't find data\n");
        }
        mysql_free_result(res);
    }
    return 0;
}
//关闭数据库连接
int sql_close(MYSQL *conn){
    mysql_close(conn);
    return 0;
}
