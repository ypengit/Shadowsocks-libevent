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


