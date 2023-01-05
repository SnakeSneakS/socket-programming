#include<stdio.h>
#include<string.h>

typedef enum HTTPMethod{
    OPTIONS,
    HEAD,
    POST,
    PUT,
    DELETE,
    TRACE,
    CONNECT,
    UNKNOWN_METHOD
} HTTPMethod;

HTTPMethod parseHTTPMethod(char *method){
    if(strcmp(method,"Options")==0) return OPTIONS;
    if(strcmp(method,"Head")==0) return HEAD;
    if(strcmp(method,"Post")==0) return POST;
    if(strcmp(method,"Put")==0) return PUT;
    if(strcmp(method,"Delete")==0) return DELETE;
    if(strcmp(method,"Trace")==0) return TRACE;
    if(strcmp(method,"Connect")==0) return CONNECT;
    return UNKNOWN_METHOD;
};

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


struct HTTPRequest{
    char *host;
    char *path;
    HTTPRequestHeader *headers;
    HTTPMethod method;
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
void ParseRequestHTTP(char *message){

}



void CreateResponseHTTP(char *message, int status){

}