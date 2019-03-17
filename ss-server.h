//#pragma pack(1)

#define SOCKS_VERSION4 0x04
#define SOCKS_VERSION5 0x05

//Sock报文,沟通加密方法
struct ver_method {
    char ver;
    char method;
};

struct connect_request{
    char ver;
    char rep;
    char rsv;
    char atyp;
};
struct connect_response{
    char ver;
    char rep;
    char rsv;
    char atyp;
    uint32_t ip;
    unsigned short port;
};

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



//*********************各种错误的原因************************//
//  0.ERROR_CONNECT_CLIENT          未连接上客户端
//  1.ERROR_PROTOCOL                传输协议出错
//  2.ERROR_VALID                   传输数据位数目错误，不符合sock5
//  3.ERROR_CONNECT_SERVER          未连接上远端服务器
//  4.ERROR_TRANSMISSION            传输错误
//
//
//
//
//
//
//
//*********************各种错误的原因************************//

enum status{
    ERROR_CONNECT_CLIENT,
    ERROR_PROTOCOL,
    ERROR_VALID,
    ERROR_CONNECT_SERVER,
    ERROR_TRANSMISSION
};



