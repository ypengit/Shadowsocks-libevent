//#pragma pack(1)

struct ver_method_select{
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


