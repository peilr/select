#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <thread>
#include <unistd.h>

#define MAXLINE 1024

void ServerContanct(int fd);
void SendData(int fd);
void ReadData(int fd);

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		printf("Usage: %s ip port\n", argv[0]);
		return -1;
	}	

	int client_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(client_fd < 0)
	{
		printf("create socket error\n");
		return -1;
	}


	struct sockaddr_in server_addr;//即将连接的服务器地址信息
	memset((void *)&server_addr, 0 ,sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));//指定端口
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);//指定IP

	int ret = connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));//连接到服务端


	if(ret < 0)
	{
		printf("connect error\n");
		return -1;
	}
	else
	{
		ServerContanct(client_fd);
	}

	return 0;
}

void ServerContanct(int fd)//连接成功处理函数
{
	printf("start send\n");
	auto t1 = std::thread(SendData, fd);//开启一个线程处理发送数据
	auto t2 = std::thread(ReadData, fd);//开启另一个线程处理服务端返回的数据

	t1.join();
	t2.join();
}
void SendData(int fd)
{
	char sendbuf[MAXLINE];
	memset((void *)sendbuf, 0, MAXLINE);
	while(1)
	{

		scanf("%s",sendbuf);
		send(fd, sendbuf, strlen(sendbuf), 0);
		printf("send over\n");
		memset((void *)sendbuf, 0, MAXLINE);
	}


}
void ReadData(int fd)
{
	char readbuf[MAXLINE];
	memset((void*)readbuf, 0, MAXLINE);
	while(1)
	{
		if(read(fd, readbuf, MAXLINE) <= 0)//若服务端断开连接，则跳出循环
			break;
		else
		{
			printf("%s\n", readbuf);
		}
		memset((void *)readbuf, 0, MAXLINE);
	}
	printf("server disconnect\n");
}
