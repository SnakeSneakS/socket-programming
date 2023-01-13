#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdbool.h>
#include<netdb.h>
#include<string.h>

#include<pthread.h>

#include<config.h>
#include<socket.h>

/*
#include<go/out/go.h>
#include<http.h>
#include<socket.h>
#include<config.h>
*/

/*proxyサーバの仕様(https):
ref: https://milestone-of-se.nesuke.com/nw-basic/grasp-nw/proxy/

# http
GET: /path
はプロキシサーバには
GET: /http://example.host.com/path
として届く

# https
1. Client->Proxy
2. Proxy->Client: 「HTTP/1.1 200 Connection Established」
3. Client <-> Proxy <-> Server で通信
或いはプロキシサーバ自身で動的に証明書を生成することで通信を除くことができる
*/

void Relay(int clientSocket, int targetSocket){
    char *clientMessageBuffer[RCVBUFSIZE];
    size_t clientMessageSize;

    bool used = false;
    bool closed = false;
    pthread_t timeoutThreadID = socketTimeoutClose(clientSocket,&used,&closed);

    while(1){
        memset(&clientMessageBuffer,0,sizeof(clientMessageBuffer));

        if ((clientMessageSize = recv(clientSocket, clientMessageBuffer, RCVBUFSIZE, 0)) < 0){
            perror("recv() failed");
            goto END;
        }
        //fprintf(stdout, "received from client [%zu]: %s\n", clientMessageSize, clientMessageBuffer);

        if(clientMessageSize==0){
            goto END;
        }
        
        if (send(targetSocket, clientMessageBuffer, clientMessageSize, 0) != clientMessageSize){
            perror("send() sent a different number of bytes than expected");
            goto END;
        }

        used=true;
    }
END: 
    closed=true;
    pthread_join(timeoutThreadID,NULL);
    close(clientSocket); 
    close(targetSocket);
}

struct RelayInThreadArg{
    int clientSocket;
    int targetSocket;
};

void *RelayInThread(void *arg){
    int clientSocket = ((struct RelayInThreadArg *) arg)->clientSocket;
    int targetSocket = ((struct RelayInThreadArg *) arg)->targetSocket;
    free(arg);
    Relay(clientSocket,targetSocket);
    return NULL;
};


//TODO: timeoutで終了
void Connect(int clientSocket, int targetSocket){
    struct RelayInThreadArg *args1 = (struct RelayInThreadArg *) malloc(sizeof(struct RelayInThreadArg));
    struct RelayInThreadArg *args2 = (struct RelayInThreadArg *) malloc(sizeof(struct RelayInThreadArg));
    if(args1==NULL || args2==NULL){
        perror("malloc() failed");
        goto END;
    }
    args1->clientSocket=clientSocket;
    args1->targetSocket=targetSocket;
    args2->clientSocket=targetSocket;
    args2->targetSocket=clientSocket;

    pthread_t threadID1, threadID2;

    pthread_create(&threadID1, NULL, RelayInThread, (void *)args1);
    pthread_create(&threadID2, NULL, RelayInThread, (void *)args2);
    printf("connected %ld, %ld\n",(long int)threadID1,(long int)threadID2);

    if(pthread_join(threadID1, NULL)!=0) perror("pthread_join() failed");
    if(pthread_join(threadID2,NULL)!=0) perror("pthread_join() failed");
    printf("deconnected %ld, %ld\n",(long int)threadID1,(long int)threadID2);

    END: return;
}