#!/bin/sh
cd rtp_server

#Linux
g++ ./*.cpp ../common/*.cpp -I./ -I../common -I../json/nlohmann -I../json -lpthread -g -o rtsp_server

#Arm64
#aarch64-linux-gnu-g++ ./*.cpp ../common/*.cpp -I./ -I../common -I../json/nlohmann -I../json -lpthread -o rtsp_server

#Qnx
#aarch64-unknown-nto-qnx7.0.0-c++ -std=gnu++11 ./*.cpp ../common/*.cpp -I./ -I../common -I../json/nlohmann -I../json -lsocket -lc -o rtsp_server 
