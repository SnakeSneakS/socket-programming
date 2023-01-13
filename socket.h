#include<stdbool.h>
#include<pthread.h>

int SendOnce(char *host, int port, char *sendBuffer, char *resultBuffer);
pthread_t socketTimeoutClose(int socket, bool *used, bool *closed);
