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
void server_put(int confd);
void server_get(int confd);
int main(int argv, char *argc[])
{
	int listenfd,confd = 0,addr_len,read_byte;
	struct sockaddr_in server_addr, client_addr;
	if(argv < 3)
	{
		printf("input format is :[-h hostname] [-p portname]\n ");
		exit(1);
	}
	printf("hostname:%s,portname:%s\n",argc[1],argc[2]);

	if((listenfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket fail");
		exit(1);
	}
	server_addr.sin_family = PF_INET;//PF_INET（协议族）
	server_addr.sin_addr.s_addr = inet_addr(argc[1]);//主机字节顺序转换成网络字节顺序
	server_addr.sin_port = htons(atoi(argc[2]));//该函数把一个用数字和点表示的IP地址的字符串转换成一个无符号长整型
	bzero(&server_addr.sin_zero,8);	//sin_zero是为了让sockaddr与sockaddr_in两个数据结构保持大小相同而保留的空字节
	if(bind(listenfd,(struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("bing error");
		exit(1);
	}
	if(listen(listenfd, 5) == -1)//等待指定的端口的出现客户端连接
	{
		perror("listen fail");
		exit(1);
	}

	printf("listening\n");
	while(1)
	{
		addr_len = sizeof(client_addr);
		if((confd = accept(listenfd, (struct sockaddr *)&client_addr, &addr_len)) == -1)// 用于接受客户端的服务请求
		{
			perror("accept fail");
			exit(1);
		}
		if((read_byte = recv(confd, buf, 10, 0)) == -1)//接收数据
		{
			perror("read fail\n");
			exit(1);
		}	
		switch(buf[0])//读取buf[0]中的命令，P=put，G=get
		{
			case 'P':
				server_put(confd);
				break;
			case 'G':
				server_get(confd);
				break;
			default:
				break;
		}	
		close(confd);

	}
	return;

}
void server_put(int confd)//服务器执行客户端的put命令
{
	printf("server_put\n\n");
	int nbyte,fd;
	char local_filename[20];
	nbyte = recv(confd, local_filename, 20, 0);
	local_filename[nbyte]= '\0';//贴别注意！！！！此处要加‘\0’，否则会出乱码
//	printf("local_filename:%s\n",local_filename);
	recv(confd,buf,1,0);
	if(buf[0]=='N')
	{
		printf("client not have the file\n");
		return;
	}


	if((fd = open(local_filename, O_WRONLY|O_CREAT|O_EXCL,0666)) == -1)//只写的方式打开文件，如果没有则创建,如果存在，则报错
	{
		perror("open file fali\n");
		return;		
	}

	while((nbyte = recv(confd, buf, BUFF, 0)) > 0)//读取客户端发送的数据
	{
		write(fd, buf, nbyte);//存储在本地文件
	}
	close(fd);//
	printf("uploaded\n");
	return;
}
void server_get(int confd)//服务器执行客户端的get命令
{
	printf("server_get\n\n");
	int nbyte,fd;
	char local_filename[20];
	nbyte = recv(confd,local_filename,20,0);//用来接收远端主机经指定的socket传来的数据，并把数据存到由参数buf 指向的内存空间，参数len为可接收数据的最大长度
	local_filename[nbyte]='\0'; 
	local_filename[nbyte+1]='\0'; 
	printf("client need download the file %s\n",local_filename);
	if((fd = open(local_filename,O_RDONLY)) == -1)//打开文件
	{
		perror("open file fail\n");
		strcpy(buf,"N");//发送打开失败回应
		send(confd, buf, 1, 0);//发送数据
		return;
	}
	strcpy(buf,"Y");//发送打开成功回应
	send(confd, buf, 1, 0);
	while((nbyte = read(fd,buf,BUFF))>0)//读取文件的数据，nbyte为实际读取的数据
	{
		send(confd, buf, nbyte, 0);//发送数据
	}
	close(fd);//关闭文件

	printf("downloaded\n");//下载完成提示
	return;
}







