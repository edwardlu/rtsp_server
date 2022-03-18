#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

//1920X1080 I-Frame less than 200KB
#define NALU_PACKAGE_MAX_LEN 1024*1024
//UDP MAX Package length less than 1500 Bytes
#define RTP_MAX_PKT_SIZE 1400 
