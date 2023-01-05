#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdbool.h>
#include<go/out/go.h>

//#include<arpa/inet.h>
//#include<stdlib.h>
#include<string.h>
//#include<unistd.h>

#define RCVBUFSIZE (1<<20)

typedef void socket_message_handler(int socket, void *echoBuffer, size_t recvMsgSize);

void Handle(int socket, socket_message_handler *handle)
{
    char echoBuffer[RCVBUFSIZE];
    int recvMsgSize;   //receive message size

    // Receive message from client */
    if ((recvMsgSize = recv(socket, echoBuffer, RCVBUFSIZE, 0)) < 0){
        perror("recv() failed");
        close(socket);
        return;
    }

    /* Send received string and receive again until end of transmission */
    while (recvMsgSize > 0)      /* zero indicates end of transmission */
    {
        //printf("received: %s\n",echoBuffer);
        
        //handle message
        (*handle)(socket,echoBuffer,recvMsgSize);

        // See if there is more data to receive 
        if ((recvMsgSize = recv(socket, echoBuffer, RCVBUFSIZE, 0)) < 0){
            perror("recv() failed");
            close(socket);
            return;
        }
    }

    
    close(socket);
    printf("CLOSED\n");
}

void HandleOnce(int socket, socket_message_handler *handle)
{
    char buf[RCVBUFSIZE];
    int size;   //receive message size

    // Receive message from client */
    if ((size = recv(socket, buf, RCVBUFSIZE, 0)) < 0){
        perror("recv() failed");
        close(socket);
        return;
    }
    (*handle)(socket,buf,size);
    close(socket);
    printf("CLOSED\n");
}

// Echo Message
void EchoMessage(int socket, char *messageBuffer, size_t messageSize){
    printf("received: \n%s",messageBuffer);
    if (send(socket, messageBuffer, messageSize, 0) != messageSize){
        perror("send() failed");
        close(socket);
        return;
    }
}

void HTTPStatusOK(int socket, char *messageBuffer, size_t messageSize){
    printf("received: \n%s",messageBuffer);
    char buf[2048];
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf),
        "HTTP/1.1 200 OK\r\n"
        //"Content-Length: 20\r\n"
        "Content-Type: text/html\r\n"
        //"Connection: close\r\n"
        "\r\n"
        "HELLO\r\n");
    if(send(socket, buf, (int)strlen(buf), 0) != (int)strlen(buf)){
        perror("send() failed");
        close(socket);
        return;
    }
}


void HandleWebRequest(int socket, char *messageBuffer, size_t messageSize){
    struct CustomHTTPRequest req = ParseHTTPRequest(messageBuffer);
    printf("Host: %s\n",req.Host);
    printf("PATH: %s\n",req.Path);
    printf("METHOD: %s\n",req.Method);
    
    char buf[2048];
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf),
        "HTTP/1.1 200 OK\r\n"
        //"Content-Length: 20\r\n"
        "Content-Type: text/html\r\n"
        //"Connection: close\r\n"
        "\r\n"
        "HELLO\r\n");
    if(send(socket, buf, (int)strlen(buf), 0) != (int)strlen(buf)){
        perror("send() failed");
        close(socket);
        return;
    }
}


        

//receive just `len` message
bool ReceiveFull(int socket, uint8_t* buf, size_t len)
{
    size_t received = 0;
    while (received < len)
    {
        ssize_t receivedNow = recv(socket, &buf[received], len - received, 0);
        if (receivedNow == 0 || receivedNow == -1){
            return false;
        }
        received += receivedNow;
    }
    return true;
}
