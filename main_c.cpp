#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <unistd.h>
#include <signal.h>
#include <thread>


int called = 0;

void signal_cb(evutil_socket_t fd, short event, void * arg){
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
    struct sockaddr_in ad, client;
    ad.sin_port = htons(3000);
    ad.sin_family = AF_INET;
    ad.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(socket_fd, (struct sockaddr*)&ad, sizeof(ad));
    listen(socket_fd, 5);

    bind(socket_fd, (struct sockaddr*)&ad, sizeof(ad));
    socklen_t len = sizeof(client);
    int connect_fd = connect(socket_fd, (struct sockaddr*)&client, len);



    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 3;


    signal_init = event_new(base, socket, EV_READ, signal_cb, event_self_cbarg());
    event_add(signal_init, &tv);

    event_base_loop(base, EVLOOP_ONCE);
    printf("After dispatching\n");

    t.join();
    event_free(signal_init);
    event_base_free(base);
    return 0;
}
