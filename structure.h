#pragma once
#include <stdint.h>

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
