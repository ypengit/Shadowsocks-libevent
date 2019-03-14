#include <sys/socket.h>
#include <event2/event.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

void write_cb(int fd, short event, void * arg){
    char buff[] = "message";
    int valid = write(fd, buff, sizeof(buff));
    printf("valid is %d\n", valid);
    if(valid > 0){
        for(int i = 0; i < valid; ++i){
            printf("%x ", buff[i]);
        }
    }
    std::cout << std::endl;
}
int main(){
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in ad;
    ad.sin_family = AF_INET;
    ad.sin_port = htons(8000);
    inet_aton("127.0.0.1", &ad.sin_addr);
    printf("%x\n", ad.sin_addr.s_addr);
    event_base * base = event_base_new();
    int conn_res;
    while(true){
        if((conn_res = connect(socket_fd, (sockaddr *)&ad, sizeof(ad)) == 0)){
            event *ev = event_new(base, socket_fd, EV_READ|EV_WRITE, write_cb, NULL);
            event_add(ev, NULL);
            event_base_loop(base, 0);
            break;
        }
    }

    printf("%d\n", socket_fd);
    printf("Program is running here!\n");
}
