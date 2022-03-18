#include <iostream>
#include <string>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "socket_head_file.hpp"
#include "rtp.hpp"

class RtpSender
{
public:
	RtpSender() = default;
	~RtpSender();

	bool CreateRtpSender();
	int RtpSendPacket();
	bool InitClientAddr(std::string& RtpSIp,std::string& RtpCIp,int RtpSPort,int RtpCPort);
	int RtpSendH264Frame(char *NaluFrame,int NaluLen);
private:
	int RtpSendPacket(int PackLen);

	int RtpSkFd;
	
	std::string RtpServerIp;
	std::string RtpClientIp;
	int RtpServerPort;
	int RtpClientPort;
	
	struct RtpPacket* RtpPack;
	struct sockaddr_in CliAddr;
};
