#include "media_source.hpp"

#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

class FileOperation : public MediaSource
{
public:
	FileOperation(const std::string& H264File);
	~FileOperation();
	
	bool InitMeidaSource() override;
	void GetNaluData(char *NaluStart,int& NaluLen) override;
	bool IsMediaSourceFinish() override;

private:
	int GetFrameFromH264File(char *OutPut);
	void GetLastFrameFromH264File(char *OutPut);
	
	bool OpenMediaSourceFile();
	char *MmapFileContent();
	
	void InitNaluInfo(int RemainSize);
	bool NaluStartCode3Bytes(char* NaluStart);
	bool NaluStartCode4Bytes(char* NaluStart);
	uint64_t GetFileSize(const std::string& FileName);
	
	void UnmapFileContent();
	void CloseMediaSoruceFile();
	void DumpNaluHead(char *OutPut,int Len);
	
	int FdH264;

	char *FileBase;
	uint64_t FileLen;
	int NaluStart;
	int NaluEnd;
	bool Even;

	int Remain;
	int Current;
	
	std::string MeidaFile;
};

#include "shared_mem.h"

class SharedMemOperation : public MediaSource
{
public:
	SharedMemOperation() = default;
	~SharedMemOperation();
	bool InitMeidaSource() override;
	bool IsMediaSourceFinish() override;
	void GetNaluData(char *NaluStart,int& NaluLen) override;

private:
	void ShowSharedMemInfo(unsigned char *shared_mem)
	{
		struct region *reg = (struct region *)shared_mem;
		std::cout<<"block name : "<<reg->region_desc<<std::endl;
	}

	int shared_mem_fd;
	unsigned char *mem_addr = {nullptr}; 
};
