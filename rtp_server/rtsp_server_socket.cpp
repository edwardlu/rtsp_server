/* Auth : bin.lu 
 * Desc : Rtp server socket 
 * Create Server socket and listen,parse the rtps command
 * Create a media source to play H264 video by Rtp protocl 
 */
#include "rtsp_server_socket.hpp"

RtspServerSocket::RtspServerSocket(std::string& Rtsp_Config_Path)
	:Stop(true), Media(nullptr)
{
	std::string temp;
	//parse json file to get ip and port for cmd and media
	std::ifstream config(Rtsp_Config_Path);
	json j;
	config >> j;

	RtspIp = j.at("RTSP_SERVER_IP");
	temp = j.at("RTSP_SERVER_PORT");
	RtspPort = atoi(temp.c_str());
	temp = j.at("RTP_SERVER_PORT");
	RtpServerPort = atoi(temp.c_str());
	H264File = j.at("H264_MEDIA_FILE");
	
#ifdef DEBUG_RTSP
	std::cout<<"Construct RtspServerSocket info : "<<std::endl;
	std::cout<<"Rtsp server addr : "<<RtspIp<<std::endl;
	std::cout<<"Rtsp server Port : "<<RtspPort<<std::endl;
	std::cout<<"Rtp  server Port : "<<RtpServerPort<<std::endl;
	std::cout<<"Rtp  server file : "<<H264File<<std::endl;
#endif
}

RtspServerSocket::~RtspServerSocket()
{
#ifdef DEBUG_RTSP
	std::cout<<"Deconstruct RtspServerSocket info : "<<std::endl;
	std::cout<<"RtspSocketFd : "<<RtspSocketFd<<std::endl;
	std::cout<<"RtspAcceptFd : "<<RtspAcceptFd<<std::endl;
#endif
	close(RtspSocketFd);
	if(RtspAcceptFd > 0)
		close(RtspAcceptFd);
}

bool RtspServerSocket::InitRtspServer()
{
	int opt = 1;
	struct sockaddr_in addr;
	
	RtspSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if(RtspSocketFd < 0)
	{
		std::cout<<strerror(errno)<<std::endl;
		return false;
	}
	if(setsockopt(RtspSocketFd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt))<0)
	{
		std::cout<<strerror(errno)<<std::endl;
		return false;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(RtspPort);
	addr.sin_addr.s_addr = inet_addr(RtspIp.c_str());

	if(bind(RtspSocketFd, (struct sockaddr*)&addr, sizeof(struct sockaddr)) < 0)
	{
		std::cout<<strerror(errno)<<std::endl;
		return false;
	}
	return true;
}

void RtspServerSocket::RtspServerRun()
{
	bool Ret;
	Ret = RtspListen();
	if(Ret == false)
	{
		std::cout<<"Failed when RTSP listen"<<std::endl;
		Stop = true;
	} else {
		Stop = false;
	}

	while(!Stop)
	{
		Ret = RtspAcceptClient();
		if(Ret == false)
		{
			std::cout<<"Failed when RTSP accept"<<std::endl;
		}

		if(Stop == true)
		{
			//exit when receive SIG_INT 
			break;
		}
		RtspPlay();
	}
}

void RtspServerSocket::RtspPlay()
{
	int RecvLoop = 0;
	int RecvLen = 0;
	int SendLen = 0;
	std::thread MediaPlayer;

	while(true)
	{
		RecvLen = recv(RtspAcceptFd, RecvBuf, RTSP_RECV_BUF_LENGTH, 0);
		if(RecvLen <= 0)
		{
			#ifdef DEBUG_RTSP //allowed recv error to break the loop
			std::cout<<"Recv error : "<<strerror(errno)<<std::endl;
			#endif
			if(RecvLoop == 0)
			{
				std::cout<<"Close accept fd"<<std::endl;
				close(RtspAcceptFd);
			}  
			
			RecvLoop++;
			
			MediaRun = false;
			if(MediaPlayer.joinable())
			{	
				MediaPlayer.join();
			}

			if(RecvLoop == 10)
				break;

		} else {
			RecvBuf[RecvLen+1] = '\0'; //add end
			Parse_ClientRtspCommand_Basic(RecvBuf,Rtsp_Cmd,Rtsp_Url,Rtsp_Seq,Rtp_Port);

			if(Rtsp_Cmd == "OPTIONS")
			{
				Process_CMD_OPTIONS(Rtsp_Rep,atoi(Rtsp_Seq.c_str()));
			} else if(Rtsp_Cmd == "DESCRIBE") {
				Parse_AddressFromUrl(Rtsp_Url,Rtsp_Adr);
				Process_CMD_DESCRIBE(Rtsp_Rep, Rtsp_Adr, Rtsp_Url, atoi(Rtsp_Seq.c_str()));
			} else if(Rtsp_Cmd == "SETUP") {
				Process_CMD_SETUP(Rtsp_Rep, atoi(Rtsp_Seq.c_str()), atoi(Rtp_Port.c_str()), RtpServerPort);
			} else if(Rtsp_Cmd == "PLAY") {
				Process_CMD_PLAY(Rtsp_Rep, atoi(Rtsp_Seq.c_str()));
			} else if(Rtsp_Cmd == "TEARDOWN"){
				Process_CMD_TEARDOWN(Rtsp_Rep, atoi(Rtsp_Seq.c_str()));
			} else {
				std::cout<<"NO Maching Rtsp message"<<std::endl;
			}
			
			SendLen = send(RtspAcceptFd, Rtsp_Rep.c_str(), Rtsp_Rep.length(), 0);
			if(SendLen < 0)
			{
				#ifdef DEBUG_RTSP //ignore for ciient reason
				std::cout<<"Send error : "<<strerror(errno)<<std::endl;
				#endif
			} 

			if(Rtsp_Cmd == "PLAY") 
			{
				Parse_H264FileFromUrl(Rtsp_Url,MediaH264);
				if(MediaH264 != "default")
				{
				#ifdef DEBUG_RTSP
					std::cout<<"Start to paly "<<MediaH264<<std::endl;
				#endif
					MediaRun = true;
					MediaPlayer = std::thread(&RtspServerSocket::RtpPlay, this, std::ref(MediaH264));
				} else {
				#ifdef DEBUG_RTSP
					std::cout<<"Start to paly "<<H264File<<std::endl;
				#endif
					MediaRun = true;
					MediaPlayer = std::thread(&RtspServerSocket::RtpPlay, this, std::ref(H264File));
				}
			}

			if(Rtsp_Cmd == "TEARDOWN")
			{
			#ifdef DEBUG_RTSP
				std::cout<<"TEARDOWN received and ready to exit"<<std::endl;
			#endif
				close(RtspAcceptFd);
				MediaRun = false;
				if(MediaPlayer.joinable())
				{
					MediaPlayer.join();
				}
				break;
			}
		}
	}
	std::cout<<"Quit Rtsp Communication ..."<<std::endl;
}

void RtspServerSocket::RtpPlay(std::string& Mediafile)
{
	bool Ret;
	//still need to create thread
	Media = new FileOperation(Mediafile);
	Ret = Media->InitMeidaSource();
	if(Ret == true)
	{
		//SIp,CIp,SPort,CPort
		Media->RtpSenderInit(RtspIp, RtpClientIp, RtpServerPort, atoi(Rtp_Port.c_str()));

		while(MediaRun)
		{
			Media->GetNaluData(Nalu, Nlen);
			Media->RtpSendH264Frame(Nalu, Nlen);
			if(Media->IsMediaSourceFinish() == true)
				break;
		}
	}
	delete Media;
	std::cout<<"Quit RtpPlay ..."<<std::endl;
}

bool RtspServerSocket::RtspAcceptClient()
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	
	socklen_t len = sizeof(addr);

	RtspAcceptFd = accept(RtspSocketFd, (struct sockaddr*)&addr, &len);
	if(RtspAcceptFd < 0)
	{
		std::cout<<strerror(errno)<<std::endl;
		return false;
	}
	RtpClientIpAddr(addr);
	return true;
}

bool RtspServerSocket::RtspListen()
{
	if(listen(RtspSocketFd, RTSP_MAX_SERVER_BACKLOG) < 0)
	{
		std::cout<<strerror(errno)<<std::endl;
		return false;
	}
	
	return true;
}

void RtspServerSocket::RtpClientIpAddr(struct sockaddr_in& Client)
{
	RtpClientIp = inet_ntoa(Client.sin_addr);
}

void RtspServerSocket::StopPlaying()
{
	Stop = true;
}
