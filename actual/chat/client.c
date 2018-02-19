#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>  
#include<unistd.h>
#include<string.h>
#include<stdlib.h>

int main(int argc, char **argv)
{
    int PORT = atoi(argv[1]);
    int client;
    struct sockaddr_in server_addr;
    struct hostent *ptrh;

    char message[1000];
    char received[1000];

    client=socket(AF_INET, SOCK_STREAM, 0);

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    ptrh=gethostbyname("10.0.32.226"); //ipadress of server here
    memcpy(&server_addr.sin_addr,ptrh->h_addr,ptrh->h_length);
    // server_addr.sin_addr.s_addr = INADDR_ANY;

    if( -1 == (connect(client, (struct sockaddr*)&server_addr, sizeof(server_addr)))) 
    { 
        printf("\nServer Not Ready !!\n");
        exit(1); 
    }

    while(1)
    {
        printf("\nFriend: ");

        fgets(message,sizeof(message),stdin);

        write(client, message, strlen(message)+1);

        read(client, received, sizeof(received));

        printf("\nMain: %s", received);
    }

    return 0;
}