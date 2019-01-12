#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdio.h> //printf scanf
#include<stdlib.h> //memset
#include<unistd.h> 
#include<string.h> //for string functions
#include<netdb.h> //for htons

int main(int argc, char **argv) 
{
  int PORT = atoi(argv[1]);
  int server,client_connected,len;
  struct sockaddr_in client_addr,server_addr;
  char message[1000],received[1000];

  server=socket(AF_INET, SOCK_STREAM, 0);

  memset(&server_addr,0,sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);

  server_addr.sin_addr.s_addr = INADDR_ANY;

  if(bind(server,
  (struct sockaddr*)&server_addr,sizeof(server_addr)) == -1)
  printf("Bind Failure\n");
  else
  printf("Bind Success:<%u>\n", server);

  while(1)
  {   
       listen(server,5);
       len=sizeof(struct sockaddr_in);

        client_connected=accept(server, (struct sockaddr*)&client_addr,(socklen_t *)&len);
        if (-1 != client_connected)
            printf("Connection accepted:<%u>\n", client_connected);

      while(1)
      {
          read(client_connected, received, sizeof(received));

          printf("\nFriend: %s", received);
          printf("\nMain: ");

          fgets(message,sizeof(message),stdin);          
          write(client_connected, message, strlen(message)+1);
 
      }
  }

  close(server); printf("\nServer Socket Closed !!\n");

  return 0;
}