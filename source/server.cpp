#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

const int BUFF_SIZE = 256;
void error(const char* err)
{
    perror(err);
    exit(1);
}

int client_con (int sockfd, struct sockaddr_in client_addr) {
    socklen_t clilen = sizeof(client_addr);
    int newsockfd = accept(sockfd, (struct sockaddr*) &client_addr, &clilen);
    if (newsockfd < 0) {
          error("accept ERROR");
    }
    return newsockfd;
}

void change(const std::string& word, std::string& cur_word, char cur_char) {
    for (int i = 0; i < word.size(); ++i) {
        if (word[i] == cur_char) {
            cur_word[i] = cur_char;
        }
    }
}

void game(int newsockfd, const std::string& word, int attemps) {
    char cur_char[BUFF_SIZE];
    std::string cur_word(word.size(), '*');
    write(newsockfd, cur_word.data(), cur_word.size());

    for (int i = 0; i < attemps; ++i) {
        bzero(cur_char,1);
        read(newsockfd, cur_char, 1);
        change(word, cur_word, *cur_char);
        write(newsockfd, cur_word.data(), cur_word.size());
        std::cout << cur_word << "\n";
        if (word == cur_word) {
            write(newsockfd, "You have alreary won!\nEnter '$' to exit", 39);
            close(newsockfd);
            return;
        }
    }
    write(newsockfd, "You lose\n", 9);
    close(newsockfd);
}

int main(int argc, char *argv[]) {
    std::string word;
    std::cout << "You are a host. Write a word: ";
    std::cin >> word;
    int attemps;
    std::cout << "How many attemps: ";
    std::cin >> attemps;
    int sockfd, newsockfd, port;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, client_addr;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("opening socket ERROR");
    }
    bzero((char*) &serv_addr, sizeof(serv_addr));
    port = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) { 
         error("binding ERROR");
    }

    listen(sockfd, 5);

    newsockfd = client_con(sockfd, client_addr);    //connect with client

    game(newsockfd, word, attemps);     //start game

    close(sockfd);
    return 0; 
}