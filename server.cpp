#include <arpa/inet.h>
#include <netinet/in.h>
#include <event2/event.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sys/epoll.h>


#define SERVER_ADDRESS "0.0.0.0"
#define PORT 8888
#define TRANSMIT_TYPE "tcp"


int main(int argc, char ** argv){
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0){
        perror("socket error!");
    }

    struct sockaddr_in server_addr;
    
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);


    if(bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        perror("bind error");
        exit(1);
    }

    if(listen(socket_fd, 20) == -1){
        perror("listen error");
        exit(1);
    }

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    int accept_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &len);
    if(accept_fd < 0){
        perror("accept error");
        exit(1);
    }

    std::cout << socket_fd <<std::endl;

    //fd_set rfds;
    //FD_ZERO(&rfds);
    //FD_SET(accept_fd, &rfds);

    int epoll_fd = epoll_create(100);

    struct epoll_event accept_event;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, accept_fd, &accept_event);

    while(1){
        int epoll_wait_res = epoll_wait(epoll_fd, &accept_event, 100, 0);
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, accept_fd, &accept_event);
        if(epoll_wait_res < 0){
            perror("epoll_wait zero condition");
        }
        else{
            char buff[1024];
            memset(buff, '\0', sizeof(buff));
            int size = read(accept_fd, buff, sizeof(buff));
            printf("%s", buff);
            std::cout << epoll_wait_res << std::endl;
        }
    }
    close(accept_fd);
    close(socket_fd);
    return 0;
}
