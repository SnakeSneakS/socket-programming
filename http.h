typedef enum HTTPMethod{
    //OPTIONS,
    GET,
    //HEAD,
    POST,
    PUT,
    DELETE,
    //TRACE,
    CONNECT, //for proxy server
    PATCH,
    UNKNOWN_METHOD
} HTTPMethod;

struct HTTPRequest{
    char *host;
    int port;
    char *path;
    //HTTPRequestHeader *headers;
    HTTPMethod method;
};

struct HTTPRequest ParseHTTPRequest(char *message);

struct HTTPResponse{
    int HTTPStatus;
    char *ContentType; //text/html
    char *Content;  //
};
void HTTPResponseString(struct HTTPResponse res, char *result, int maxLen);
