#include <fstream>
#include <iostream>
#include <json.hpp>
#include <string>

using json = nlohmann::json;

int main()
{
	std::string path = "../rtp_server_config.json";
	std::ifstream config(path);
	json j;
	config >> j;
	
	std::cout<<"RTP_SERVER_CMD_IP : "<<j.at("RTP_SERVER_CMD_IP")<<std::endl;
	std::cout<<"RTP_SERVER_MED_IP : "<<j.at("RTP_SERVER_MED_IP")<<std::endl;
	std::cout<<"RTP_SERVER_CMD_PORT : "<<j.at("RTP_SERVER_CMD_PORT")<<std::endl;
	std::cout<<"RTP_SERVER_CMD_PORT : "<<j.at("RTP_SERVER_CMD_PORT")<<std::endl;

	return 0;
}
