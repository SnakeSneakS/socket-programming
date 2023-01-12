#include<stdio.h>
#include<sys/socket.h> 
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include<config.h>

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serverAddr;

    if (argc != 4)
    {
       fprintf(stderr, "Usage: %s <Server IP> <Server Port> <Word>\n", argv[0]);
       exit(1);
    }

    char *serverIP = argv[1];
    unsigned short serverPort = atoi(argv[2]);
    char *text = argv[3];

    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        perror("socket() failed");
        exit(1);
    }

    /* Construct the server address structure */
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family      = AF_INET; 
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);
    serverAddr.sin_port        = htons(serverPort); 

    printf("connecting to %s:%d\n",serverIP,serverPort);
    if (connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0){
        perror("connect() failed");
        exit(1);
    }
        

    //マルチクライアントの必要性を理解
    sleep(5);
    
    unsigned int textLen = strlen(text);          /* Determine input length */
    if (send(sock, text, textLen, 0) != textLen){
        perror("send() sent a different number of bytes than expected");
        exit(1);
    }

    char buffer[RCVBUFSIZE];
    int totalBytesRcvd = 0;
    int bytesRcvd;

    while (totalBytesRcvd < textLen)
    {
        /* Receive up to the buffer size (minus 1 to leave space for
           a null terminator) bytes from the sender */
        if ((bytesRcvd = recv(sock, buffer, RCVBUFSIZE - 1, 0)) <= 0){
            perror("recv() failed or connection closed prematurely");
            exit(1);
        }
        totalBytesRcvd += bytesRcvd;
        buffer[bytesRcvd] = '\0';
        printf("Received: %s\n", buffer);
    }

    close(sock);
    exit(0);
}
