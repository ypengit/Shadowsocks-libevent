//#pragma pack(1)
struct method_select_request{
    char ver;
    char nmethods;
    char methods[0];
};

struct method_select_response{
    char ver;
    char method;
};

struct connect_request{
    char ver;
    char rep;
    char rsv;
    char atyp;
};

struct recv_data_request{
    char ver;
    char cmd;
    char rsv;
    char atyp;
};
