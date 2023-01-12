#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdbool.h>
#include<go/out/go.h>
#include<http.h>
#include<proxy.h>

//#include<arpa/inet.h>
//#include<stdlib.h>
#include<string.h>
//#include<unistd.h>

#include<config.h>

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

void HTTPHelloWorld(int socket, char *messageBuffer, size_t messageSize){
    /*XXXXXXXXXXXXXXXXstruct CustomHTTPRequest req = ParseHTTPRequest(messageBuffer);
    printf("Host: %s\n",req.Host);
    printf("PATH: %s\n",req.Path);
    printf("METHOD: %s\n",req.Method);*/
    
    printf("received: \n%s",messageBuffer);
    char buf[60];
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "Hello World\r\n");
    if(send(socket, buf, (int)strlen(buf), 0) != (int)strlen(buf)){
        perror("send() failed");
        close(socket);
        return;
    }
}

struct HTTPResponse createResponseForGetRequest(struct HTTPRequest req){
    struct HTTPResponse res;
    if(strcmp(req.path,"/proxy")==0){
        printf("NotImplemented");
    }
    // 前方一致: /echo/
    else if(strncmp(req.path,"/echo",5)==0){
        res.HTTPStatus=200;
        res.ContentType="text/html";
        char content[1000];
        if(strcmp(req.path,"/echo")==0){
            char *example="HelloWorld\%20INPUT\%20TEXT\%20HERE!!";
            sprintf(content,"for path /echo/$Value, return $Value<br/><br/>example: <a href=\"/echo/%s\">%s/echo/%s<a>", example,req.host,example);
        }else{
            sprintf(content,"%s",req.path);
        }
        
        res.Content=(content);  
        //例えば右のようにscriptを実行できる. http://localhost:8080/%3Cscript%3Ealert(%221%22);%3C/script%3E
    }
    // Not Found
    else{
        res.HTTPStatus=404;
        res.ContentType="text/html";
        char content[1000];
        sprintf(content,"404 Not Found\n");
        res.Content=(content);  
    }
    return res;
}

size_t createResponseForConnectRequest(
    struct HTTPRequest req,
    char *reqBuffer,
    char *resBuffer){
    size_t res = Relay(
        req.host,
        req.port,
        req.path,
        reqBuffer,
        resBuffer
    );
    return res;
}

void HandleWebRequest(int socket, char *messageBuffer, size_t messageSize){
    struct HTTPRequest req = ParseHTTPRequest(messageBuffer);
    
    struct HTTPResponse res;

    char responseMessage[RCVBUFSIZE];
    memset(responseMessage, 0, sizeof(responseMessage));

    //error check
    if(req.host==NULL || req.path==NULL){
        res.HTTPStatus=500;
        res.ContentType="text/html";
        res.Content="Internal server error";
        HTTPResponseString(res, responseMessage, RCVBUFSIZE);
        goto send;
    }

    /*
    switch (req.method)
    {
    case (enum HTTPMethod)GET:
        res = createResponseForGetRequest(req);
        HTTPResponseString(res, responseMessage, RCVBUFSIZE);
        break;
    case (enum HTTPMethod)CONNECT:
        createResponseForConnectRequest(req, messageBuffer, responseMessage);
        break;
    case (enum HTTPMethod)UNKNOWN_METHOD:
        res.HTTPStatus=404;
        res.ContentType="text/html";
        res.Content="Unknown Method";
        HTTPResponseString(res, responseMessage, RCVBUFSIZE);
        break;
    default:
        res.HTTPStatus=500;
        res.ContentType="text/html";
        res.Content="Internal server error";
        HTTPResponseString(res, responseMessage, RCVBUFSIZE);
        break;
    }
    */
    createResponseForConnectRequest(req, messageBuffer, responseMessage);

    send: if(send(socket, responseMessage, (int)strlen(responseMessage), 0) != (int)strlen(responseMessage)){
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
