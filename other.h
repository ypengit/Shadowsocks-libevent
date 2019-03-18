#include <getopt.h>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <sys/types.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <pthread.h>
#include <thread>
#include <memory>
#include <signal.h>
#include <memory>
#include <unistd.h>
#include <string.h>
#include <queue>
#include <unordered_map>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <stdlib.h>
#include <netdb.h>
#include <limits>
#include <fcntl.h>
#include <list>

#include "structure.h"

class MemoryPool;
struct message;




class MemoryPool{
    public:
        //当前持有
        MemoryPool():hold(10){
            for(int i = 0; i < hold; ++i){
                free_list.push_back((struct message *)malloc(400));
            }
        }
        ~MemoryPool(){
            //删除free_list和used_list中的所有数据
            for(auto it = free_list.begin(); it != free_list.end(); ++it){
                free(*it);
            }
            for(auto it = used_list.begin(); it != used_list.end(); ++it){
                free(*it);
            }
        }
        struct message * get_instance(){
            printf("free is %d, used is %d!\n", free_list.size(), used_list.size());
            if(free_list.size() * 1.5 < used_list.size()){
                //扩容一倍
                for(int i = 0; i < hold; ++i){
                    free_list.push_back((struct message *)malloc(400));
                }
            }
            auto it = free_list.front();
            free_list.pop_front();
            used_list.push_back(it);
            return it;
        }
        void return_instance(struct message * ins){
            printf("free is %d, used is %d!\n", free_list.size(), used_list.size());
            if(free_list.size() > used_list.size() * 1.5 && (free_list.size() >= 5 || used_list.size() >= 5)){
                //缩容50%
                for(int i = 0; i < hold/2; ++i){
                    auto it = free_list.front();
                    free_list.pop_front();
                    free(it);
                }
            }
            for(auto it = used_list.begin(); *it == ins; ++it){
                used_list.erase(it);
            }
            free_list.push_back(ins);
        }
    private:
        int hold;
        std::list<struct message *> free_list;
        std::list<struct message *> used_list;
};


struct message{
    struct ver_method header;
    struct connect_request connect_info;
    struct sockaddr_in src_addr;
    struct sockaddr_in dst_addr;
    struct event_base * base;
    char stage;
    char select_method;
    short valid;
    int src_fd;
    int dst_fd;
    char * buff;
    struct event *ev1;
    struct event *ev2;
    struct event *ev_m;
    struct bufferevent *bev_c;
    static MemoryPool memorypool;
    struct timeval timeout;
    status error;
    message(){
        //应当从内存池中申请一块区域
        valid = 0;
        buff = new char[1460];
        bzero(buff, 1460);
        evutil_timerclear(&timeout);
        timeout.tv_sec = 1;
    }
    ~message(){
    }

    void deconstructor(){
        printf("~message is invoked!\n");
        delete [] buff;

    }

    void * operator new(size_t, struct message * t){

    }

    void * operator new(size_t){
        //placement new的时候会调用构造函数
        struct message * res = new(memorypool.get_instance())message;
        return res;
    }
    void operator delete(void * ins){
        ((struct message *)ins)->deconstructor();
        memorypool.return_instance((struct message *)ins);
    }
};



class Event{
    public:
    Event():
        base(event_base_new())
    {

    }
    struct event_base * getBase(){
        return base;
    }

    struct event_base *base;
    bool quiting = false;
};

class EventThread{
    public:
        EventThread(){
            pthread_create(&t, NULL, &loopFunc, (void *)&ev);
        }
        ~EventThread(){
            pthread_join(t, NULL);
        }
        struct event_base* get_base(){
            return ev.base;
        }
        Event ev;
        pthread_t t;
    private:
        static void *loopFunc(void * arg){
            Event *ev = (Event *)arg;
            event_base_loop(ev->base, EVLOOP_NO_EXIT_ON_EMPTY);
        }
};

class EventThreadPool{
    public:
    EventThreadPool(int num){
        for(int i = 0; i < num; ++i){
            EventThread *th = new EventThread();
            vec.push_back(th);
            res.push_back(th->get_base());
        }
    }

    ~EventThreadPool(){

    }

    std::vector<struct event_base*> get_bases(){
        return res;
    }

    std::vector<struct event_base*> res;
    std::vector<EventThread*> vec;
};
