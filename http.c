#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<go/out/go.h>

/*
//全部ではない
typedef enum HTTPRequestHeader{
    ACCEPT,
    FROM,
    HOST,
    UNKNOWN_HEADER,
} HTTPRequestHeader;
HTTPRequestHeader parseHTTPRequestHeader(char *header){
    char *type = strtok(header, ": ");
    printf("%s\n", type);

    if(strcmp(type,"Accept")==0) return ACCEPT;
    if(strcmp(type,"From")==0) return FROM;
    if(strcmp(type,"HOST")==0) return HOST;
    return UNKNOWN_HEADER;
};
*/

typedef enum HTTPMethod{
    //OPTIONS,
    GET,
    //HEAD,
    POST,
    PUT,
    DELETE,
    //TRACE,
    CONNECT,
    PATCH,
    UNKNOWN_METHOD
} HTTPMethod;

HTTPMethod parseHTTPMethod(char *method){
    if(strcasecmp(method,"GET")==0) return GET;
    //if(strcmp(method,"OPTIONS")==0) return OPTIONS;
    //if(strcmp(method,"HEAD")==0) return HEAD;
    if(strcmp(method,"POST")==0) return POST;
    if(strcmp(method,"PUT")==0) return PUT;
    if(strcmp(method,"DELETE")==0) return DELETE;
    //if(strcmp(method,"TRACE")==0) return TRACE;
    if(strcmp(method,"PATCH")==0) return PATCH;
    if(strcmp(method,"CONNECT")==0) return CONNECT;
    return UNKNOWN_METHOD;
};

struct HTTPRequest{
    char *host;
    int port;
    char *path;
    //HTTPRequestHeader *headers;
    HTTPMethod method;
    bool isError;
};

/*
parse http message:

e.g. 
    """
    GET / HTTP/1.1
    Host: localhost:8080
    User-Agent: curl/7.71.1
    Accept: *／*
    """
    ->
    HOST: localhost
    PATH: /
    METHOD: GET
*/
struct HTTPRequest ParseHTTPRequest(char *message) {
    struct CustomHTTPRequest r = CGOParseHTTPRequest(message);
    if(r.Host==NULL || r.Path==NULL | r.Method==NULL){
        fprintf(stderr,"failed to parse http request. %s",message);
        struct HTTPRequest req;
        req.isError=true;
        return req;
    }
    /*
    printf("Host: %s\n",r.Host);
    printf("PATH: %s\n",r.Path);
    printf("PORT: %d\n",r.Port);
    printf("METHOD: %s\n",r.Method);
    */

    struct HTTPRequest req;
    req.host=r.Host;
    req.port=r.Port;
    req.path=r.Path;
    req.isError=false;
    req.method=parseHTTPMethod(r.Method);
    return req;
}


struct HTTPResponse{
    int HTTPStatus;
    char *ContentType; //text/html
    char *Content;  //
};
void HTTPResponseString(struct HTTPResponse res, char *result, int maxLen){
    snprintf(result,
        maxLen,
        "HTTP/1.1 %d\r\n"\
        "Content-Type: %s\r\n"\
        "\r\n"\
        "%s\r\n",
        res.HTTPStatus,
        res.ContentType,
        res.Content);
};



