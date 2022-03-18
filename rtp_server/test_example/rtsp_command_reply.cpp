#include <iostream>
#include <string>
#include <string.h>


#define SESSION_CONTENT 12345678

char RTSP_PAYLOAD[256] = "PLAY rtsp://192.168.1.1:8554/live_video RTSP/1.0\r\nCSeq: 1\r\nRequire: implicit-play\r\nProxy-Require: gzipped-messages\r\n";
char RTSP_PAYLOAD_SETUP[256] = "SETUP rtsp://10.134.1.55:8554/test_1.h264/track0 RTSP/1.0\r\nCSeq: 4\r\nUser-Agent: LibVLC/3.0.8 (LIVE555 Streaming Media v2018.02.18)\r\nTransport: RTP/AVP;unicast;client_port=39592-39593\r\n";

int main()
{
	std::string Rtsp_Des;
	std::string Rtsp_Set;
	std::string Rtsp_Cmd;
	std::string Rtsp_Url;
	std::string Rtsp_Seq;
	std::string Rtsp_Adr;
	std::string Rtp_Port;
	Parse_ClientRtspCommand_Basic(RTSP_PAYLOAD,Rtsp_Cmd,Rtsp_Url,Rtsp_Seq,Rtp_Port);
	std::cout<<"CMD : "<<Rtsp_Cmd<<" size : "<<Rtsp_Cmd.length()<<std::endl;
	std::cout<<"URL : "<<Rtsp_Url<<" size : "<<Rtsp_Url.length()<<std::endl;
	std::cout<<"SEQ : "<<Rtsp_Seq<<" size : "<<Rtsp_Seq.length()<<std::endl;

	Parse_AddressFromUrl(Rtsp_Url,Rtsp_Adr);
	std::cout<<"Adr : "<<Rtsp_Adr<<" size : "<<Rtsp_Adr.length()<<std::endl;

	Process_CMD_DESCRIBE(Rtsp_Des,Rtsp_Adr,Rtsp_Url,atoi(Rtsp_Seq.c_str()));
	std::cout<<"Sdp : "<<Rtsp_Des<<std::endl;
	std::cout<<"==========================================================="<<std::endl;
	Parse_ClientRtspCommand_Basic(RTSP_PAYLOAD_SETUP,Rtsp_Cmd,Rtsp_Url,Rtsp_Seq,Rtp_Port);
	std::cout<<"CMD : "<<Rtsp_Cmd<<" size : "<<Rtsp_Cmd.length()<<std::endl;
	std::cout<<"URL : "<<Rtsp_Url<<" size : "<<Rtsp_Url.length()<<std::endl;
	std::cout<<"SEQ : "<<Rtsp_Seq<<" size : "<<Rtsp_Seq.length()<<std::endl;
	std::cout<<"POT : "<<Rtp_Port<<" size : "<<Rtp_Port.length()<<std::endl;
	
	Process_CMD_SETUP(Rtsp_Set, atoi(Rtsp_Seq.c_str()), atoi(Rtp_Port.c_str()),1086);
	
	std::cout<<"Set "<<Rtsp_Set<<std::endl;
	std::cout<<"==========================================================="<<std::endl;
	return 0;
}
