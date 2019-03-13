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
#include <limits>

#include "ss-server.h"

#define SOCKS_VERSION 0x05


struct message{
    struct sockaddr_in dst_addr;
    struct event_base * base;
    char stage;
    char select_method;
    char atyp;
    char rep;
    int src_fd;
    int dst_fd;
    short valid;
    struct event *ev1;
    struct event *ev2;
    struct event *ev_m;
    char * buff;
    message(){
        //应当从内存池中申请一块区域
        valid = 0;
        buff = new char[1460];
        bzero(buff, 1460);
    }
    ~message(){
        delete [] buff;
    }
};



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
        struct event_base* get_base(){
            return ev.base;
        }
        Event ev;
        pthread_t t;
};





class EventThreadPool{
    public:
    EventThreadPool(int num){
        for(int i = 0; i < num; ++i){
            EventThread th;
            vec.push_back(&th);
            res.push_back(th.get_base());
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


//编码和解码的工作留到后面做，这里先假设不加密的情况[TODO]
//class EncryptDecrypt{
//    public:
//    static encrypt(
//};






//一个连接从它开始创建，就要给他创建一个事件，这个事件随着两端的连接是否断开为标志，而
//不以其他情况为转移，只要两端的连接还在，那么就不应该断开这个连接

void close_fd(int fd, struct message *msg){
    event_del(msg->ev1);
    event_del(msg->ev2);
    close(fd);
}


void handleReadFromClient(int fd, short event, void * arg){
    struct message *msg = (struct message *)arg;

    if(event & EV_READ){
        //以太网最大MTU为1500,IP头部20字节，TCP头部20字节
        msg->valid = recv(fd, msg->buff, 1460, 0);
        if(msg->valid > 0){
            printf("%s", msg->buff);
            msg->valid = send(msg->dst_fd, msg->buff, msg->valid, 0);
            if(msg->valid == -1){
                //向服务器发送失败,删除本事件,并向客户端报告这个状态码,关闭客户端的连接
                close_fd(msg->src_fd, msg);
                return;
            }
            else{
                //只对那一部分被使用过的进行重置
                bzero(msg->buff, msg->valid);
            }
        }
        else if(msg->valid == -1){
            //虽然客户端显示有数据，但读取数据错误，应该关闭服务器的连接
            close_fd(msg->dst_fd, msg);
            return;
        }
    }
    else if(event & EV_CLOSED){
        //如果此时检测到关闭，仍旧是消去事件，并关闭另一个fd
        close_fd(msg->dst_fd, msg);
        return;
    }
}

void handleReadFromServer(int fd, short event, void * arg){
    struct message *msg = (struct message *)arg;

    if(event & EV_READ){
        //以太网最大MTU为1500,IP头部20字节，TCP头部20字节
        msg->valid = recv(fd, msg->buff, 1460, 0);
        if(msg->valid > 0){
            printf("%s", msg->buff);
            msg->valid = send(msg->src_fd, msg->buff, msg->valid, 0);
            if(msg->valid == -1){
                //向服务器发送失败,删除本事件,并向客户端报告这个状态码,关闭客户端的连接
                close_fd(msg->dst_fd, msg);
                return;
            }
            else{
                //只对那一部分被使用过的进行重置
                bzero(msg->buff, msg->valid);
            }
        }
        else if(msg->valid == -1){
            //虽然客户端显示有数据，但读取数据错误，应该关闭服务器的连接
            close_fd(msg->src_fd, msg);
            return;
        }
        else if(event & EV_CLOSED){
            //如果此时检测到关闭，仍旧是消去事件，并关闭另一个fd
            event_del(msg->ev1);
            event_del(msg->ev2);
            close_fd(msg->dst_fd, msg);
            return;
        }
    }
}



void close_m_fd(int fd, struct message *msg){
    event_del(msg->ev_m);
    close(fd);
}

void handleMainConn(int fd, short event, void * arg){
    /*
     * msg->stage状态：
     * 1. stage = 0 表示为初始状态
     * 2. stage = 1 表示沟通过使用的加密方式
     * 3. stage = 2 表示沟通完成，双方可以发送数据了
     * 4. stage = 3 表示事件被注册到基本事件集中
    */
    struct message * msg = (struct message *)arg;

    while(true){
        if(msg->stage == 0){


            //*1*:接收版本号、加密方法数目和加密方法
            //*2*:从加密方法中，选择一项加密方法，并告知客户端
            struct ver_method_select header;
            msg->valid = recv(msg->src_fd, (char *)&header, 2, 0);

            if((header.ver != SOCKS_VERSION) || (msg->valid != 2)){
                //协议错误,根据sock5协议标准，此情况下无需回答
                close_m_fd(msg->src_fd, msg);
                break;
            }

            //选择加密方式
            if(header.method == 0){
                //默认不加密
                msg->select_method = 0x00;
            }
            else{
                //在剩下255种加密方法中选择一种,长度也可能不是255，由第二字节决定
                msg->select_method = rand()%header.method;
                char * methods = new char[header.method];
                msg->valid = recv(msg->src_fd, methods, header.method, 0);
                if(msg->valid != header.method){
                    close_m_fd(msg->src_fd, msg);
                    break;
                }
                msg->select_method = methods[msg->select_method];
                delete [] methods;
            }

            //将选择出来的加密方式传给客户端
            header.method = msg->select_method;

            msg->valid = send(msg->src_fd, (char *)&header, 2, 0);

            if(msg->valid != 2){
                close_m_fd(msg->src_fd, msg);
                break;
            }
            msg->stage = 1;
        }
        else if(msg->stage == 1){
            //在 stage = 1 双方握手，确定目标服务器的IP、端口和协议信息
            struct connect_request recv_req;
            //从客户端接收信息
            msg->valid = recv(msg->src_fd, (char *)&recv_req, 4, 0);
            if(msg->valid != 4){
                close_m_fd(msg->src_fd, msg);
                break;
            }
            msg->atyp = recv_req.atyp;

            //这里最长的是域名，可能有128位，而IPV4与IPV6都只有几位，
            //用char足矣,且方便读取域名信息
            char address_len;
            switch(recv_req.atyp){
                case 0x01:
                    //IPV4类型
                    address_len = 4;
                    break;
                case 0x03:
                    msg->valid = recv(msg->src_fd, &address_len, 1, 0);
                    break;
                case 0x04:
                    break;
            }
            char * address = new char[address_len];

            msg->valid = recv(msg->src_fd, address, address_len, 0);

            if(msg->valid != address_len){
                close_m_fd(msg->src_fd, msg);
                break;
            }


            unsigned short port;
            msg->valid = recv(msg->src_fd, (char *)&port, 2, 0);

            if(msg->valid != 2){
                close_m_fd(msg->src_fd, msg);
                break;
            }

            //域名查找较为复杂，稍后完成[TODO]
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
            else if(recv_req.atyp == 0x01){
                //传递过来的数据都是网络字节序，可以直接转义使用,其他无论是
                //IPV4还是IPV6都可以直接设置传输
                msg->dst_addr.sin_family = AF_INET;
                msg->dst_addr.sin_port   = port;
                msg->dst_addr.sin_addr.s_addr = *((uint32_t *)address);
            }
            else{
                //IPV6稍后完成[TODO]

            }
            //如果没有在这里返回，则认为代理服务器已经获得远端服务器的ip与端口
            msg->stage = 2;
        }
        else if(msg->stage == 2){
            //连接到远端服务器
            msg->dst_fd = socket(AF_INET, SOCK_STREAM, 0);

            if(0 == connect(msg->dst_fd, (struct sockaddr *)&(msg->dst_addr), sizeof(msg->dst_addr))){
                //如果成功，则向客户端发送成功连接报文
                struct connect_request resp_req;
                resp_req.ver = '\x05';
                resp_req.rep = '\x00';
                resp_req.rsv = '\x00';
                resp_req.atyp = msg->atyp;

                msg->valid = send(msg->src_fd, (char *)&resp_req, 4, 0);

                if(msg->valid != 4){
                    close_m_fd(msg->src_fd, msg);
                    break;
                }

                msg->stage = 3;
            }
            else{
                //与客户端连接成功，但与远端服务器连接不成功,仅需要关闭客户端的链接，远端服务器的还未建立起来
                //此时不能简单关闭，因为还在连接建立阶段，故要根据情况返回状态码
                //[TODO]
                break;
            }
        }
        else if(msg->stage == 3){

            //说明此时已经连接到远端服务器了，两边的连接已经通畅，此时应该注册2个事件
            //每个时间应该能够知道它能否被读，已经它是否被关闭，事件应该是常备事件
            //ev1 负责从客户端读取数据发送到远端服务器
            //ev2 负责从远端服务器读取数据发送到客户端
            struct event * ev1 = event_new(msg->base, msg->src_fd, EV_READ|EV_PERSIST|EV_CLOSED,  handleReadFromClient, (void *)msg);
            struct event * ev2 = event_new(msg->base, msg->dst_fd, EV_READ|EV_PERSIST|EV_CLOSED,  handleReadFromServer, (void *)msg);

            //将两事件注册到msg上，在错误或关闭时，可以消去事件
            msg->ev1 = ev1;
            msg->ev2 = ev2;

            event_add(ev1, NULL);
            event_add(ev2, NULL);

            //这里很重要，不能使用event_base_dispatch，因为要及时返回，否则事件集很可能阻塞在这里，直到ev1,ev2各调用一次
            event_base_loop(msg->base, EVLOOP_NONBLOCK);
            msg->stage = 4;
            break;
            //第四个状态即为已经分配完全，不用再干预,它的撤销停止都放在子线程中执行
        }
    }
}

class Server{
    public:
        //初始化服务器信息
        Server(std::string server_ip, int server_port, std::string password, std::string method, int threadNum):
            eventThreadPool(threadNum)
        {
            bases = eventThreadPool.get_bases();

        }

        //创建一个线程池，用于处理来自于主线程的消息
        void start(){
            struct sockaddr_in source_addr;
            int socket_fd = listenPort(source_addr);
            //这里要循环，以在主端口上监听，如果有事件到来，那么
            while(true){
                struct message msg;
                struct sockaddr_in client_addr;
                socklen_t len = sizeof(client_addr);
                msg.src_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &len);

                //使用小根堆，获得各个线程的负载情况，找到那些正在处理事件少的线程，分配给新到来的事件
                msg.base = get_low_load_base();
                msg.stage = 0;


                //仅注册一次，分发给子线程处理，它的所有解析、传送、处理和关闭都交给子线程执行
                msg.ev_m = event_new(msg.base, msg.src_fd, EV_READ, handleMainConn, (void *)&msg);
                event_add(msg.ev_m, NULL);
                event_base_dispatch(msg.base);
                //应该在主线程中注册一个signal事件，用于结束所有存在的事件[TODO]
            }
        }

        struct event_base * get_low_load_base(){
            struct event_base * res;
            unsigned short c = 65535u, t;
            for(int i = 0; i < bases.size(); ++i){
                t = event_base_get_num_events(bases[i], EVENT_BASE_COUNT_ADDED);
                if(t < c){
                    c = t;
                    res = bases[i];
                }
            }
            return res;
        }

        int listenPort(struct sockaddr_in &source_addr){
            int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

            struct sockaddr_in serverAddr;

            bzero(&serverAddr, sizeof(serverAddr));

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
        EventThreadPool eventThreadPool;
        std::vector<struct event_base *> events;
        std::vector<struct event_base*> bases;
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
    server_ip = "127.0.0.1";
    server_port = 9999;
    password = "123456";
    method = "chacha20";
    std::unordered_map<std::string, char> methods;
    //配置文件，读取所有加密方式

	//while(opt = getopt(argc, argv, str)){/*{{{*/
	//	switch(opt){
	//		case 'c':
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
    //    }
    //}
/*}}}*/

    int threadNum = 4;

    //创建监听服务器，监听事件的到来,服务器的设计
    //要考虑多个使用者的情况

    Server server(server_ip, server_port, password, method, threadNum);
    server.start();


    //使用事件循环，来处理sock5报文,它负责将从端口
    //接收到的报文转给其他线程来处理,这个event
    //为主线程,负责将socket_fd注册到event上
    //  事件的处理用来监听主线程上端口信息的变化情
    //  况，并把信息传递给一个全局对象,这个对象是
    //  epoll的包装


    return 0;
}
