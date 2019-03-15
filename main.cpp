#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <unistd.h>
#include <signal.h>
#include <thread>
#include <fcntl.h>


int called = 0;

void signal_cb(evutil_socket_t fd, short event, void * arg){
    if(event & EV_TIMEOUT){
        printf("EV_TIMEOUT is invoked!\n");
    }
    else if(event & EV_READ){
        printf("EV_READ is invoked!\n");
    }
    struct event * signal = (struct event *)arg;

    printf("signal_cb: got signal %d\n", event_get_signal(signal));
    if(called >= 2){
        event_del(signal);
    }
    called++;
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

    int flag,old_flag;

    struct sockaddr_in ad, client;
    ad.sin_port = htons(3000);
    ad.sin_family = AF_INET;
    ad.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(socket_fd, (struct sockaddr*)&ad, sizeof(ad));
    listen(socket_fd, 5);

    socklen_t len = sizeof(client);


    //int accept_fd = accept(socket_fd, (struct sockaddr*)&client, &len);

    //flag = fcntl(accept_fd, F_GETFL, 0);
    //flag |= O_NONBLOCK;
    //flag = fcntl(accept_fd, F_SETFL, flag); //将连接套接字设置为非阻塞。

    //printf("accept_fd is %d\n", accept_fd);


    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 3;


    signal_init = event_new(base, 0, EV_READ|EV_TIMEOUT, signal_cb, event_self_cbarg());

    event_add(signal_init, &tv);

    event_base_loop(base, EVLOOP_NONBLOCK);
    //event_base_loop(base, 0);
    //event_base_dispatch(base);

    printf("After dispatching\n");

    t.join();
    event_free(signal_init);
    event_base_free(base);
    return 0;
}
