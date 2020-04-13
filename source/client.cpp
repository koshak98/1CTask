#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <iostream>

const int BUFF_SIZE = 256;

void error(const char* err) {
    perror(err);
    exit(0);
}

int main(int argc, char* argv[]) {
    int sockfd, port;
    struct sockaddr_in server_addr;
    struct hostent* server;
    if (argc < 3) {
        error("few arguments ERROR");
    }

    port = atoi(argv[2]);   //from str to int
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("open socket ERROR");
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        error("incorrect host ERROR");
    }
    bzero((char*) &server_addr, sizeof(server_addr));   //fill zeros begin from server_addr
    server_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&server_addr.sin_addr.s_addr, server->h_length);    //copy from h_addr to s_addr
    server_addr.sin_port = htons(port);

    if (connect(sockfd,(struct sockaddr *) &server_addr,sizeof(server_addr)) < 0) {
        error("connecting ERROR");
    }

    std::cout << "Try to found out the word\n";
    char buffer[BUFF_SIZE];
    while (true) {
        read(sockfd,buffer,BUFF_SIZE - 1);
        printf("%s\n",buffer);
        bzero(buffer, BUFF_SIZE);

        printf("Please enter the letter: \n");
        fgets(buffer, BUFF_SIZE - 1, stdin);
        if (*buffer == '$') {
            std::cout << "Good bye!\n";
            break;
        }
        write(sockfd, buffer, 1);
        bzero(buffer,BUFF_SIZE);
    }
    close(sockfd);
    return 0;
}