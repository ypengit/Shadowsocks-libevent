//#pragma pack(1)

#define SOCKS_VERSION4 0x04
#define SOCKS_VERSION5 0x05

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

#include "other.h"

//Sock报文,沟通加密方法

//*********************Stage的各种状态***********************//
//
//  成功的状态:
//
//  stage = 0: 建立与到客户端的连接
//
//  stage = 1; 对加密方法进行沟通
//
//  stage = 2; 对目标地址和端口进行沟通
//
//  stage = 3; 建立到目标的connect
//
//  stage = 4; 成功建立到目标的connect
//
//  stage = 5; 建立双向通信后，进行双方数据发送
//
//  失败的状态:
//
//  stage = 6; 未建立到客户端的连接
//
//  stage = 7; 建立了到客户端的连接，但未建立到服务器的连接
//
//  stage = 8; 建立了双向连接，但仍出现了错误
//
//*********************Stage的各种状态***********************//






class Server{
    public:
        Server(std::string server_ip, int server_port, std::string password, std::string method, int threadNum);
        void start();
    private:
        struct event_base * get_low_load_base();
        int listenPort(struct sockaddr_in &source_addr);

        static void handleConnToServer(struct bufferevent *bev, short event, void * arg);
        static void handleConnFromMain(int fd, short event, void * arg);
        static int close_events_and_fds(struct message * msg);

        static void handleReadFromServer(int fd, short event, void * arg);
        static void handleReadFromClient(int fd, short event, void * arg);
        static int handleWriteToClient(void * arg);
        static int handleWriteToServer(void * arg);

        std::string server_ip;
        std::string password;
        std::string method;
        int server_port;
        int threadNum;
        EventThreadPool eventThreadPool;
        std::vector<struct event_base *> events;
        std::vector<struct event_base*> bases;
};
