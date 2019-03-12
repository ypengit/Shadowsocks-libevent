#include <getopt.h>
#include <iostream>
#include <vector>
#include <event2/event.h>
#include <pthread.h>
#include <thread>
#include <signal.h>
#include <memory>
#include <unistd.h>
#include <string.h>
#include <queue>
#include <unordered_map>
#include <arpa/inet.h>
#include <netinet/in.h>



void quit(int fd, short event, void * arg){
    struct event_base * base = (struct event_base *)arg;
    event_base_loopbreak(base);
}



class Event{
    public:
    Event():
        base(event_base_new())
    {

    }

    //往event_base中添加一个handler,往event_base中
    //注册这个事件
    void addHandler(int fd, void (*cb)(int, short, void *)){
        struct event * ev = event_new(base, fd, 0, cb, (void *)base);
        event_add(ev, NULL);
    }

    void loop(){
        event_add(evsignal_new(base, SIGINT, quit, event_self_cbarg()), NULL);
        event_base_dispatch(base);
    }


    void startToQuit(){
        quiting = true;
        event_base_loopbreak(base);
        exit(0);
    }


    struct event_base *base;
    bool quiting = false;
};


void loopFunc(std::shared_ptr<Event> event){
    //[TODO]这里分配了event对象又该怎么做？
    //
    //自动就handle了呀，因为这些事件注册在Event这个基本对象中，
    //libevent这个库只要fd可用，就自动执行回调函数
    //loop->handleEvents();
    //
    //
    //那么，每个线程应该保有一个Event对象！！！！
    event->loop();
};



class EventThread{
    public:
        EventThread(){
            std::thread th(loopFunc, ev);
            t = &th;
        }
        ~EventThread(){
            stop();
        }

        void start(){
            t->join();
        }

        void stop(){
        }

        std::shared_ptr<Event> ev;
        std::thread *t;
};


class EventThreadPool:std::enable_shared_from_this<EventThreadPool>{
    public:
    EventThreadPool(int num){
        for(int i = 0; i < num; ++i){
            EventThread th;
            vec.push_back(&th);
        }
    }
    void start(){
        for(int i = 0; i < vec.size(); ++i){
            vec[i]->start();
        }
    }

    ~EventThreadPool(){

    }

    std::vector<EventThread*> vec;
};


//编码和解码的工作留到后面做，这里先假设不加密的情况[TODO]
//class EncryptDecrypt{
//    public:
//    static encrypt(
//};


struct message{
    struct sockaddr_in *addr;
    //柔性数组
    char data[0];
};




void handleWork(int fd, short event, void * arg){
    struct message * msg = (struct message *)arg;
    //读取从local端传来的消息，并伪装发给服务器
    //if(existed.find() != existed.end()){
    if(false){
        //这一对端口 发送地址、发送端口、目标地址、目标端口的结构体存在，说明它的读和写都有fd,可以直接读和写
        if(1){
            //从服务器发来的消息

        }
        else{
            //从local发来的消息

        }
    }
    else{
        //读取传来的消息并解析
        //创建新的socket,并与服务器通信
        struct sockaddr_in *target_addr;
        int sendfd = socket(AF_INET, SOCK_STREAM, 0);
        target_addr = msg->addr;
        if(connect(sendfd, (struct sockaddr *)target_addr, sizeof(struct sockaddr)) < 0){
            perror("connect");
            exit(1);
        }
        else{
            int validLen = write(sendfd, msg->data, strlen(msg->data));
            printf("%d chars is written!\n", validLen);
        }
    }
}


void dispatchToWorkThread(int fd, short event, void * arg){
    struct sockaddr_in *client_addr = (struct sockaddr_in*)arg;
    struct event_base * base;
    struct message msg;
    //这里在主线程就要派遣,不能留到工作线程，因为工作线程会持续不断的读取fd，这样多个线程就要重复读取，有异步问题
    struct event *ev = event_new(base, fd, 0, handleWork, (void *)&msg);
    event_add(ev, NULL);
    //超时关闭请求，后面再做[TODO]
}

void handleMainConn(int fd, short event, void * arg){
    struct event_base *base = (struct event_base *)arg;

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    int accept_fd = accept(fd, (struct sockaddr*)&client_addr, &len);

    if(accept_fd < 0){
        perror("accept error");
        exit(1);
    }
    //此时应该在主线程中监听，如果监听到事件到来，则解析并把事件派遣到工作线程
    struct event * listenEvent;
    listenEvent = event_new(base, accept_fd, EV_PERSIST, dispatchToWorkThread, (void *) &client_addr);
    event_add(listenEvent, NULL);
    event_base_dispatch(base);
}

class Server{
    public:
        //初始化服务器信息
        Server(std::string server_ip, int server_port, std::string password, std::string method, Event * mainEvent, int threadNum):
            eventThreadPool(threadNum)
        {

        }

        //创建一个线程池，用于处理来自于主线程的消息
        void start(){
            eventThreadPool.start();
            int socket_fd = listenPort();
            mainEvent->addHandler(socket_fd, handleMainConn);
            //应该在主线程中注册一个signal事件，用于结束主线程的左右事件
        }

        int listenPort(){
            int socket_fd = socket(AF_INET, communicateMethod, 0);

            struct sockaddr_in serverAddr;
            //serverAddr.sin_family = AF_INET;
            //serverAddr.sin_port = htons(server_port);
            //serverAddr.sin_addr.s_addr = htonl(server_ip.c_str());
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(7000);
            serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);



            if(bind(socket_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1){
                perror("bind");
                exit(1);
            }
            if(listen(socket_fd, 2048) == -1){
                perror("listen");
                exit(1);
            }
            //此处就结束了，将accept操作放到主线程的那个循环中去
            return socket_fd;
        }

        std::string server_ip;
        std::string password;
        std::string method;
        int server_port;
        Event * mainEvent;
        int threadNum;
        EventThreadPool eventThreadPool;
        bool quit;
        int communicateMethod;
        //小根堆线程负载均衡
        std::vector<struct event_base *> events;
};


int main(int argc, char ** argv){
    int opt;

    std::string server_ip;
    std::string local_address_ip;
    std::string password;
    std::string method;

    int server_port;
    int local_port;
    int timeout;

    char * configPath = NULL;
    const char * str = "c|s|p|l|k|m|t|h";
	while(opt = getopt(argc, argv, str)){
		switch(opt){
			case 'c':
                configPath = optarg;
                server_ip = "127.0.0.1";
                server_port = 9999;
                password = "123456";
                method = "chacha20";
                break;
			case 'h':
				std::cout << "" << std::endl
					<< "  usage:" << std::endl
					<< "" << std::endl
					<< "    ss-server" << std::endl
					<< "" << std::endl
					<< "       -s <server_host>           Host name or IP address of your remote server." << std::endl
					<< "       -p <server_port>           Port number of your remote server." << std::endl
					<< "       -l <local_port>            Port number of your local server." << std::endl
					<< "       -k <password>              Password of your remote server." << std::endl
					<< "       -m <encrypt_method>        Encrypt method: rc4-md5, " << std::endl
					<< "                                  aes-128-gcm, aes-192-gcm, aes-256-gcm," << std::endl
					<< "                                  aes-128-cfb, aes-192-cfb, aes-256-cfb," << std::endl
					<< "                                  aes-128-ctr, aes-192-ctr, aes-256-ctr," << std::endl
					<< "                                  camellia-128-cfb, camellia-192-cfb," << std::endl
					<< "                                  camellia-256-cfb, bf-cfb," << std::endl
					<< "                                  chacha20-ietf-poly1305," << std::endl
					<< "                                  salsa20, chacha20 and chacha20-ietf." << std::endl
					<< "                                  The default cipher is rc4-md5." << std::endl
					<< "" << std::endl
					<< "       [-a <user>]                Run as another user." << std::endl
					<< "       [-f <pid_file>]            The file path to store pid." << std::endl
					<< "       [-t <timeout>]             Socket timeout in seconds." << std::endl
					<< "       [-c <config_file>]         The path to config file." << std::endl
					<< "       [-n <number>]              Max number of open files." << std::endl
					<< "       [-i <interface>]           Network interface to bind." << std::endl
					<< "       [-b <local_address>]       Local address to bind." << std::endl
					<< "" << std::endl
					<< "       [-u]                       Enable UDP relay." << std::endl
					<< "       [-U]                       Enable UDP relay and disable TCP relay." << std::endl
					<< "       [-6]                       Resovle hostname to IPv6 address first." << std::endl
					<< "" << std::endl
					<< "       [-d <addr>]                Name servers for internal DNS resolver." << std::endl
					<< "       [--reuse-port]             Enable port reuse." << std::endl
					<< "       [--fast-open]              Enable TCP fast open." << std::endl
					<< "                                  with Linux kernel > 3.7.0." << std::endl
					<< "       [--acl <acl_file>]         Path to ACL (Access Control List)." << std::endl
					<< "       [--manager-address <addr>] UNIX domain socket address." << std::endl
					<< "       [--mtu <MTU>]              MTU of your network interface." << std::endl
					<< "       [--mptcp]                  Enable Multipath TCP on MPTCP Kernel." << std::endl
					<< "       [--no-delay]               Enable TCP_NODELAY." << std::endl
					<< "       [--key <key_in_base64>]    Key of your remote server." << std::endl
					<< "       [--plugin <name>]          Enable SIP003 plugin. (Experimental)" << std::endl
					<< "       [--plugin-opts <options>]  Set SIP003 plugin options. (Experimental)" << std::endl
					<< "" << std::endl
					<< "       [-v]                       Verbose mode." << std::endl
					<< "       [-h, --help]               Print this message." << std::endl;
                exit(0);
			case 's':
                server_ip = optarg;
				break;
            case 'p':
				server_port = atoi(optarg);
                break;
			case 'l':
				local_port = atoi(optarg);
                break;
            case 'k':
                password = optarg;
                break;
            case 'm':
                method = optarg;
                break;
            default:
                break;
            //先做这些选项[TODO],剩下的以后做
        }
    }

    int threadNum = 4;

    //创建监听服务器，监听事件的到来,服务器的设计
    //要考虑多个使用者的情况
    Event mainEvent;

    Server server(server_ip, server_port, password, method, &mainEvent, threadNum);
    server.start();


    //使用事件循环，来处理sock5报文,它负责将从端口
    //接收到的报文转给其他线程来处理,这个event
    //为主线程,负责将socket_fd注册到event上
    //  事件的处理用来监听主线程上端口信息的变化情
    //  况，并把信息传递给一个全局对象,这个对象是
    //  epoll的包装
    mainEvent.loop();


    return 0;
}
