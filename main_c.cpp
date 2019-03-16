#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <unistd.h>
#include <signal.h>
#include <thread>
#include <fcntl.h>
#include <unistd.h>

int setnoblocking(int fd){
    int flag,old_flag;

    //这个msg->dst完全可以设定为非阻塞的,因为它已经给分配了fd
    flag = fcntl(fd, F_GETFL, 0);
    flag |= O_NONBLOCK;
    flag = fcntl(fd, F_SETFL, flag); //将连接套接字设置为非阻塞。

    return flag;
}

int setblocking(int fd){
    int flag,old_flag;

    //这个msg->dst完全可以设定为非阻塞的,因为它已经给分配了fd
    flag = fcntl(fd, F_GETFL, 0);
    flag &= ~O_NONBLOCK;
    flag = fcntl(fd, F_SETFL, flag); //将连接套接字设置为非阻塞。

    return flag;
}

int called = 0;

void signal_cb(evutil_socket_t fd, short event, void * arg){
    struct event * signal = (struct event *)arg;
    if(event & EV_TIMEOUT){
        printf("EV_TIMEOUT is invoked!\n");
        event_del(signal);
    }
    else if(event & EV_READ){
        printf("EV_READ is invoked!\n");
        char buff[1024];
        int valid = read(fd, buff, 1024);
        if(valid > 0){
            printf("%d chars is read\n", valid);
            printf("%s", buff);
        }
        else{
            printf("0 chars is read\n");
            event_del(signal);
        }
    }
    else if(event & EV_CLOSED){
        shutdown(fd, SHUT_RDWR);
        event_del(signal);
    }

    printf("signal_cb: got signal %d\n", event_get_signal(signal));
}

void func(){
    while(true){

    }
}


int main(int argc, char ** argv){
    std::thread t (func);
    struct event_base * base;
    struct event *signal_init;

    base = event_base_new();
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in ad, client;
    ad.sin_port = htons(3000);
    ad.sin_family = AF_INET;
    ad.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(socket_fd, (struct sockaddr*)&ad, sizeof(ad));
    listen(socket_fd, 5);

    socklen_t len = sizeof(client);


    setnoblocking(socket_fd);
    int accept_fd = accept(socket_fd, (sockaddr*)&client, &len);

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 3;


    printf("Program is running here!\n");
    signal_init = event_new(base, accept_fd, EV_READ|EV_CLOSED|EV_TIMEOUT|EV_PERSIST, signal_cb, event_self_cbarg());

    event_add(signal_init, &tv);

    //while(true){
        event_base_loop(base, EVLOOP_NO_EXIT_ON_EMPTY);
    //}
    //event_base_loop(base, EVLOOP_ONCE);
    //event_base_loop(base, 0);
    //event_base_dispatch(base);

    printf("After dispatching\n");

    t.join();
    event_free(signal_init);
    event_base_free(base);
    return 0;
}
