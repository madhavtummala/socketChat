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

#define clear() printf("\033[H\033[J")
#define PORT 8080
#define BUFFSIZE 1024

bool server;
bool file;

void file_recieve(int client)
{
	char fname[100];
	char recvBuff[BUFFSIZE];

	// read(client, recvBuff, sizeof(recvBuff));
	// printf("%s",recvBuff);

	printf("File to Download: ");
	fflush(stdout);
	fgets(fname,sizeof(fname),stdin);
	fname[strlen(fname)-1]='\0';
	write(client, fname, sizeof(fname));

	FILE *fp;

	printf("Receiving file...\n");
   	fp = fopen(fname, "ab"); 
    if(NULL == fp)
    {
    	printf("Error in file name\n");
        perror("Error opening file");
        file=false;
        return;
    }
    
    ssize_t sz=0,br;
    /* Receive data in chunks of 256 bytes */
    while(1)
    { 
    	br = read(client, recvBuff, BUFFSIZE);
    	
        sz++;
        //clear();
        printf("Received: %ld Mb",(sz/BUFFSIZE));
        sleep(1);
        printf("\r");
        sleep(1);
        fflush(stdout);
	    sleep(1);
        
        if(fwrite(recvBuff, 1, br, fp)!=br)
        {
        	perror("Write file error");
        	file=false;
        	return;
        }

        if(br<BUFFSIZE) break;

        memset(&recvBuff, 0, sizeof(recvBuff));
        // printf("%s \n", recvBuff);
    }

    if(br < 0) printf("\n Read Error \n");
    fclose(fp);
    printf("\nFile OK....Completed\n");
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
			if(strcmp(buffer, "q")==0) server=false;
			else printf("Server: %s\n",buffer);
		}
		else file_recieve(client);
	}

	return NULL;
}

void *write_client(void* client_socket)
{
	int client = *(int*)client_socket;
	char buffer[BUFFSIZE];

	while(server)
	{
		if(!file)
		{
			memset(&buffer, 0, sizeof(buffer));
			fgets(buffer,sizeof(buffer),stdin);
			buffer[strlen(buffer)-1]='\0';
			write(client, buffer, sizeof(buffer));
			if(strcmp(buffer, "q")==0) server=false;
			if(strcmp(buffer, "f")==0) file=true;
		}
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
	file = false;
	pthread_t read, write;

	pthread_create(&read, NULL, read_client, (void*)&client_socket);
	pthread_create(&write, NULL, write_client, (void*)&client_socket);

	pthread_join(read, NULL);
	pthread_join(write, NULL);

	return 0;
}