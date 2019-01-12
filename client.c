#include <stdio.h>  			//printf, scanf
#include <unistd.h>				//for read()			
#include <sys/socket.h>			//for socket functions
#include <stdlib.h>       		//EXIT_FAILURE, memset
#include <string.h>				//strlen()
#include <netinet/in.h>			//for struct sockaddr
#include <pthread.h>			//for multi threading
#include <stdbool.h>			//for boolean var
/*

int connect(int sockfd, const struct sockaddr *addr,
				socklen_t addrlen);

*/

#define PORT 8080
#define BUFFSIZE 1024

bool server;

void *read_client(void* client_socket)
{
	int client = *(int*)client_socket;
	char buffer[BUFFSIZE];

	while(server)
	{
		memset(&buffer, 0, sizeof(buffer));
		read(client, buffer, sizeof(buffer));
		if(strcmp(buffer, "q")==0) server=false;
		else printf("Server: %s\n",buffer);
	}

	return NULL;
}

void *write_client(void* client_socket)
{
	int client = *(int*)client_socket;
	char buffer[BUFFSIZE];

	while(server)
	{
		memset(&buffer, 0, sizeof(buffer));
		fgets(buffer,sizeof(buffer),stdin);
		buffer[strlen(buffer)-1]='\0';
		write(client, buffer, sizeof(buffer));
		if(strcmp(buffer, "q")) server=false;
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

	server = true;
	pthread_t read, write;

	pthread_create(&read, NULL, read_client, (void*)&client_socket);
	pthread_create(&write, NULL, write_client, (void*)&client_socket);

	pthread_join(read, NULL);
	pthread_join(write, NULL);

	return 0;
}