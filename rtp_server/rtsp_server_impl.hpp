#ifndef RTSP_IMPL_H
#define RTSP_IMPL_H
#include <iostream>

class RtspServerScoektImpl
{
public:
	RtspServerScoektImpl() = default;
	virtual ~RtspServerScoektImpl() = default;

	virtual bool InitRtspServer() = 0;
	virtual void RtspServerRun() = 0;
	virtual void StopPlaying() = 0;
	
	bool GetRtspServerStatus()
	{
		return Running;
	}
	
protected:
	bool Running = false;
};
#endif
