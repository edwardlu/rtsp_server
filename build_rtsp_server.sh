#!/bin/sh
#cd rtp_server

#Linux
g++ rtsp_server_standalone.cpp rtp_server/*.cpp ./shared_mem/*.c -I./rtp_server -I./shared_mem -I./common -I./json -I./json/nlohmann -lpthread -lrt -o rtsp_server_shm
#Arm64
#aarch64-linux-gnu-g++ rtsp_server_standalone.cpp rtp_server/*.cpp shared_mem/*.cc -I./rtp_server -I./shared_mem -I./common -I./json -I./json/nlohmann -lpthread -o rtsp_server_shm

#Qnx
#aarch64-unknown-nto-qnx7.0.0-c++ -std=gnu++11 ./*.cpp ../common/*.cpp -I./ -I../common -I../json/nlohmann -I../json -lsocket -lc -o rtsp_server
