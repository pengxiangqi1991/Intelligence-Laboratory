/*
使用socket编程实现一个简单的文件服务器。客户端程序实现put功能(将一个文件从本地传到文件服务器)
和get功能(从文件服务器取一远程文件存为本地文件)。客户端和文件服务器不在同一台机器上。
put [-h hostname] [-p portname] local_filename remote_filename 
get [-h hostname] [-p portname] remote_filename local_filename 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#define BUFF 1024
char buf[BUFF];
void client_put(int confd,char *local_filename, char *remote_filename);
void client_get(int confd,char *local_filename, char *remote_filename);
int main(int argc, char *argv[])
{
  int confd;
  struct sockaddr_in server_addr;

  if(argc < 5)
  {
    printf("input format is :\nput [-h hostname] [-p portname] local_filename remote_filename\nget [-h hostname] [-p portname] remote_filename local_filename\n");
    exit(1);
  }
  if(!strcmp(argv[1],"put"))//输出提示。
   printf("%s hostname:%s,portname:%s,local_filename:%s,remote_filename:%s\n",argv[1],argv[2],argv[3],argv[4],argv[5]);
 else
   printf("%s hostname:%s,portname:%s,local_filename:%s,remote_filename:%s\n",argv[1],argv[2],argv[3],argv[5],argv[4]);


  server_addr.sin_family = PF_INET;//PF_INET（协议族）
  server_addr.sin_addr.s_addr = inet_addr(argv[2]);//主机字节顺序转换成网络字节顺序
  server_addr.sin_port = htons(atoi(argv[3]));//该函数把一个用数字和点表示的IP地址的字符串转换成一个无符号长整型
  bzero(&server_addr.sin_zero,8);  //sin_zero是为了让sockaddr与sockaddr_in两个数据结构保持大小相同而保留的空字节
 
  if ((confd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket fail\n");
    exit(1);
  }


  if (connect(confd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
  {
    perror("connect fail");
    exit(1);
  }

  if (!strcmp(argv[1], "put"))
  {
   client_put(confd,argv[4],argv[5]);
  }
  else if (!strcmp(argv[1], "get"))
  {
   client_get(confd,argv[5],argv[4]);
  }
  else
  {
    printf("input format error\n");
  }

  close(confd);//关闭连接

  return 0;

}
void client_put(int confd,char *local_filename, char *remote_filename)//服务器执行客户端的put命令
{
  static int i=0;
  int len = strlen(remote_filename);
//printf("remote_filename len:%d\n",len);
  printf("client_put\n");
  int nbyte, fd;
  strcpy(buf, "P");//sent 模式
  send(confd, buf, 1, 0);

  send(confd, remote_filename, len, 0);//发送remote_filename
//       printf("file:%s\n",local_filename);

  if ((fd = open(local_filename, O_RDONLY)) == -1)
  {
    perror("open file fail");
    strcpy(buf, "N");
    send(confd, buf, 1, 0);
    return;
  }
  strcpy(buf, "Y");//发送打开成功标志
  send(confd, buf, 1, 0);
  while ((nbyte = read(fd, buf, BUFF)) > 0)
  {
    send(confd, buf, nbyte, 0);
//  printf("i:%d\n",i++);//调试使用
  }
  close(fd);
  printf("client have uploaded the file %s to server secceed\n",local_filename);
 return;
}
void client_get(int confd,char *local_filename, char *remote_filename)//服务器执行客户端的put命令
{
  printf("client_get\n");
//  printf("remote_filename:%s\n",remote_filename);
  int nbyte, fd;
  strcpy(buf, "G");//get 模式
  send(confd, buf, 1, 0);

  send(confd, remote_filename, strlen(remote_filename), 0);
  recv(confd, buf, 1, 0);
  if (buf[0] == 'N')
  {
    printf("server not have the file\n");
    return;
  }
  if ((fd = open(local_filename, O_WRONLY|O_CREAT|O_EXCL, 0666)) == -1)
  {
    perror("open file fail");
    return;
  }


  nbyte = recv(confd, buf, BUFF, 0);
  while(nbyte > 0)//读取客户端发送的数据
  {
    write(fd, buf, nbyte);//存储在本地文件
    nbyte = recv(confd, buf, BUFF, 0);
  }

  close(fd);
  printf("client have downloaded the file %s from server secceed\n",remote_filename);
  return;

}







