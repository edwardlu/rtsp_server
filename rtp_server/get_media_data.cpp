#include "get_media_data.hpp"

FileOperation::FileOperation(const std::string& H264File)
{
	FileLen = GetFileSize(H264File);
	InitNaluInfo(FileLen);
	MeidaFile = H264File;
}

FileOperation::~FileOperation()
{
	if(FileBase != nullptr)
	{
		UnmapFileContent();
	}
	if(FdH264 < 0)
	{
		CloseMediaSoruceFile();
	}
}

bool FileOperation::InitMeidaSource()
{
	bool Opened = OpenMediaSourceFile();
	if(Opened == false)
	{
		return false;
	}
	char *Mapped = MmapFileContent();
	if(Mapped == nullptr)
	{
		return false;
	}

	return true;
}

bool FileOperation::IsMediaSourceFinish()
{
		if(Even == false)
			return true;
		else 
			return false;
}	

void FileOperation::GetNaluData(char *NaluStart,int& NaluLen)
{
	int Len = GetFrameFromH264File(NaluStart);
	NaluLen = Len;
#ifdef DUMP_NALU_HEAD
	DumpNaluHead(NaluStart,Len);
#endif
}

//001
bool FileOperation::NaluStartCode3Bytes(char* NaluStart)
{
	if(NaluStart[0] == 0 && NaluStart[1] == 0 && NaluStart[2] == 1)
	{
		return true;
	}
	return false;
}

//0001
bool FileOperation::NaluStartCode4Bytes(char* NaluStart)
{
	if(NaluStart[0] == 0 && NaluStart[1] == 0 && NaluStart[2] == 0 && NaluStart[3] == 1)
	{
		return true;
	}
	return false;
}

int FileOperation::GetFrameFromH264File(char *OutPut)
{
	char *Ptr = FileBase + Current;

	for(int idx=0;idx<Remain;idx++)
	{
		if(Even == true)
		{
			if(NaluStartCode4Bytes(Ptr) == true)
			{
				NaluStart = idx;
				Even = false;
				Ptr+=3;
				idx+=3;
			}
		} else {
			if(NaluStartCode4Bytes(Ptr) == true)
			{
				NaluEnd = idx;
				Even = true;
				memcpy(OutPut,Ptr-(NaluEnd-NaluStart),NaluEnd-NaluStart);
				Current += idx;
				Remain = FileLen - Current;
				break;
			}
		}
		Ptr++;
	}

	if(Even == false)
	{
		GetLastFrameFromH264File(OutPut);
		return Remain;
	}

	return NaluEnd;
}

void FileOperation::GetLastFrameFromH264File(char *OutPut)
{
	char *Ptr = FileBase + Current;
	Remain = FileLen - Current;
#ifdef DEBUG_FILEOPER_DEBUG
	std::cout<<"Get last NALU remain bytes "<<Remain<<std::endl;
#endif
	memcpy(OutPut,Ptr, Remain);
}
	
void FileOperation::InitNaluInfo(int RemainSize)
{
	NaluStart = 0;
	NaluEnd = 0;
	Even = true;

	Remain = RemainSize;
	Current = 0;
}

uint64_t FileOperation::GetFileSize(const std::string& FileName)
{
	uint64_t size = 0;
	struct stat buf;
	if(stat(FileName.c_str() , &buf) != -1)
		size = static_cast<uint64_t>(buf.st_size);
	else 
		size = 0;

	return size;
}

bool FileOperation::OpenMediaSourceFile()
{
	if((FdH264 = open(MeidaFile.c_str(), O_RDWR)) < 0)
	{
		std::cout<<"Open Media Source File failed"<<std::endl;
		return false;
	}

	return true;
}

char *FileOperation::MmapFileContent()
{
	void *addr;
	if((addr = mmap(NULL, FileLen, PROT_READ|PROT_WRITE, MAP_SHARED, FdH264, 0)) == (void*)-1)
	{
		std::cout<<"Map Media Source File failed"<<std::endl;
		return nullptr;
	}
	FileBase = reinterpret_cast<char *>(addr);
	return FileBase;
}

void FileOperation::UnmapFileContent()
{
	int ret = munmap((void *)FileBase,FileLen);
	if(ret < 0)
	{
		std::cout<<"Unmap Media Source File failed"<<std::endl;
	}
}

void FileOperation::CloseMediaSoruceFile()
{
	close(FdH264);
}

void FileOperation::DumpNaluHead(char *OutPut,int Len)
{
	static uint64_t FrameCount;
	unsigned char Nalu = (unsigned char)(OutPut[4] & 0x1f);
	if(Nalu == NALU_TYPE_SLICE)
		std::cout<<"NALU_TYPE_SLICE"<<" Frame num : "<<FrameCount<<" Length : "<<Len<<std::endl;
	else if(Nalu == NALU_TYPE_DPA)
		std::cout<<"NALU_TYPE_DPA"<<" Frame num : "<<FrameCount<<" Length : "<<Len<<std::endl;
	else if(Nalu == NALU_TYPE_DPB)
		std::cout<<"NALU_TYPE_DPB"<<" Frame num : "<<FrameCount<<" Length : "<<Len<<std::endl;
	else if(Nalu == NALU_TYPE_DPC)
		std::cout<<"NALU_TYPE_DPC"<<" Frame num : "<<FrameCount<<" Length : "<<Len<<std::endl;
	else if(Nalu == NALU_TYPE_IDR)
		std::cout<<"NALU_TYPE_IDR"<<" Frame num : "<<FrameCount<<" Length : "<<Len<<std::endl;
	else if(Nalu == NALU_TYPE_SEI)
		std::cout<<"NALU_TYPE_SEI"<<" Frame num : "<<FrameCount<<" Length : "<<Len<<std::endl;
	else if(Nalu == NALU_TYPE_SPS)
		std::cout<<"NALU_TYPE_SPS"<<" Frame num : "<<FrameCount<<" Length : "<<Len<<std::endl;
	else if(Nalu == NALU_TYPE_PPS)
		std::cout<<"NALU_TYPE_PPS"<<" Frame num : "<<FrameCount<<" Length : "<<Len<<std::endl;
	else if(Nalu == NALU_TYPE_AUD)
		std::cout<<"NALU_TYPE_AUD"<<" Frame num : "<<FrameCount<<" Length : "<<Len<<std::endl;
	else if(Nalu == NALU_TYPE_EOSEQ)
		std::cout<<"NALU_TYPE_EOSEQ"<<" Frame num : "<<FrameCount<<" Length : "<<Len<<std::endl;
	else if(Nalu == NALU_TYPE_EOSTREAM)
		std::cout<<"NALU_TYPE_EOSTREAM"<<" Frame num : "<<FrameCount<<" Length : "<<Len<<std::endl;
	else if(Nalu == NALU_TYPE_FILL)
		std::cout<<"NALU_TYPE_FILL"<<" Frame num : "<<FrameCount<<" Length : "<<Len<<std::endl;
	else
		std::cout<<"NALU_TYPE_NOFOUND"<<" Frame num : "<<FrameCount<<" Length : "<<Len<<std::endl;
	
	FrameCount++;
}

SharedMemOperation::~SharedMemOperation()
{
	destory_sem(mem_addr);
	recycle_shared_mem(shared_mem_fd,true);
}

bool SharedMemOperation::InitMeidaSource()
{
	bool Ret;
	mem_addr = create_shared_mem(shared_mem_fd);
	if(mem_addr == nullptr)
	{
		std::cout<<"allocate shared mem region failed"<<std::endl;
		return false;
	}
	Ret = init_sem(mem_addr);
	if(Ret == false)
	{
		std::cout<<"init sem in shared mem failed"<<std::endl;
		return false;
	}

	return true;
}

void SharedMemOperation::GetNaluData(char *NaluStart,int& NaluLen)
{
	reader_wait(mem_addr);
	std::cout<<"get nalu at "<<get_frame_count(mem_addr)<<" frame size is "<<get_byte_used(mem_addr)<<std::endl;
	memcpy(NaluStart,(char *)get_data_area(mem_addr),get_byte_used(mem_addr));
	NaluLen = get_byte_used(mem_addr);
	writer_post(mem_addr);
}

bool SharedMemOperation::IsMediaSourceFinish()
{
	return false; //live video does not stop unless camera stops
}
