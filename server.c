#include <stdio.h>  			//printf, scanf
#include <unistd.h>				//for read()			
#include <sys/socket.h>			//for socket functions
#include <stdlib.h>       		//EXIT_FAILURE, memset
#include <string.h>				//strlen()
#include <netinet/in.h>			//for struct sockaddr
/*

int sockfd = socket(AF_INET, SOCK_STREAM, 0);

int setsockopt(int sockfd, int level, int optname, 
				const void *optval, socklen_t optlen);

int bind(int sockfd, const struct sockaddr *addr, 
				socklen_t addrlen);

int listen(int sockfd, int backlog);

int new_socket = accept(int sockfd, struct sockaddr *addr,
							socklen_t *addrlen);

*/

#define BACKLOG 5
#define PORT 8080

int main()
{
	int server_fd, new_socket,new_socket2, valread;
	int ret;
	struct sockaddr_in address;
	int opt = 0;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};
	char *hello = "Hello from server";

	//creating a socker file descriptor
	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(server_fd == 0)
	{
		perror("socker failed\n");
		exit(EXIT_FAILURE);
	}

	//Forcefully attaching socker to the port 8080
	//ret = setsockopt(server_fd, SOL_SOCKET, 
							//SO_REUSEADDR | SO_REUSEPORT,
							//	&opt, sizeof(opt));
	if(ret)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	//forcefully binding the socket
	ret = bind(server_fd, (struct sockaddr *)&address,
						sizeof(address));

	if(ret<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	printf("Before listen\n");
	ret = listen(server_fd, BACKLOG);
	printf("After listen\n");
	if(ret<0)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}

	new_socket = accept(server_fd, (struct sockaddr *)&address,
										(socklen_t*)&addrlen);

	new_socket2 = accept(server_fd, (struct sockaddr *)&address,
										(socklen_t*)&addrlen);

	if(new_socket<0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}

	if(new_socket2<0)
	{
		perror("accept2");
		exit(EXIT_FAILURE);
	}
	valread = read(new_socket, buffer, 1024);
	printf("%s\n",buffer);
	valread = read(new_socket2, buffer, 1024);
	printf("%s\n",buffer);
	send(new_socket, hello , strlen(hello), 0);
	send(new_socket, hello , strlen(hello), 0);
	printf("Hello messages sent\n");
	
	return 0;

}