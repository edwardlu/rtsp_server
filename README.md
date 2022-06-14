# rtsp_server

# base on basic rtsp protocol to setup a server to communicate with rtsp client
  For now : 
	only on client supported to play the video - for future add multicast 
	only support TCP socket for rtsp   - for future add TLS 
	only support UDP socket for rtp    - for future add TLS
	only support play local h264 video - for future add live video (h264)
  
# Support shared mem to get NALU from the encoder and inform the rtsp server to push
  
# How to build
  ./build_rtsp_server.sh and this will generat the bin in rtp_server

# How to cross build
  replace g++ in the build_rtsp_server.sh with aarch64-linux-gnu-g++

# How to run 
  1 on Nvidia Jetson paltform ./rtsp_server only support push local h264 file 
  on remote PC install vlc and then play net stream set : rtsp://10.134.1.64:8654/test_cif.h264
  port and ip should mathch the rtsp_server config according to the json config
  for rtsp server futures will be add for auth and find media files from client
  
  2 To support the live video push stream need to use the camera device and NvEncoder 

  
