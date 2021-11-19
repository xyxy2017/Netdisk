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
    sprintf(query,"%s%d",query, p->parent_id);
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
                    printf("%8s ",row[t]);
                }
                printf("\n");
            }
        }else{
            printf("Don't find data\n");
        }
        mysql_free_result(res);
    }
    return 0;
}
//用于cd，返回cd的目录id值
int sql_cd(MYSQL *conn,pnode_t p){
    MYSQL_RES *res;
    MYSQL_ROW row;
    char filename[100]={0};
    int ret=get_filename(p->t,filename);
    if(ret==-1){
        printf("error filename.\n");
        return 0;
    }
    char query[300]="select ID from filesystem where filetype='d' and parent_ID=";
    sprintf(query,"%s%d%s%s%s",query,p->parent_id," and filename='",filename,"'");
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
                    printf("%8s ",row[t]);
                }
                printf("\n");
            }
        }else{
            printf("Don't find data\n");
        }
        mysql_free_result(res);
    }
    return 0;

}
//数据库插入，用于puts
int sql_insert(MYSQL *conn,char *filename){

}
//数据库删除，用于remove
int sql_delete(MYSQL *conn,char *filename){

}
//关闭数据库连接
int sql_close(MYSQL *conn){
    mysql_close(conn);
    return 0;
}
