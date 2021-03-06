#ifndef _RTP_H_
#define _RTP_H_

#include <stdint.h>

#define RTP_VERSION 2
#define RTP_PAYLOAD_TYPE_H264 96
#define RTP_PAYLOAD_TYPE_AAC 97
#define RTP_HEADER_SIZE 12

/*
 *             RTP Header
 *    0                   1                   2                   3
 *    7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |V=2|P|X|  CC   |M|     PT      |       sequence number         |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                           timestamp                           |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |           synchronization source (SSRC) identifier            |
 *   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *   |            contributing source (CSRC) identifiers             |
 *   :                             ....                              :
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

struct RtpHeader{
	/* byte 0 */
	uint8_t csrcLen:4;
	uint8_t extension:1;
	uint8_t padding:1;
	uint8_t version:2;

	/* byte 1 */
	uint8_t payloadType:7;
	uint8_t marker:1;

	/* byte 2,3 */
	uint16_t seq;

	/* byte 4-7 */
	uint32_t timestamp;

	/* byte 8-11 */
	uint32_t ssrc;
};

struct RtpPacket{
	struct RtpHeader rtpHeader;
	uint8_t payload[0];
};

void RtpInitHead(struct RtpPacket* rtpPacket, uint8_t csrcLen, uint8_t extension,
		uint8_t padding, uint8_t version, uint8_t payloadType, uint8_t marker,
		uint16_t seq, uint32_t timestamp, uint32_t ssrc);

void RtpHeadChangeHostNet(struct RtpPacket* rtpPacket);

void RtpChangeHeadNetHost(struct RtpPacket* rtpPacket);

int rtpSendPacket(int socket, const char* ip, int16_t port, struct RtpPacket* rtpPacket, uint32_t dataSize);
#endif  // _RTP_H_
