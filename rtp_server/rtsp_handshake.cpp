#include "rtsp_handshake.hpp"

const char* GetLineFromBuf(const char* InBuf, char* OutLine)
{
	if((InBuf == nullptr)||(OutLine == nullptr))
		return nullptr;

	while(*InBuf != '\n')
	{
		*OutLine = *InBuf;
		OutLine++;
		InBuf++;
	}
	//add separater and tail
	*OutLine = '\n';
	OutLine++;
	*OutLine = '\0';

	InBuf++;

	const char *NextLine = InBuf;

	return NextLine;
}

void Parse_AddressFromUrl(std::string& Url,std::string& Addr)
{
	std::string Temp;
	//if need DNS Resolve than reslove name
	auto url_position_start_temp = Url.find("rtsp");
	auto url_position_end_temp = Url.find("\r");
	Temp = Url.substr(url_position_start_temp+7,url_position_end_temp - url_position_start_temp);
	auto url_position_end = Temp.find(":"); 
	Addr = Temp.substr(0, url_position_end);
}

void Parse_H264FileFromUrl(std::string& Url,std::string& File)
{
	//if need DNS Resolve than reslove name
	auto live_position_start = Url.find("live");
	auto video_position_start = Url.find("video");
	
	if(live_position_start != Url.npos)
	{
		auto live_position_end = Url.find("\r");
		File = Url.substr(live_position_start,live_position_end - live_position_start);
	} else if(video_position_start != Url.npos) {
		auto video_position_end  = Url.find("\r");
		File = Url.substr(video_position_start,video_position_end - video_position_start);
	} else {
		File = "default";
	}
}

//for RTCP port just add 1 to RTP port
void Parse_ClientRtpPorts(std::string& Transport,std::string& Port)
{
	//Transport: RTP/AVP;unicast;client_port=rtp-ports-rtcp=ports\r\n
	auto port_position_start = Transport.find("=");
	auto port_position_end = Transport.find("-");
	Port = Transport.substr(port_position_start+1,port_position_end-port_position_start-1);
}

void Parse_ClientRtspCommand_Basic(const char *in, 
		std::string& Cmd, std::string& Url, std::string& Cseq, std::string& RtpPort)
{
	char LineTemp[CLIENT_CMD_MAX_LEN];
	memset(LineTemp,'0',CLIENT_CMD_MAX_LEN);
	const char *Next = GetLineFromBuf(in, LineTemp);
	std::string line_1st = LineTemp;

	memset(LineTemp,'0',CLIENT_CMD_MAX_LEN);
	Next = GetLineFromBuf(Next,LineTemp);
	std::string line_2st = LineTemp;
	
	auto cmd_position_start = line_1st.find(" ");
	Cmd = line_1st.substr(0,cmd_position_start);

	auto url_position_end = line_1st.find("RTSP");
	Url = line_1st.substr(cmd_position_start+1,url_position_end-cmd_position_start-1);

	auto seq_position_start = line_2st.find(" ");
	auto seq_position_end = line_2st.find("\r");
	Cseq = line_2st.substr(seq_position_start+1, seq_position_end - seq_position_start-1);
	
	if(Cmd == "SETUP")
	{
		memset(LineTemp,'0',CLIENT_CMD_MAX_LEN);
		while(true) 
		{
			Next = GetLineFromBuf(Next, LineTemp);
			std::string Line = LineTemp;
			if(Line.find("Transport") != std::string::npos)
				break;
		}
		std::string line_3st = LineTemp;
		Parse_ClientRtpPorts(line_3st, RtpPort);
	}
}

void Process_CMD_OPTIONS(std::string& RtpOpt, int ICseq)
{
	RtpOpt.clear();
	
	char result[CLIENT_REP_MAX_LEN];
	
	sprintf(result, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Public: OPTIONS, DESCRIBE, SETUP, PLAY\r\n"
			"\r\n", ICseq);

	RtpOpt = result;
}

void Process_CMD_DESCRIBE(std::string& RtpDes, std::string& IpAddr, std::string& Url, int ICseq)
{
	RtpDes.clear();
	
	char sdp[CLIENT_REP_MAX_LEN];
	char result[CLIENT_REP_MAX_LEN];

	sprintf(sdp,"v=0\r\n"
			"o=tusimple_rtsp_live 9%ld 1 IN IP4 %s\r\n"
			"t=0 0\r\n"
			"a=control:*\r\n"
			"m=video 0 RTP/AVP 96\r\n"
			"a=rtpmap:96 H264/90000\r\n"
			"a=control:track0\r\n", time(NULL), IpAddr.c_str());
	
	sprintf(result, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Content-Base: %s\r\n"
			"Content-type: application/sdp\r\n"
			"Content-length: %d\r\n"
			"\r\n"
			"%s", ICseq, Url.c_str(), (int)strlen(sdp), sdp);
			
	RtpDes = result;
}

void Process_CMD_SETUP(std::string& RtpSetUP, int ICseq, int CliRtpPort, int SerRtpPort)
{
	RtpSetUP.clear();
	
	char result[CLIENT_REP_MAX_LEN];
	
	sprintf(result, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Transport: RTP/AVP;unicast;client_port=%d-%d;server_port=%d-%d\r\n"
			"Session: 12345678\r\n"
			"\r\n", ICseq, CliRtpPort, CliRtpPort+1, SerRtpPort, SerRtpPort+1);
			
	RtpSetUP = result;
}

void Process_CMD_PLAY(std::string& RtpPlay, int ICseq)
{
	RtpPlay.clear();

	char result[CLIENT_REP_MAX_LEN];
	
	sprintf(result, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Range: npt=0.000-\r\n"
			"Session: 66334873; timeout=60\r\n"
			"\r\n", ICseq);

	RtpPlay = result;
}

void Process_CMD_TEARDOWN(std::string& RtpTearDown, int ICseq)
{
	RtpTearDown.clear();

	char result[CLIENT_REP_MAX_LEN];

	sprintf(result, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n" , ICseq);

	RtpTearDown = result;
}
