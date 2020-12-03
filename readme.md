# Linux 网络编程

黑马程序员Linux网络编程学习笔记。主要说明每个c文件是什么作用，需要复习去看代码就行了。



## tcpsocket文件夹

### 1.tcpserver.c

​	tcp服务器端设计

### 2.tcpserver1.c

​	同tcpserver.c.

cfd =accept(lfd,(struct sockaddr *)&clit_addr,&clit_addr_len);唯一的区别就是在这边可以获得客户端的网络地址，ip+port，然后读取的方法如下

```c
 50     printf("client ip=%s,port=%d\n",
 51             inet_ntop(AF_INET,&clit_addr.sin_addr.s_addr,cliet_ip,sizeof(cliet_ip)),
 52             ntohs(clit_addr.sin_port));
```

可以看到ip地址的获取方法利用inet_ntop，port利用ntohs。



### 3.tcpclient.c

​	我自己写的错误版本客户端实现。

### 4.tcpclient1.c

​	正确版本的客户端实现，值得注意的是在tcpserver.c/tcpserver1.c中，

​	      serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);服务器端地址利用本地可用的任意地址

但是在客户端中，连接只能连接确定的服务器地址 ，所以需要做转换。

​		ret = inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr.s_addr);  

