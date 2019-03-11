#include <arpa/inet.h>
#include <netinet/in.h>
#include <event2/event.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <iostream>


#define PORT 8888
#define TRANSMIT_TYPE "tcp"


int main(int argc, char ** argv){
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0){
        perror("socket error!");
    }

    struct sockaddr_in addr;

    bzero(&addr, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    socklen_t len = sizeof(addr);

    int connect_fd = connect(socket_fd, (struct sockaddr*)&addr, len);

    std::cout << connect_fd << std::endl;
    char buff[200] = "write some messages!\n";
    while(true){
        int size = write(socket_fd, buff, strlen(buff) + 1);
        std::cout << size << std::endl;
    }
    return 0;
}
