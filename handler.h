#include<stdio.h>

// TCP client handling function 
typedef void socket_message_handler(int socket, char *echoBuffer, size_t recvMsgSize);
void Handle(int socket, socket_message_handler *handle);

void EchoMessage(int socket, char *messageBuffer, size_t messageSize);
void HTTPHelloWorld(int socket, char *messageBuffer, size_t messageSize);
void HandleWebRequest(int socket, char *messageBuffer, size_t messageSize);
void HandleOnce(int socket, socket_message_handler *handle);
