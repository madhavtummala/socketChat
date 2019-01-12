#include <stdio.h>  			//printf, scanf
#include <unistd.h>				//for read()			
#include <sys/socket.h>			//for socket functions
#include <stdlib.h>       		//EXIT_FAILURE, memset
#include <string.h>				//strlen()
#include <netinet/in.h>			//for struct sockaddr
#include <pthread.h>

/*

int connect(int sockfd, const struct sockaddr *addr,
				socklen_t addrlen);

*/

#define PORT 8080
#define BUFFSIZE 1024

void *read_client(void* client_socket)
{
	int client = *(int*)client_socket;
	char buffer[BUFFSIZE];

	while(1)
	{
		memset(&buffer, 0, sizeof(buffer));
		read(client, buffer, sizeof(buffer));
		printf("%s\n",buffer);
	}

	return NULL;
}

void *write_client(void* client_socket)
{
	int client = *(int*)client_socket;
	char buffer[BUFFSIZE];

	while(1)
	{
		memset(&buffer, 0, sizeof(buffer));
		fgets(buffer,sizeof(buffer),stdin);
		buffer[strlen(buffer)-1]='\0';
		write(client, buffer, sizeof(buffer));
	}

	return NULL;
}

int main()
{
	int client_socket,ret;
	struct sockaddr_in server_addr;
	int addrlen = sizeof(server_addr);

	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(client_socket == 0)
	{
		perror("socker failed\n");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_family = AF_INET;

	ret = connect(client_socket, 
					(struct sockaddr *)&server_addr, 
						sizeof(server_addr));
	if(ret<0)
	{
		perror("Connection error");
		exit(EXIT_FAILURE);
	}

	pthread_t read, write;

	pthread_create(&read, NULL, read_client, (void*)&client_socket);
	pthread_create(&write, NULL, write_client, (void*)&client_socket);

	pthread_join(read, NULL);
	pthread_join(write, NULL);

	return 0;
}

/*

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
*/