#include<stdio.h>
#include<sys/socket.h> 
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>

#include<config.h>
#include<http.h>
#include<socket.h>

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serverAddr;

    if (argc != 4)
    {
       fprintf(stderr, "Usage: %s <Server Host> <Server Port> <Word>\n", argv[0]);
       exit(1);
    }

    char *host = argv[1];
    int port = atoi(argv[2]);
    char *text = argv[3];

    char resultBuffer[RCVBUFSIZE];
    memset(resultBuffer,0,sizeof(resultBuffer));

    int resSize = SendOnce(host, port, text, resultBuffer);

    //resultBuffer[resSize]='\0';
    printf("%s",resultBuffer);
    exit(0);
}

