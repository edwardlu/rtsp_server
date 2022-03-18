# rtsp_server
# base on basic rtsp protocol to setup a server to communicate with rtsp client
  For now :
        only support H264 formate video media file
        only on client supported to play the video
        only support TCP socket for rtsp   - no tls
        only support UDP socket for rtp    - no TLS
        only support play local h264 video - for future add live video (h264)

# How to build
./build_rtsp_server.sh and this will generat the bin in rtp_server

# How to cross build
replace g++ in the build_rtsp_server.sh with aarch64-linux-gnu-g++
can be generate by qcc for QNX

# How to run 
on Nvidia Jetson paltform ./rtsp_server ../net_config/rtsp_server_config.json

# Modify the json config
 "RTSP_SERVER_IP"   :"192.168.1.1",
 "RTSP_SERVER_PORT" :"8654",
 "RTP_SERVER_PORT"  :"55532",
 "H264_MEDIA_FILE"  :"../data/test.264"

on remote PC install vlc and then play net stream and set : rtsp://10.134.1.64:8654/test_cif.h264
port and ip should mathch the rtsp_server config according to the json config
for rtsp server futures will be add for auth and find media files from client
add default video file test.264 if no video file matched



