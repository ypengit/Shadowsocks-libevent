#include <sys/socket.h>
#include <event2/event.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

void read_cb(int fd, short event, void * arg){
    char buff[1024];
    int valid = recv(fd, buff, 1024, 0);
    if(valid > 0){
        for(int i = 0; i < valid; ++i){
            printf("%x ", buff[i]);
        }
        std::cout << std::endl;
        printf("%s\n", buff);
    }
}
int main(){
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in ad, src_ad;
    ad.sin_family = AF_INET;
    ad.sin_port = htons(8000);
    inet_aton("127.0.0.1", &ad.sin_addr);
    printf("%x\n", ad.sin_port);
    printf("%x\n", ad.sin_addr.s_addr);
    bind(socket_fd, (sockaddr*)&ad, sizeof(ad));
    listen(socket_fd, 5);
    event_base * base = event_base_new();
    socklen_t len = sizeof(src_ad);
    int accept_fd = accept(socket_fd, (sockaddr *)&src_ad, &len);
    event *ev = event_new(base, accept_fd, EV_READ, read_cb, NULL);
    event_add(ev, NULL);
    event_base_dispatch(base);
}
