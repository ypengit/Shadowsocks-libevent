#include <getopt.h>
#include "server.h"
#include <time.h>
#include <string.h>

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
    server_port = 1080;
    password = "123456";
    method = "chacha20";

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

    Server server(server_ip, server_port, password, method, threadNum);
    server.start();

    return 0;
}
