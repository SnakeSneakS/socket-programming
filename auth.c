#include<stdio.h>
#include<stdbool.h>
#include<string.h>

bool checkBasicAuth(char *username1, char *username2, char *password1, char *password2){
    bool isOk = 
        (strcmp(username1,username2)==0) 
        && (strcmp(password1,password2)==0);
    return isOk;
}

//for test
int main(){
    printf("%d\n",checkBasicAuth("user","user","pw","pw"));
    printf("%d\n",checkBasicAuth("user","user2","pw","pw"));
    printf("%d\n",checkBasicAuth("user","user","pw","pw2"));
}