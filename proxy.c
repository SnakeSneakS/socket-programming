#include<stdio.h>
#include<sys/socket.h> 
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>

#include<http.h>
#include<config.h>


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


//relay connection
//TODO: threadingして複数処理可能にする
int Relay(char *host, int port, char *path, char *relayBuffer, char *resultBuffer)
{
    fprintf(stdout, "relay toward %s:%d%s\n",host,port,path);
    //resolve host name
    struct hostent *hostResolved;
    if ((hostResolved = gethostbyname(host))==NULL){
        fprintf(stderr, "gethostbyname failed");
        struct HTTPResponse res;
        res.HTTPStatus = 500;
        res.ContentType="text/html";
        res.Content="host resolve failed.";
        HTTPResponseString(res, resultBuffer, RCVBUFSIZE);
        return sizeof(resultBuffer);
    }

    unsigned long iPTarget = *((unsigned long *) hostResolved->h_addr_list[0]);

    struct sockaddr_in targetAddr;
    memset(&targetAddr, 0, sizeof(targetAddr));
    targetAddr.sin_family      = AF_INET; 
    targetAddr.sin_addr.s_addr = iPTarget; 
    targetAddr.sin_port        = htons(port); 

    int relaySock;
    if ((relaySock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        perror("socket() failed");
        exit(1);
    }

    if (connect(relaySock, (struct sockaddr *) &targetAddr, sizeof(targetAddr)) < 0){
        perror("connect() failed");
        exit(1);
    }
    
    //proxy->target
    unsigned int textLen = strlen(relayBuffer);          /* Determine input length */
    if (send(relaySock, relayBuffer, textLen, 0) != textLen){
        perror("send() sent a different number of bytes than expected");
        exit(1);
    }

    //target->proxy
    int bytesRcvd;
    if ((bytesRcvd = recv(relaySock, resultBuffer, RCVBUFSIZE - 1, 0)) <= 0){
        perror("recv() failed or connection closed prematurely");
        struct HTTPResponse res;
        res.HTTPStatus = 500;
        res.ContentType="text/html";
        res.Content="recv failed.";
        HTTPResponseString(res, resultBuffer, RCVBUFSIZE);
        return sizeof(resultBuffer);
    }

    /*
    while(bytesRcvd>0){
        if((bytesRcvd = recv(relaySock, resultBuffer, RCVBUFSIZE - 1, 0)) <= 0){
            perror("recv() failed or connection closed prematurely");
            struct HTTPResponse res;
            res.HTTPStatus = 500;
            res.ContentType="text/html";
            res.Content="recv failed.";
            HTTPResponseString(res, resultBuffer, RCVBUFSIZE);
            return sizeof(resultBuffer);
        }
        printf("test message size: %d\n", bytesRcvd);
    }
    */

    close(relaySock);
    return bytesRcvd;
}
