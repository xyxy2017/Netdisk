# 目录格式
## server
include存放头文件
src存放源代码
filesystem存放所有客户端上传的文件，以文件MD5值作为文件名
## client
客户端
# 配置
数据库连接在./server/src/sql\_func.c中的sql\_conn中修改，
password修改成自己的root登录密码，好像要提前修改数据库，
database为本项目所用数据库名称，提前创建好。
数据库设计见截图。
# 启动
启动服务器命令
./server 服务器ip 服务器端口 线程数目 队列容量
启动客户端
./client 服务器ip 服务器端口

服务端有许多数据库测试输出行。
# 命令
```
cd 进入目录
ls 显示当前目录下文件
puts 上传文件
gets 下载文件
remove 删除文件
pwd 显示当前路径
mkdir 创建文件夹
```

