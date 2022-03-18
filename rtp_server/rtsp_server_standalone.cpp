#include "rtsp_server_impl.hpp"
#include "rtsp_server_socket.hpp"
#include <string>
#include <signal.h>

RtspServerScoektImpl *RSI;

void SigInt(int signo)
{
	std::cout<<"Signal received : "<<signo<<std::endl;
	RSI->StopPlaying();
}

int main(int argc,char *argv[])
{
	bool ret;
	

	if(argc < 2)
	{
		std::cout<<"Please add rtsp_server_config.json"<<std::endl;
		return 1;
	}

	std::string config_file = argv[1];
	
	signal(SIGINT, SigInt);

	RtspServerScoektImpl *Rtsp = new RtspServerSocket(config_file);
	RSI = Rtsp;

	ret = Rtsp->InitRtspServer();
	if(ret == true)
	{
			Rtsp->RtspServerRun();
	}

	delete Rtsp;
	
	return 0;
}
