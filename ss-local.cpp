#include <sys/socket.h>
#include <event2/event.h>
#include <errno.h>


struct msg{

};


void handleConn(int fd, short event, void * arg){

}


int main(int argc, char ** argv){
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in src_addr;
    if(bind(socket_fd, (sockaddr*)&src_addr, sizeof(src_addr)) == -1){
        perror("bind");
        exit(1);
    }

    if(listen(socket_fd, 5) == -1){
        perror("listen");
        exit(1);
    }

    struct event_base *base = event_base_new();
    while(true){
        struct sockaddr_t dst_addr;
        struct socklen_t len = sizeof(dst_addr);
        int src_fd = accept(socket_fd, (struct sockaddr*)&dst_addr, &len);
        struct message msg;
        struct event *ev = event_new(base, src_fd, EV_READ, handleFromClient, (void *) msg);
        event_add(ev, NULL);
        event_base_loop(base, EVLOOP_NONBLOCK);
    }
}
