#include <stdio.h>  			//printf, scanf
#include <unistd.h>				//for read()			
#include <sys/socket.h>			//for socket functions
#include <stdlib.h>       		//EXIT_FAILURE, memset
#include <string.h>				//strlen()
#include <netinet/in.h>			//for struct sockaddr
#include <pthread.h>			//for multi threading
#include <stdbool.h>			//for boolean var
/*

int sockfd = socket(AF_INET, SOCK_STREAM, 0);

int setsockopt(int sockfd, int level, int optname, 
				const void *optval, socklen_t optlen);

int bind(int sockfd, const struct sockaddr *addr, 
				socklen_t addrlen);

int listen(int sockfd, int NUMCONN);

int new_socket = accept(int sockfd, struct sockaddr *addr,
							socklen_t *addrlen);

*/

#define NUMCONN 5
#define PORT 8080
#define BUFFSIZE 1024
#define MAX_CLIENTS 5

int n_conn;
int fd[MAX_CLIENTS];
bool server;

void *read_client(void* client_socket)
{
	int client = *(int*)client_socket;
	char buffer[BUFFSIZE];

	while(server)
	{
		memset(&buffer, 0, sizeof(buffer));
		read(client, buffer, sizeof(buffer));
		if(strcmp(buffer,"q")==0) break;
		else printf("Client(%d): %s\n",client,buffer);
	}
	printf("client %d left\n", client);

	return NULL;
}

void *write_client()
{
	//int client = *(int*)client_socket;
	char buffer[BUFFSIZE];

	while(server)
	{
		memset(&buffer, 0, sizeof(buffer));
		fgets(buffer,sizeof(buffer),stdin);
		buffer[strlen(buffer)-1]='\0';

		if(strcmp(buffer, "q")==0)
			server=false;

		for(int j=0;j<n_conn;j++)
			write(fd[j], buffer, sizeof(buffer));
	}

	return NULL;
}

int main()
{
	int server_socket,client_socket,ret;
	struct sockaddr_in server_addr;
	int addrlen = sizeof(server_addr);
	pthread_t clients[MAX_CLIENTS];

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket == 0)
	{
		perror("socker failed\n");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_family = AF_INET;

	ret = bind(server_socket, (struct sockaddr *)&server_addr, 
								sizeof(server_addr));
	if(ret<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	ret = listen(server_socket, NUMCONN);
	if(ret<0)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}

	pthread_t write;
	pthread_create(&write, NULL, write_client, NULL);
	server=true;

	for(n_conn=0;n_conn<MAX_CLIENTS;n_conn++)
	{
		client_socket = accept(server_socket, 
							(struct sockaddr *)&server_addr,
									(socklen_t*)&addrlen);

		pthread_create(&clients[n_conn], NULL, 
									read_client, 
									(void*)&client_socket);

		fd[n_conn] = client_socket;
	}

	for(int j=0;j<MAX_CLIENTS;j++)
		pthread_join(clients[j], NULL);

	pthread_join(write, NULL);


	return 0;
}