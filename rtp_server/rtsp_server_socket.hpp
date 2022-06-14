#include <fstream>
#include <errno.h>
#include <unistd.h>
#include <thread>
#include <mutex>

#include "time.hpp"
#include "rtsp_server_impl.hpp"
#include "media_source.hpp"
#include "rtsp_handshake.hpp"
#include "socket_head_file.hpp"
#include "get_media_data.hpp"
#include "json.hpp"

#define RTSP_MAX_SERVER_BACKLOG 1 //only one
#define RTSP_RECV_BUF_LENGTH 1500
#define DEBUG_RTSP 1
using json = nlohmann::json;

class RtspServerSocket : public RtspServerScoektImpl
{
public:
	RtspServerSocket(std::string& Rtsp_Config_Path);
	~RtspServerSocket();

	virtual bool InitRtspServer() override;
	virtual void RtspServerRun() override;
	virtual void StopPlaying() override;

private:
	void RtspPlay();
	void RtpPlay(std::string& Mediafile);
	
	bool RtspAcceptClient();
	bool RtspListen();
	void RtpClientIpAddr(struct sockaddr_in& Client);
	//rtsp tcp
	std::string RtspIp;
	int RtspPort;
	int RtspSocketFd;
	int RtspAcceptFd;
	
	//rtp udp
	std::string RtpClientIp;
	int RtpServerPort;
	
	bool Stop;
	bool MediaRun;

	char RecvBuf[RTSP_RECV_BUF_LENGTH];
	
	std::string Rtsp_Rep;
	std::string Rtsp_Cmd;
	std::string Rtsp_Url;
	std::string Rtsp_Seq;
	std::string Rtsp_Adr;

	std::string Rtp_Port;
	
	//video play info
	MediaSource *Media;
	int Nlen;
	char Nalu[NALU_PACKAGE_MAX_LEN];
	std::string H264File = {};
	std::string MediaH264 = {};
};
