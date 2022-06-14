#include "rtp_sender.hpp"

RtpSender::~RtpSender()
{
	delete []RtpPack;
	
	if(RtpSkFd<0)
	{ 
	} else {
		close(RtpSkFd);
	}
}

bool RtpSender::CreateRtpSender()
{
	int opt = 1;

	RtpSkFd = socket(AF_INET, SOCK_DGRAM, 0);
	if(RtpSkFd < 0)
	{
		std::cout<<"Create socket failed "<<std::endl;
		return false;
	}
	
	if(setsockopt(RtpSkFd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt))<0)
	{
		std::cout<<"Setsocketopt failed "<<std::endl;
		return false;
	}

	return true;
}

bool RtpSender::InitClientAddr(std::string& RtpSIp,std::string& RtpCIp,int RtpSPort,int RtpCPort)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	
	RtpServerIp = RtpSIp;
	RtpClientIp = RtpCIp;
	RtpServerPort = RtpSPort;
	RtpClientPort = RtpCPort;
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(RtpServerPort);
	addr.sin_addr.s_addr = inet_addr(RtpServerIp.c_str());
	
	if(bind(RtpSkFd, (struct sockaddr*)&addr, sizeof(struct sockaddr)) < 0)
	{
		std::cout<<strerror(errno)<<std::endl;
		return false;
	}
#ifdef DEBUG_RTP
	std::cout<<"Rtp Server Bind at : "<<RtpServerIp.c_str()<<" "<<RtpServerPort<<std::endl;
#endif
	CliAddr.sin_family = AF_INET;
	CliAddr.sin_port = htons((int16_t)RtpClientPort);
	CliAddr.sin_addr.s_addr = inet_addr(RtpClientIp.c_str());
#ifdef DEBUG_RTP
	std::cout<<"Rtp Client Bind at : "<<RtpClientIp.c_str()<<" "<<RtpClientPort<<std::endl;
#endif
	RtpPack = reinterpret_cast<RtpPacket *>(new char[NALU_PACKAGE_MAX_LEN]());
	RtpInitHead(RtpPack, 0, 0, 0, RTP_VERSION, RTP_PAYLOAD_TYPE_H264, 0, 0, 0, 0x88923423);
	
	return true;
}

int RtpSender::RtpSendPacket(int PackLen)
{
	int Ret;
	
	RtpPack->rtpHeader.seq = htons(RtpPack->rtpHeader.seq);
	RtpPack->rtpHeader.timestamp = htonl(RtpPack->rtpHeader.timestamp);
	RtpPack->rtpHeader.ssrc = htonl(RtpPack->rtpHeader.ssrc);
	
	Ret = sendto(RtpSkFd, (void*)RtpPack, (uint32_t)PackLen+RTP_HEADER_SIZE, 0,
			(struct sockaddr*)&CliAddr, sizeof(CliAddr));

	if(Ret <= 0)
	{
		std::cout<<"Rtp Send h264 frame failed"<<std::endl;
		return Ret;
	}
	
	//need to change ending for these three elements,because when update
	//it would be not correct
	RtpPack->rtpHeader.seq = ntohs(RtpPack->rtpHeader.seq);
	RtpPack->rtpHeader.timestamp = ntohl(RtpPack->rtpHeader.timestamp);
	RtpPack->rtpHeader.ssrc = ntohl(RtpPack->rtpHeader.ssrc);

	return Ret;
}

int RtpSender::RtpSendH264Frame(char *NaluFrame,int NaluLen)
{
	uint8_t naluType;
	int sendBytes = 0;
	int ret;

	naluType = NaluFrame[0];
	if(NaluLen <= RTP_MAX_PKT_SIZE)
	{
		memcpy(RtpPack->payload, NaluFrame, NaluLen);
		ret = RtpSendPacket(NaluLen);
		if(ret < 0)
		{
			return ret;
		}
		RtpPack->rtpHeader.seq++;
		sendBytes += ret;
	} else {
		int pktNum = NaluLen/RTP_MAX_PKT_SIZE;
		int remainPktSize = NaluLen % RTP_MAX_PKT_SIZE;
		int pos = 1;

		for(int i=0; i<pktNum; i++)
		{
			RtpPack->payload[0] = (naluType & 0x60) | 28;
			RtpPack->payload[1] = naluType & 0x1F;

			if(i == 0)
			{
				RtpPack->payload[1] |= 0x80;
			} else if(remainPktSize == 0 && i == pktNum - 1) {
				RtpPack->payload[1] |= 0x40;
			}
			
			memcpy(RtpPack->payload+2, NaluFrame+pos, RTP_MAX_PKT_SIZE);
			ret = RtpSendPacket(RTP_MAX_PKT_SIZE+2);
			if(ret < 0)  
			{
				return ret;
			}
			RtpPack->rtpHeader.seq++;
			sendBytes += ret;
			pos += RTP_MAX_PKT_SIZE;
		}

		if(remainPktSize > 0)
		{
			RtpPack->payload[0] = (naluType & 0x60) | 28;
			RtpPack->payload[1] = naluType & 0x1F;
			RtpPack->payload[1] |= 0x40;

			memcpy(RtpPack->payload+2, NaluFrame+pos, remainPktSize+2);
			ret = RtpSendPacket(remainPktSize+2);
			if(ret < 0)
			{
				return ret;
			}
			RtpPack->rtpHeader.seq++;
			sendBytes += ret;
		}
	}
	RtpPack->rtpHeader.timestamp += 90000/25;
#ifdef MEDIA_SOURCE_LOCAL_FILE
	usleep(1000*1000/25);
#endif
	return sendBytes;
}
