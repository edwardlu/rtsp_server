#include <string>
#include <string.h>
#include <time.h>

#define CLIENT_CMD_MAX_LEN 512
#define CLIENT_REP_MAX_LEN 512
const char* GetLineFromBuf(const char* InBuf, char* OutLine);

void Parse_AddressFromUrl(std::string& Url,std::string& Addr);

void Parse_H264FileFromUrl(std::string& Url,std::string& File);

//for RTCP port just add 1 to RTP port
void Parse_ClientRtpPorts(std::string& Transport,std::string& Port);

void Parse_ClientRtspCommand_Basic(const char *in, 
		std::string& Cmd, std::string& Url, std::string& Cseq, std::string& RtpPort);

void Process_CMD_OPTIONS(std::string& RtpOpt, int ICseq);

void Process_CMD_DESCRIBE(std::string& RtpDes, std::string& IpAddr, std::string& Url, int ICseq);

void Process_CMD_SETUP(std::string& RtpSetUP, int ICseq, int CliRtpPort, int SerRtpPort);

void Process_CMD_PLAY(std::string& RtpPlay, int ICseq);

void Process_CMD_TEARDOWN(std::string& RtpTearDown, int ICseq);
