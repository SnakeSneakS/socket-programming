#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdbool.h>
#include<netdb.h>
#include<string.h>

#include<go/out/go.h>
#include<http.h>
#include<socket.h>

#include<config.h>
#include<proxy.h>

typedef void socket_message_handler(int socket, void *echoBuffer, size_t recvMsgSize);

// Echo Message
void EchoMessage(int socket, char *messageBuffer, size_t messageSize){
    printf("received: \n%s",messageBuffer);
    if (send(socket, messageBuffer, messageSize, 0) != messageSize){
        perror("send() failed");
        close(socket);
        return;
    }
    close(socket);
};

void HTTPHelloWorld(int socket){
    char messageBuffer[RCVBUFSIZE];
    size_t messageSize;   //receive message size

    // Receive message from client 
    if ((messageSize = recv(socket, messageBuffer, RCVBUFSIZE, 0)) < 0){
        perror("recv() failed");
        close(socket);
        return;
    }
    
    printf("received: \n%s",messageBuffer);
    char buf[60];
    memset(&buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "Hello World\r\n\r\n");
    if(send(socket, buf, (int)strlen(buf), 0) != (int)strlen(buf)){
        perror("send() failed");
        close(socket);
        return;
    }

    close(socket);
};


size_t createResponseForConnectRequest(
    struct HTTPRequest req,
    char *reqBuffer,
    char *resBuffer){
    size_t res = SendOnce(
        req.host,
        req.port,
        reqBuffer,
        resBuffer
    );
    return res;
}

void handleEchoGet(int socket, struct HTTPRequest req){
    struct HTTPResponse res;
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

    char responseMessage[RCVBUFSIZE];
    memset(&responseMessage, 0, sizeof(responseMessage));
    HTTPResponseString(res, responseMessage, RCVBUFSIZE);
    if(send(socket, responseMessage, (int)strlen(responseMessage), 0) != (int)strlen(responseMessage)){
        perror("send() failed");
        close(socket);
        return;
    }
}

void respondNotFound(int socket, struct HTTPRequest req){
    struct HTTPResponse res;
    res.HTTPStatus=404;
    res.ContentType="text/html";
    char content[1000];
    sprintf(content,"404 Not Found\n");
    res.Content=(content);  

    char responseMessage[RCVBUFSIZE];
    memset(&responseMessage, 0, sizeof(responseMessage));
    HTTPResponseString(res, responseMessage, RCVBUFSIZE);
    if(send(socket, responseMessage, (int)strlen(responseMessage), 0) != (int)strlen(responseMessage)){
        perror("send() failed");
        close(socket);
        return;
    }
}

void handleIndexGet(int socket, struct HTTPRequest req){
     struct HTTPResponse res;
    res.HTTPStatus=404;
    res.ContentType="text/html";
    char content[200];
    snprintf(content,200,"\
        <html>\
            <h1>hello world!</h1>\
            <p></p>\
            <p>path:</p>\
            <ul>\
                <li><a href=\"/echo\">echo</a></li>\
            </ul>\
        </html>\n");
    res.Content=(content);  

    char responseMessage[RCVBUFSIZE];
    memset(&responseMessage, 0, sizeof(responseMessage));
    HTTPResponseString(res, responseMessage, RCVBUFSIZE);
    if(send(socket, responseMessage, (int)strlen(responseMessage), 0) != (int)strlen(responseMessage)){
        perror("send() failed");
        close(socket);
        return;
    }
}

void handleProxyConnect(int clientSocket, struct HTTPRequest req){
    fprintf(stdout, "establish connection toward %s:%d%s\n",req.host,req.port,req.path);
    
    //resolve host name
    struct hostent *hostResolved;
    if ((hostResolved = gethostbyname(req.host))==NULL){
        fprintf(stderr, "gethostbyname failed");
        struct HTTPResponse res;
        res.HTTPStatus = 500;
        res.ContentType="text/html";
        res.Content="host resolve failed.";
        char resultBuffer[RCVBUFSIZE];
        HTTPResponseString(res, resultBuffer, RCVBUFSIZE);
        if(send(clientSocket, resultBuffer, (int)strlen(resultBuffer), 0) != (int)strlen(resultBuffer)){
            perror("send() failed");
            return;
        }
        return;
    }

    unsigned long iPTarget = *((unsigned long *) hostResolved->h_addr_list[0]);
    struct sockaddr_in targetAddr;
    memset(&targetAddr, 0, sizeof(targetAddr));
    targetAddr.sin_family      = AF_INET; 
    targetAddr.sin_addr.s_addr = iPTarget; 
    targetAddr.sin_port        = htons(req.port); 

    int targetSocket;
    if ((targetSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        perror("socket() failed");
        return;
    }
    if (connect(targetSocket, (struct sockaddr *) &targetAddr, sizeof(targetAddr)) < 0){
        perror("connect() failed");
        return;
    }
    
    char *connectOk = "HTTP/1.1 200 Connection Established\r\n\r\n";
    if(send(clientSocket, connectOk, (int)strlen(connectOk), 0) != (int)strlen(connectOk)){
        perror("send() failed");
        close(targetSocket);
        return;
    }
    
    Connect(clientSocket, targetSocket);
    
    END: close(targetSocket);
}

void handleHttpRequest(int socket,struct HTTPRequest req){
    //error check
    if(req.isError){
        char responseMessage[RCVBUFSIZE];
        memset(&responseMessage, 0, sizeof(responseMessage));
        struct HTTPResponse res;
        res.HTTPStatus=500;
        res.ContentType="text/html";
        res.Content="Internal server error";
        HTTPResponseString(res, responseMessage, RCVBUFSIZE);

        if(send(socket, responseMessage, (int)strlen(responseMessage), 0) != (int)strlen(responseMessage)){
            perror("send() failed");
            close(socket);
            return;
        }

        return;
    }

    if(strcmp(req.path,"/")==0){
        handleIndexGet(socket,req);
    }
    // ????????????: /echo/
    else if(strncmp(req.path,"/echo",5)==0 && req.method==GET){
        handleEchoGet(socket,req);
    }
    // Not Found
    else if(req.method==GET){
        respondNotFound(socket, req);  
    }else{
        fprintf(stdout, "unhandling request");
    }

    return;
}

void HandleWebRequest(int socket){
    char messageBuffer[RCVBUFSIZE];
    size_t messageSize;   //receive message size

    // Receive message from client 
    if ((messageSize = recv(socket, messageBuffer, RCVBUFSIZE, 0)) < 0){
        perror("recv() failed");
        close(socket);
        return;
    }

    struct HTTPRequest req = ParseHTTPRequest(messageBuffer);
    handleHttpRequest(socket, req);
    
    close(socket);
    printf("closed\n");
}


void handleHTTPProxy(int socket, char *messageBuffer,struct HTTPRequest req){
    fprintf(stdout, "relay connection toward %s:%d%s\n",req.host,req.port,req.path);
    
    if(strcmp(req.host,"")==0 || strcmp(req.host,"localhost")==0 || strcmp(req.host,"127.0.0.1")==0){
        handleHttpRequest(socket, req);
        return;
    }

    char responseBuffer[RCVBUFSIZE];
    int size = SendOnce(req.host, req.port, messageBuffer, responseBuffer);
    if(send(socket, responseBuffer, (int)strlen(responseBuffer), 0) != (int)strlen(responseBuffer)){
        perror("send() failed");
        return;
    }
};

void HandleWebProxyRequest(int socket){
    char messageBuffer[RCVBUFSIZE];
    size_t messageSize;   //receive message size

    // Receive message from client 
    if ((messageSize = recv(socket, messageBuffer, RCVBUFSIZE, 0)) < 0){
        perror("recv() failed");
        close(socket);
        return;
    }

    struct HTTPRequest req = ParseHTTPRequest(messageBuffer);
    //printf("debug: %s\n",messageBuffer);

    //error check
    if(req.isError){
        char responseMessage[RCVBUFSIZE];
        memset(&responseMessage, 0, sizeof(responseMessage));
        struct HTTPResponse res;
        res.HTTPStatus=500;
        res.ContentType="text/html";
        res.Content="Internal server error";
        HTTPResponseString(res, responseMessage, RCVBUFSIZE);

        if(send(socket, responseMessage, (int)strlen(responseMessage), 0) != (int)strlen(responseMessage)){
            perror("send() failed");
            close(socket);
            return;
        }
        goto END;
    }

    if(req.method==CONNECT){
        handleProxyConnect(socket, req);
        goto END;
    }
    else if(req.method!=UNKNOWN_METHOD){
        //curl google.com --proxy http://user:password@localhost:8080
        handleHTTPProxy(socket, messageBuffer, req);
        goto END;
    }else{
        respondNotFound(socket, req);  
        goto END;
    }
    
    END: close(socket);
    printf("closed\n");
}