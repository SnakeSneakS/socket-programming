#include<stdio.h>
#include<sys/socket.h> 
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>

#include<config.h>
#include<http.h>



int SendOnce(char *host, int port, char *sendBuffer, char *resultBuffer)
{
    fprintf(stdout, "connecting to %s:%d\n",host,port);
    //resolve host name
    struct hostent *hostResolved;
    if ((hostResolved = gethostbyname(host))==NULL){
        fprintf(stderr, "gethostbyname failed");
        struct HTTPResponse res;
        res.HTTPStatus = 500;
        res.ContentType="text/html";
        res.Content="host resolve failed.";
        HTTPResponseString(res, resultBuffer, RCVBUFSIZE);
        return strlen(resultBuffer);
    }

    unsigned long iPTarget = *((unsigned long *) hostResolved->h_addr_list[0]);

    struct sockaddr_in targetAddr;
    memset(&targetAddr, 0, sizeof(targetAddr));
    targetAddr.sin_family      = AF_INET; 
    targetAddr.sin_addr.s_addr = iPTarget; 
    targetAddr.sin_port        = htons(port); 

    int sock;
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        perror("socket() failed");
        exit(1);
    }

    if (connect(sock, (struct sockaddr *) &targetAddr, sizeof(targetAddr)) < 0){
        perror("connect() failed");
        close(sock);
        return -1;
    }
    
    //proxy->target
    unsigned int textLen = strlen(sendBuffer);          /* Determine input length */
    if (send(sock, sendBuffer, textLen, 0) != textLen){
        perror("send() sent a different number of bytes than expected");
        close(sock);
        return -1;
    }

    //target->proxy
    int bytesRcvd;
    if ((bytesRcvd = recv(sock, resultBuffer, RCVBUFSIZE - 1, 0)) <= 0){
        perror("recv() failed or connection closed prematurely");
        struct HTTPResponse res;
        res.HTTPStatus = 500;
        res.ContentType="text/html";
        res.Content="recv failed.";
        HTTPResponseString(res, resultBuffer, RCVBUFSIZE);
        return strlen(resultBuffer);
    }

    END: close(sock);
    return strlen(resultBuffer);
}

