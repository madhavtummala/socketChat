#include <stdio.h>  			//printf, scanf
#include <unistd.h>				//for read()			
#include <sys/socket.h>			//for socket functions
#include <stdlib.h>       		//EXIT_FAILURE, memset
#include <string.h>				//strlen()
#include <netinet/in.h>			//for struct sockaddr

/*

int connect(int sockfd, const struct sockaddr *addr,
				socklen_t addrlen);

*/

#define PORT 8080

int main()
{
	struct sockaddr_in address;
	int sockfd = 0,valread,ret;
	struct sockaddr_in serv_addr;
	char *hello = "Hello from client";
	char buffer[1024] = {0};

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd <0)
	{
		perror("socker creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	ret = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	if(ret<0)
	{
		perror("Connection error");
		exit(EXIT_FAILURE);
	}
	send(sockfd, hello, strlen(hello), 0);
	printf("Hello messsage sent\n");
	valread = read(sockfd, buffer, 1024);
	printf("%s\n",buffer);

	return 0;



}