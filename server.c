#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<pthread.h>

#include<handler.h>

enum Mode{
    HTTP_SERVER,
    HTTP_PROXY_SERVER,
};

#define MAXPENDING 5



struct ThreadArgs{
    int clientSocket;
    enum Mode serverMode;
};


void *ThreadMain(void *threadArgs){
    pthread_detach(pthread_self()); 
    int clientSocket = ((struct ThreadArgs *) threadArgs)->clientSocket;
    enum Mode serverMode = ((struct ThreadArgs *) threadArgs)->serverMode;
    free(threadArgs);

    switch (serverMode)
    {
    case HTTP_SERVER:
        HandleWebRequest(clientSocket);
        break;
    case HTTP_PROXY_SERVER:
        HandleWebProxyRequest(clientSocket);
        break;
    default:
        perror("unexpected server mode");
        break;
    }

    return NULL;
}



void ListenAndServeTCP(int port, enum Mode serverMode)
{
    int serverSock;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
  
    if ((serverSock = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket() failed");
        return;
    }
     
    // local address structure 
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    int yes = 1;
    setsockopt(serverSock,SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

    // Bind to the local address
    if (bind(serverSock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0){
        perror("bind() failed");
        return;
    }

    // Mark the socket `listen`
    if (listen(serverSock, MAXPENDING) < 0){
        perror("listen() failed");
        return;
    }

    printf("listening on port %d\n", port);
    
    while(true) 
    {
        /* Set the size of the in-out parameter */ 
        unsigned int clientLen = sizeof(clientAddr);

        //wait client connect
        int clientSocket = clientSocket = accept(serverSock, (struct sockaddr *) &clientAddr, &clientLen);
        if (clientSocket < 0){
            perror("accept() failed");
            close(clientSocket);
            return;
        }

        struct ThreadArgs *threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs));
        if(threadArgs==NULL){
            perror("malloc() failed");
            exit(1);
        }
        threadArgs->clientSocket=clientSocket;
        threadArgs->serverMode=serverMode;

        pthread_t threadID;
        if (pthread_create(&threadID, NULL, ThreadMain, (void *) threadArgs) != 0){
            perror("pthread_create() failed");
            exit(1);
        }
        
        printf("Handling client %s, thread: %ld\n", inet_ntoa(clientAddr.sin_addr), (long int) threadID);
        
    }
    close(serverSock);
}

int main(int argc, char *argv[]){
    if (argc != 3){
        fprintf(stderr, "Usage:  %s <Server Port> <ServerMode>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    enum Mode serverMode = atoi(argv[2]);

    switch(serverMode){
        case HTTP_SERVER:
            fprintf(stdout,"running server on port %d in mode %s\n",port,"HTTP SERVER");
            break;
        case HTTP_PROXY_SERVER:
            fprintf(stdout,"running server on port %d in mode %s\n",port,"HTTP PROXY SERVER");
            break;
        default: 
            fprintf(stderr,"unexpected server mode: %d\n",serverMode);
            exit(1);
    }
    

    ListenAndServeTCP(port, serverMode);
}


