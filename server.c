#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<handler.h>

#define MAXPENDING 5

void ListenAndServeTCP(int port, socket_message_handler *handleMessage)
{
    int servSock;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
  
    //socket for incomming connections
    //if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
    if ((servSock = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket() failed");
        return;
    }
     
    // local address structure 
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    //https://www.geekpage.jp/programming/linux-network/http-server.php 
    //これは何だろう
    int yes = 1;
    setsockopt(servSock,SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

    // Bind to the local address
    if (bind(servSock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0){
        perror("bind() failed");
        return;
    }

    // Mark the socket `listen`
    if (listen(servSock, MAXPENDING) < 0){
        perror("listen() failed");
        return;
    }

    printf("listening on port %d\n", port);
    
    while(true) 
    {
        /* Set the size of the in-out parameter */ 
        unsigned int clientLen = sizeof(clientAddr);

        //wait client connect
        int clientSocket;
        if (
            (clientSocket = accept(servSock, (struct sockaddr *) &clientAddr, &clientLen)) < 0
        ){
            perror("accept() failed");
            close(clientSocket);
            return;
        }

        printf("Handling client %s\n", inet_ntoa(clientAddr.sin_addr));
        
        //Handle(clientSocket, handleMessage);
        //Handle(clientSocket, HTTPStatusOK);
        HandleOnce(clientSocket,HandleWebRequest);

    }
    close(servSock);
}

int main(int argc, char *argv[]){
    if (argc != 2){
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    ListenAndServeTCP(port, EchoMessage);
}
