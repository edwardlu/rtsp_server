#include "rtp.hpp"
#include "socket_head_file.hpp"

void RtpInitHead(struct RtpPacket* rtpPacket, uint8_t csrcLen, uint8_t extension,
				uint8_t padding, uint8_t version, uint8_t payloadType, uint8_t marker,
				uint16_t seq, uint32_t timestamp, uint32_t ssrc)
{
	rtpPacket->rtpHeader.csrcLen = csrcLen;
	rtpPacket->rtpHeader.extension = extension;
	rtpPacket->rtpHeader.padding = padding;
	rtpPacket->rtpHeader.version = version;
	rtpPacket->rtpHeader.payloadType = payloadType;
	rtpPacket->rtpHeader.marker = marker;
	rtpPacket->rtpHeader.seq = seq;
	rtpPacket->rtpHeader.timestamp = timestamp;
	rtpPacket->rtpHeader.ssrc = ssrc;
}

void RtpHeadChangeHostNet(struct RtpPacket* rtpPacket)
{
	rtpPacket->rtpHeader.seq = htons(rtpPacket->rtpHeader.seq);
	rtpPacket->rtpHeader.timestamp = htonl(rtpPacket->rtpHeader.timestamp);
	rtpPacket->rtpHeader.ssrc = htonl(rtpPacket->rtpHeader.ssrc);
}

//After sendto need to modify
void RtpChangeHeadNetHost(struct RtpPacket* rtpPacket)
{
	rtpPacket->rtpHeader.seq = ntohs(rtpPacket->rtpHeader.seq);
	rtpPacket->rtpHeader.timestamp = ntohl(rtpPacket->rtpHeader.timestamp);
	rtpPacket->rtpHeader.ssrc = ntohl(rtpPacket->rtpHeader.ssrc);
}

int rtpSendPacket(int socket, const char* ip, int16_t port, struct RtpPacket* rtpPacket, uint32_t dataSize)
{
	struct sockaddr_in addr;
	int ret;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);

	rtpPacket->rtpHeader.seq = htons(rtpPacket->rtpHeader.seq);
	rtpPacket->rtpHeader.timestamp = htonl(rtpPacket->rtpHeader.timestamp);
	rtpPacket->rtpHeader.ssrc = htonl(rtpPacket->rtpHeader.ssrc);
	
	ret = sendto(socket, (void*)rtpPacket, dataSize+RTP_HEADER_SIZE, 0,
			(struct sockaddr*)&addr, sizeof(addr));
	
	//need to change ending for these three elements,because when update
	//it would be not correct
	rtpPacket->rtpHeader.seq = ntohs(rtpPacket->rtpHeader.seq);
	rtpPacket->rtpHeader.timestamp = ntohl(rtpPacket->rtpHeader.timestamp);
	rtpPacket->rtpHeader.ssrc = ntohl(rtpPacket->rtpHeader.ssrc);

	return ret;
}
