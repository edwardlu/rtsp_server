#ifndef MEDIA_OPERATION_H
#define MEDIA_OPERATION_H

#include <iostream>
#include <string>
#include "rtp_sender.hpp"

#define NALU_TYPE_SLICE 1
#define NALU_TYPE_DPA 2
#define NALU_TYPE_DPB 3
#define NALU_TYPE_DPC 4
#define NALU_TYPE_IDR 5
#define NALU_TYPE_SEI 6
#define NALU_TYPE_SPS 7
#define NALU_TYPE_PPS 8
#define NALU_TYPE_AUD 9
#define NALU_TYPE_EOSEQ 10
#define NALU_TYPE_EOSTREAM 11
#define NALU_TYPE_FILL 12

#ifdef MEDIA_SOURCE_STANDLONE
class MediaSource
#else
class MediaSource : public RtpSender
#endif
{
public:
	MediaSource() = default;
	virtual ~MediaSource() = default;
	virtual bool IsMediaSourceFinish() = 0;
	virtual bool InitMeidaSource() = 0;
	virtual void GetNaluData(char *NaluStart,int& NaluLen) = 0;

	void RtpSenderInit(std::string& SIp,std::string& CIp,int SPort,int CPort)
	{
		RtpSender::CreateRtpSender();
		bool Ret = RtpSender::InitClientAddr(SIp, CIp, SPort, CPort);
		if(Ret == false)
			std::cout<<"MediaSource Init RtpSender failed"<<std::endl;
	}
	void RtpSendH264Frame(char *NALU,int Len)
	{
		int StrippedLen = Len-4;
		RtpSender::RtpSendH264Frame(&NALU[4],StrippedLen); //start with 0 0 0 1 so skip
	}
private:
};

#endif
