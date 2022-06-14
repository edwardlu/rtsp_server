#include "shared_mem.h"
#include <iostream>
bool stop = false;

void sig_int(int sig_no)
{
	stop = true;
}

int main()
{
	bool ret;
	int sh_fd = 0;
	unsigned char *sh_addr = NULL;

	signal(SIGINT,sig_int);

	sh_addr = create_shared_mem(sh_fd);
	if(sh_addr == NULL)
	{
		std::cout<<"failed to create a RTP Shared mem"<<std::endl;
	}
	init_shared_mem(sh_addr);
	ret = init_sem(sh_addr);
	if(ret == false)
		return false;

	while(stop == false)
	{
		reader_wait(sh_addr);
		std::cout<<"Read data from writer "<<get_frame_count(sh_addr)<<std::endl;	
		writer_post(sh_addr);
	}

	ret = destory_sem(sh_addr);
	if(ret == false)
		return false;

	recycle_shared_mem(sh_fd,true);
	return 0;
}
