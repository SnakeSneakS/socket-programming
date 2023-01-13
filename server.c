#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<pthread.h>
#include<signal.h>

#include<handler.h>

enum Mode{
    HTTP_SERVER,
    HTTP_PROXY_SERVER,
};

#define MAXPENDING 5

//threading
struct ThreadArgs{
    pthread_mutex_t *mutex;
    int clientSocket;
    enum Mode serverMode;
    unsigned long *counter;
};
void *ThreadMain(void *threadArgs){
    pthread_detach(pthread_self()); 
    pthread_mutex_t *mutex = ((struct ThreadArgs *) threadArgs)->mutex;
    int clientSocket = ((struct ThreadArgs *) threadArgs)->clientSocket;
    enum Mode serverMode = ((struct ThreadArgs *) threadArgs)->serverMode;
    unsigned long *counter = ((struct ThreadArgs *) threadArgs)->counter;
    free(threadArgs);

    pthread_mutex_lock(mutex);
    *counter += 1;
    pthread_mutex_unlock(mutex);

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

    pthread_mutex_lock(mutex);
    *counter -= 1;
    pthread_mutex_unlock(mutex);

    return NULL;
}

//for gracefully shutting down server
bool _stop = false;
void setStopServerFlag(int ignored){
    printf("setStopServerFlag on\n");
    _stop = true;
}

//TODO: graceful shutting down
void ListenAndServeTCP(int port, enum Mode serverMode)
{
    //for graceful shut down
    //Ctrl+C to shutting down  
    struct sigaction stopActionHandler; 
    stopActionHandler.sa_handler=setStopServerFlag;
    if (sigfillset(&stopActionHandler.sa_mask) < 0){
        perror("sigfillset() failed");
        return;
    }
    sigemptyset(&stopActionHandler.sa_mask);
    stopActionHandler.sa_mask=0; //block all signals while 
    stopActionHandler.sa_flags = SA_RESETHAND;
    if (sigaction(SIGINT, &stopActionHandler, NULL)==-1){
        perror("sigaction() failed");
        return;
    }


    unsigned long workerCounter = 0; //active workers counter
    pthread_mutex_t mutex;
    if(pthread_mutex_init(&mutex, NULL)!=0){
        perror("pthread_mutex_init() failed");
        return;
    }

    int serverSock;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;    
  
    if ((serverSock = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket() failed");
        goto FREE_SERVER;
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
        goto FREE_SERVER;
    }

    // Mark the socket `listen`
    if (listen(serverSock, MAXPENDING) < 0){
        perror("listen() failed");
        goto FREE_SERVER;
    }

    printf("listening on port %d\n", port);
    
    while(!_stop) 
    {
        //wait client connect
        unsigned int clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSock, (struct sockaddr *) &clientAddr, &clientLen);
        if (clientSocket < 0){
            perror("accept() failed");
            goto FREE_CLIENT;
        }

        struct ThreadArgs *threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs));
        if(threadArgs==NULL){
            perror("malloc() failed");
            goto FREE_CLIENT;
        }
        threadArgs->mutex=&mutex;
        threadArgs->clientSocket=clientSocket;
        threadArgs->serverMode=serverMode;
        threadArgs->counter=&workerCounter;

        pthread_t threadID;
        if (pthread_create(&threadID, NULL, ThreadMain, (void *) threadArgs) != 0){
            perror("pthread_create() failed");
            goto FREE_CLIENT;
        }
        
        printf("worker: %lu\n", workerCounter);
        printf("Handling client %s, thread: %ld\n", inet_ntoa(clientAddr.sin_addr), (long int) threadID);

        continue;

    FREE_CLIENT:
        printf("free client %s\n", inet_ntoa(clientAddr.sin_addr));
        close(clientSocket);
        continue; //break;
    }

FREE_SERVER:
    printf("----------\n");
    printf("Stopping server...\n");
    printf("Input Ctrl+C to force stop\n");
    printf("----------\n");
    printf("waiting workers... %lu\n", workerCounter);
    while(workerCounter>0){
        sleep(1);
        printf("waiting workers... %lu\n", workerCounter);
    }
    if(pthread_mutex_destroy(&mutex)!=0) perror("pthread_mutex_destroy() failed");
    if(close(serverSock)!=0) perror("close(serverSock) failed");
    printf("server stoped!\n");
    return;
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


