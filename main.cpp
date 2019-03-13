#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <unistd.h>
#include <signal.h>


int called = 0;

void signal_cb(evutil_socket_t fd, short event, void * arg){
    struct event * signal = (struct event *)arg;

    printf("signal_cb: got signal %d\n", event_get_signal(signal));
    if(called >= 2){
        event_del(signal);
    }
    called++;
}



int main(int argc, char ** argv){
    struct event_base * base;
    struct event *signal_init;

    base = event_base_new();
    signal_init = evsignal_new(base, SIGHUP, signal_cb, event_self_cbarg());

    event_add(signal_init, NULL);

    event_base_dispatch(base);
    event_free(signal_init);
    event_base_free(base);
    return 0;
}
