#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <sys/select.h>
#include <thread>
#include <vector>

#define MAXLINE 1024


struct Client{
	int client_fd;
	struct sockaddr_in client_addr;
	Client(int fd, struct sockaddr_in addr):
		client_fd(fd)
	{
		memcpy((void*)&client_addr, (void*)&addr, sizeof(struct sockaddr_in));
	}
};
	std::vector<Client> clients;
int main(int argc, char* argv[])
{

	if(argc < 2)
	{
		printf("Usage %s port\n", argv[0]);
		return -1;
	}
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(server_fd < 0) 
	{

		printf("create socket falid\n");
		return -1;
	}

	struct sockaddr_in server_addr;
	bzero((void *)&server_addr, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));
	socklen_t server_len = sizeof(server_addr);
	int ret = bind(server_fd, (struct sockaddr*)&server_addr, server_len);

	if(ret < 0) 
	{

		printf("bind error\n"); 
		return -1;
	}

	if(listen(server_fd, 10) < 0) 
	{

		printf("listen error\n"); 
		return -1;
	}

	printf("start listening!\n");
	
	
	int connect_fd;

	char buff[1024];
	int nbyte;
	fd_set rset;

	char buf[16];
	FD_ZERO(&rset);//清空文件描述符集合
	FD_SET(server_fd, &rset);//将listenfd加入到描述符集合中

	for(;;)
	{
		fd_set tmp_fds = rset;
		if(select(FD_SETSIZE, &tmp_fds, NULL, NULL, NULL) < 0)
		{
			printf("select error");
		}
		if(FD_ISSET(server_fd, &tmp_fds))//若是监听套接字可读，说明有新的连接到来
		{
			struct sockaddr_in client_addr;
			bzero((void *)&client_addr, sizeof(struct sockaddr_in));
			socklen_t client_len = sizeof(struct sockaddr_in);
			printf("new client connect!\n");
			connect_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
			g_counter++;
			FD_SET(connect_fd, &rset);//将新连接加入监听集合中
			clients.push_back(Client(connect_fd, client_addr));//将新的客户端信息加入到客户集合中

		}
		for(auto itr = clients.begin();itr!=clients.end();++itr)//遍历已连接的客户端
		{
			if(FD_ISSET(itr->client_fd, &tmp_fds))//若客户端描述符可读的
			{
				memset(buff, 0, 1024);
				nbyte = recv(itr->client_fd, buff, 1024, 0);
				if(nbyte == 0)//客户端断开连接
				{
					printf("client %s:%d disconnect!\n",(char*)inet_ntop(AF_INET, (void *)&(itr->client_addr.sin_addr), buf, sizeof(buf)),ntohs(itr->client_addr.sin_port));
					FD_CLR(itr->client_fd, &rset);
					clients.erase(itr);
					break;
				}
				else//客户端发送消息
				{
					printf("client %s:%d send msg:%s\n",(char*)inet_ntop(AF_INET, (void *)&(itr->client_addr.sin_addr), buf, sizeof(buf)),ntohs(itr->client_addr.sin_port), buff);
				}
			}
		}

	}
	
	return 0;
}

