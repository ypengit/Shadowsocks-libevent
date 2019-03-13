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
#include <time.h>
#include <stdlib.h>
#include <netdb.h>

#include "ss-server.h"

#define SOCKS_VERSION 0x05


struct message{
    struct sockaddr_in dst_addr;
    struct event_base * base;
    char stage;
    char select_method;
    char atyp;
    int src_fd;
    int dst_fd;
    //柔性数组
    int len;
    char * data;
};

void quit(int fd, short event, void * arg){
    exit(0);
}


struct source_addr_base{
    struct sockaddr_in * source_addr;
    struct event_base * base;
};

class Event{
    public:
    Event():
        base(event_base_new())
    {

    }

    //往event_base中添加一个handler,往event_base中
    //注册这个事件
    struct event_base * getBase(){
        return base;
    }


    struct event_base *base;
    bool quiting = false;
};


void *loopFunc(void * arg){
    Event *ev = (Event *)arg;
    //[TODO]这里分配了event对象又该怎么做？
    //
    //自动就handle了呀，因为这些事件注册在Event这个基本对象中，
    //libevent这个库只要fd可用，就自动执行回调函数
    //loop->handleEvents();
    //
    //
    //那么，每个线程应该保有一个Event对象！！！！
};

void * func(void * args){
}


class EventThread{
    public:
        EventThread(){
            pthread_create(&t, NULL, &loopFunc, (void *)&ev);
        }
        ~EventThread(){
            pthread_join(t, NULL);
        }
        Event ev;
        pthread_t t;
};





class EventThreadPool:std::enable_shared_from_this<EventThreadPool>{
    public:
    EventThreadPool(int num){
        for(int i = 0; i < num; ++i){
            std::shared_ptr<EventThread> th(new EventThread);
            vec.push_back(th);
        }
    }

    ~EventThreadPool(){

    }

    std::vector<std::shared_ptr<EventThread>> vec;
};


//编码和解码的工作留到后面做，这里先假设不加密的情况[TODO]
//class EncryptDecrypt{
//    public:
//    static encrypt(
//};






//一个连接从它开始创建，就要给他创建一个事件，这个事件随着两端的连接是否断开为标志，而
//不以其他情况为转移，只要两端的连接还在，那么就不应该断开这个连接



void handleReadFromClient(int fd, short event, void * arg){

}

void handleReadFromServer(int fd, short event, void * arg){

}

void handleWriteToClient(){

}

void handleWriteToServer(){

}




void handleMainConn(int fd, short event, void * arg){
    //msg的stage状态：
    //stage = 0 表示为初始状态
    //stage = 1 表示沟通过使用的加密方式
    //stage = 2 表示沟通完成，双方可以发送数据了
    struct message * msg = (struct message *)arg;
    //注册两个事件，用于从服务器和客户端监听消息
    //编码解码
    //1.读取版本号，确认是tcp连接还是udp连接

    while(true){
        if(msg->stage == 0){
            struct method_select_request method;
            int valid = recv(msg->src_fd, (char *)&method, sizeof(method), 0);

            if(method.ver != SOCKS_VERSION){
                //协议错误
                printf("协议错误\n");
                exit(0);
            }

            if(valid != sizeof(method)){
                //读取长度错误
                printf("读取长度错误\n");
                exit(0);
            }
            printf("size of method is %x\n", method.nmethods);
            printf("%x %x\n", method.ver, method.nmethods);
            if(method.nmethods == 0){
                msg->select_method = 0x00;
                //默认不加密
            }
            else{
                //在剩下255种加密方法中选择一种
                char select_method;
                printf("%d\n", method.nmethods);
                select_method = rand()%method.nmethods;
                char * methods = new char[method.nmethods];
                recv(msg->src_fd, methods, sizeof(methods), 0);
                msg->select_method = methods[select_method];
                printf("method is %x\n", msg->select_method);
                delete [] methods;
            }

            struct method_select_response res_method;
            res_method.ver = method.ver;
            res_method.method = msg->select_method;

            valid = send(msg->src_fd, (char *)&res_method, sizeof(res_method), 0);

            if(valid != sizeof(res_method)){

            }
            msg->stage = 1;
        }
        else if(msg->stage == 1){
            //在 stage = 1 双方握手，确定目标服务器的IP、端口和协议信息
            struct connect_request recv_req;
            //从客户端接收信息
            int valid = recv(msg->src_fd, (char *)&recv_req, sizeof(recv_req), 0);
            if(valid != sizeof(recv_req)){

            }
            int address_len;
            switch(recv_req.atyp){
                case 0x01:
                    //IPV4类型
                    address_len = 4;
                    break;
                case 0x03:
                    char tmpbuff;
                    valid = recv(msg->src_fd, &tmpbuff, 1, 0);
                    address_len = tmpbuff;
                    break;
                case 0x04:
                    break;
            }
            char * address = new char[address_len + 1];

            valid = recv(msg->src_fd, address, address_len, 0);
            if(valid != address_len){
                perror("address len");
            }

            for(int i = 0; i < address_len; ++i){
                printf("%x", address[i]);
            }
            std::cout << std::endl;

            if(recv_req.atyp == 0x03){
                //使用域名查询
                struct hostent * h;
                h = gethostbyname(address);
                if(!h){
                    int i = 0;
                    while(h->h_addr_list[i] != NULL){
                        //已经是网络字节序
                        //msg->dst_addr.sin_addr.s_addr = h->h_addr_list[i];
                        ++i;
                        break;
                    }
                }
                else{
                    //没有查询到目标主机
                }

            }
            else{
                unsigned short port;

                valid = recv(msg->src_fd, (char *)&port, 2, 0);

                printf("src port is %u\n", ntohs(port));
                printf("src addr is %s\n", inet_ntoa(*((in_addr *)((uint32_t *)address))));
                //printf("src address is %s\n", inet_ntoa());
                msg->dst_addr.sin_family = AF_INET;
                msg->dst_addr.sin_port   = port;
                msg->dst_addr.sin_addr.s_addr = *((uint32_t *)address);
                printf("add is %u\n", msg->dst_addr.sin_addr.s_addr);
                printf("add is %u\n", inet_addr("127.0.0.1"));
                printf("port is %u\n", port);
                printf("port is %u\n", htons(8000));
            }

            //如果没有在这里返回，则认为代理服务器已经获得远端服务器的ip与端口
            msg->stage = 2;
        }
        else if(msg->stage == 2){
            //连接到远端服务器
            msg->dst_fd = socket(AF_INET, SOCK_STREAM, 0);

            int valid;
            int dst_fd;

            if(connect(msg->dst_fd, (struct sockaddr *)&msg->dst_addr, sizeof(msg->dst_addr)) == 0){
                //如果成功，则向客户端发送成功连接报文
                struct connect_request resp_req;
                resp_req.ver = '\x05';
                resp_req.rep = '\x00';
                resp_req.rsv = '\x00';
                resp_req.atyp = msg->atyp;

                valid = send(msg->dst_fd, (char *)&resp_req, 4, 0);
                std::cout << "msg->dst_fd is " << msg->dst_fd << std::endl;
                std::cout << "valid is " << valid << std::endl;
            std::cout << "Program is running at here!" << std::endl;
                msg->stage = 3;
            }
            else{
                //否则，说明代理服务器无法连接到远端服务器

            }
        }
        else if(msg->stage == 3){
            //说明此时已经连接到远端服务器了，两边的连接已经通畅，此时应该注册4个事件
            event_new(msg->base, msg->src_fd, EV_READ,  handleReadFromClient, NULL);
            event_new(msg->base, msg->dst_fd, EV_READ,  handleReadFromServer, NULL);
        }
    }
    //switch(buff[0]){
    //    case 0x0:
    //        //不需要密码的情况
    //}



    //    int valid = send(msg->dst_fd, buff, sizeof(buff));
    //    //成功连接则将写事件注册到线程中
    //    struct ev_write_server = event_new(msg->base, msg->dst_fd, EV_WRITE, handleWriteToServer,  (void *)msg);
    //    struct ev_read_server  = event_new(msg->base, msg->dst_fd, EV_READ,  handleReadFromServer, (void *)msg);

    //    struct ev_write_client = event_new(msg->base, msg->src_fd, EV_WRITE, handleWriteToClient,  (void *)msg);
    //    struct ev_read_client  = event_new(msg->base, msg->src_fd, EV_READ,  handleReadFromClient, (void *)msg);

    //}
    //else{
    //    //从客户端发来的连接没有成功，应该向客户端发送连接不可达的报文
    //    close();
    //}

}

class Server{
    public:
        //初始化服务器信息
        Server(std::string server_ip, int server_port, std::string password, std::string method, Event * mainEvent, int threadNum):
            eventThreadPool(threadNum),
            mainEvent(mainEvent)
        {

        }

        //创建一个线程池，用于处理来自于主线程的消息
        void start(){
            struct sockaddr_in source_addr;
            int socket_fd = listenPort(source_addr);
            while(true){
                struct sockaddr_in client_addr;
                socklen_t len = sizeof(client_addr);
                int accept_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &len);
                struct message msg;
                msg.base = mainEvent->getBase();
                msg.src_fd = accept_fd;
                msg.stage = 0;
                struct event * ev = event_new(mainEvent->getBase(), accept_fd, EV_READ, handleMainConn, (void *)&msg);
                event_add(ev, NULL);
                event_base_dispatch(mainEvent->getBase());
                //应该在主线程中注册一个signal事件，用于结束主线程的左右事件
            }
        }

        int listenPort(struct sockaddr_in &source_addr){
            socket_fd = socket(AF_INET, SOCK_STREAM, 0);

            struct sockaddr_in serverAddr;

            bzero(&serverAddr, sizeof(serverAddr));

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

            return socket_fd;
        }

    private:
        std::string server_ip;
        std::string password;
        std::string method;
        int server_port;
        int threadNum;
        Event * mainEvent;
        EventThreadPool eventThreadPool;
        bool quit;
        int communicateMethod;
        int socket_fd;
        //小根堆线程负载均衡
        std::vector<struct event_base *> events;
};


int main(int argc, char ** argv){
    int opt;
    srand(time(0));

    std::string server_ip;
    std::string local_address_ip;
    std::string password;
    std::string method;

    int server_port;
    int local_port;
    int timeout;

    char * configPath = NULL;
    const char * str = "c|s|p|l|k|m|t|h";
    server_ip = "127.0.0.1"; server_port = 9999;
    password = "123456";
    method = "chacha20";
	//while(opt = getopt(argc, argv, str)){/*{{{*//*{{{*/
	//	switch(opt){
	//		case 'c':
    //            configPath = optarg;
    //            server_ip = "127.0.0.1";
    //            server_port = 9999;
    //            password = "123456";
    //            method = "chacha20";
    //            break;
	//		case 'h':
	//			std::cout << "" << std::endl
	//				<< "  usage:" << std::endl
	//				<< "" << std::endl
	//				<< "    ss-server" << std::endl
	//				<< "" << std::endl
	//				<< "       -s <server_host>           Host name or IP address of your remote server." << std::endl
	//				<< "       -p <server_port>           Port number of your remote server." << std::endl
	//				<< "       -l <local_port>            Port number of your local server." << std::endl
	//				<< "       -k <password>              Password of your remote server." << std::endl
	//				<< "       -m <encrypt_method>        Encrypt method: rc4-md5, " << std::endl
	//				<< "                                  aes-128-gcm, aes-192-gcm, aes-256-gcm," << std::endl
	//				<< "                                  aes-128-cfb, aes-192-cfb, aes-256-cfb," << std::endl
	//				<< "                                  aes-128-ctr, aes-192-ctr, aes-256-ctr," << std::endl
	//				<< "                                  camellia-128-cfb, camellia-192-cfb," << std::endl
	//				<< "                                  camellia-256-cfb, bf-cfb," << std::endl
	//				<< "                                  chacha20-ietf-poly1305," << std::endl
	//				<< "                                  salsa20, chacha20 and chacha20-ietf." << std::endl
	//				<< "                                  The default cipher is rc4-md5." << std::endl
	//				<< "" << std::endl
	//				<< "       [-a <user>]                Run as another user." << std::endl
	//				<< "       [-f <pid_file>]            The file path to store pid." << std::endl
	//				<< "       [-t <timeout>]             Socket timeout in seconds." << std::endl
	//				<< "       [-c <config_file>]         The path to config file." << std::endl
	//				<< "       [-n <number>]              Max number of open files." << std::endl
	//				<< "       [-i <interface>]           Network interface to bind." << std::endl
	//				<< "       [-b <local_address>]       Local address to bind." << std::endl
	//				<< "" << std::endl
	//				<< "       [-u]                       Enable UDP relay." << std::endl
	//				<< "       [-U]                       Enable UDP relay and disable TCP relay." << std::endl
	//				<< "       [-6]                       Resovle hostname to IPv6 address first." << std::endl
	//				<< "" << std::endl
	//				<< "       [-d <addr>]                Name servers for internal DNS resolver." << std::endl
	//				<< "       [--reuse-port]             Enable port reuse." << std::endl
	//				<< "       [--fast-open]              Enable TCP fast open." << std::endl
	//				<< "                                  with Linux kernel > 3.7.0." << std::endl
	//				<< "       [--acl <acl_file>]         Path to ACL (Access Control List)." << std::endl
	//				<< "       [--manager-address <addr>] UNIX domain socket address." << std::endl
	//				<< "       [--mtu <MTU>]              MTU of your network interface." << std::endl
	//				<< "       [--mptcp]                  Enable Multipath TCP on MPTCP Kernel." << std::endl
	//				<< "       [--no-delay]               Enable TCP_NODELAY." << std::endl
	//				<< "       [--key <key_in_base64>]    Key of your remote server." << std::endl
	//				<< "       [--plugin <name>]          Enable SIP003 plugin. (Experimental)" << std::endl
	//				<< "       [--plugin-opts <options>]  Set SIP003 plugin options. (Experimental)" << std::endl
	//				<< "" << std::endl
	//				<< "       [-v]                       Verbose mode." << std::endl
	//				<< "       [-h, --help]               Print this message." << std::endl;
    //            exit(0);
	//		case 's':
    //            server_ip = optarg;
	//			break;
    //        case 'p':
	//			server_port = atoi(optarg);
    //            break;
	//		case 'l':
	//			local_port = atoi(optarg);
    //            break;
    //        case 'k':
    //            password = optarg;
    //            break;
    //        case 'm':
    //            method = optarg;
    //            break;
    //        default:
    //            break;
    //        //先做这些选项[TODO],剩下的以后做
    //    }/*}}}*/
    //}/*}}}*/

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


    return 0;
}
