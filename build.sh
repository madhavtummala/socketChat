gcc client.c -o client -lpthread
gcc server.c -o server -lpthread
PORT="8080"
PATH="../"
mv client $PATH/client