#include <stdio.h>  			//printf, scanf
#include <unistd.h>				//for read()			
#include <sys/socket.h>			//for socket functions
#include <stdlib.h>       		//EXIT_FAILURE, memset
#include <string.h>				//strlen()
#include <netinet/in.h>			//for struct sockaddr
#include <pthread.h>			//for multi threading
#include <stdbool.h>			//for boolean var
#include <dirent.h>				//for directories
#include <sys/stat.h>			//for file size
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
bool server,file;

void file_send(int client)
{

	DIR *d;
	struct dirent *dir;
	d = opendir(".");
	char buffer[BUFFSIZE];
	char fname[100];

	if(d)
	{
		char *name;
		while((dir = readdir(d)) != NULL){
			strcat(buffer,dir->d_name);
			strcat(buffer,"\n");
		}
		closedir(d);
	}
	//printf("File Details will be sent! Press ENTER to continue..");
	write(client, buffer, sizeof(buffer));
	
	read(client, fname, sizeof(fname));
	printf("Sending file %s...\n",fname);

	struct stat file_stats;
	stat(fname, &file_stats);
	printf("Size: %u\n", (unsigned int)file_stats.st_size);

    FILE *fp = fopen(fname,"rb");
    if(fp==NULL)
    {
    	printf("Error in file name\n");
        perror("File opern error");
        file=false;
        return; 
    }   

	/* Read data from file and send it */
	int size=file_stats.st_size/BUFFSIZE;
	int i=0,j;
	double percent;
    while(1)
	{
    	/* First read file in chunks of 256 bytes */
		memset(&buffer, 0, sizeof(buffer));
        int nread = fread(buffer,1,BUFFSIZE,fp);

        percent=(i/(double)size)*10;
        printf("[");
        for(j=0;j<percent;j++)
        	printf("#");
        printf("(%.2f)%]\r", percent*10);
        i++;
        fflush(stdout);

    	/* If read was success, send data. */
        if(nread > 0)
        {
            //printf("Sending \n");
            write(client, buffer, nread);
        }
        if(nread < BUFFSIZE)
        {
            if(feof(fp))
	            printf("\nFile transfer completed.\n");

            if(ferror(fp)) printf("Error reading\n");
            	break;
	    }
    }
    file=false;
}

void *read_client(void* client_socket)
{
	int client = *(int*)client_socket;
	char buffer[BUFFSIZE];

	while(server)
	{
		if(!file)
		{
			memset(&buffer, 0, sizeof(buffer));
			read(client, buffer, sizeof(buffer));
			if(strcmp(buffer,"q")==0) break;
			else if(strcmp(buffer,"f")==0) file=true;
			else printf("Client(%d): %s\n",client,buffer);
		}
		else file_send(client);
	}
	printf("client %d left\n", client);
	write(client, buffer, sizeof(buffer));

	return NULL;
}

void *write_client()
{
	//int client = *(int*)client_socket;
	char buffer[BUFFSIZE];

	while(server)
	{
		if(!file)
		{
			memset(&buffer, 0, sizeof(buffer));
			fgets(buffer,sizeof(buffer),stdin);
			buffer[strlen(buffer)-1]='\0';

			if(strcmp(buffer, "q")==0)
				server=false;

			for(int j=0;j<n_conn;j++)
				write(fd[j], buffer, sizeof(buffer));
		}
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
	file=false;

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