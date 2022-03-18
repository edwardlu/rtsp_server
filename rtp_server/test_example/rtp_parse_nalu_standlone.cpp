#include "media_source.hpp"
#include "get_media_data.hpp"

char H264VideoFrame[NALU_PACKAGE_MAX_LEN];

int main(int argc,char *argv[])
{
	int NaluLength;
	const std::string File = argv[1];

	MediaSource *Source = new FileOperation(File);
	bool Inited = Source->InitMeidaSource();
	
	for(;;)
	{
		Source->GetNaluData(H264VideoFrame,NaluLength);
		
		if(Source->IsMediaSourceFinish() == true)
			break;
	}

	return 0;
}
