#include "shared_mem.h"
#include <iostream>
bool stop = false;

void sig_int(int sig_no)
{
	stop = true;
}

int main()
{
	int sh_fd = 0;
	unsigned char *sh_addr = NULL;
	
	signal(SIGINT,sig_int);

	sh_addr = create_shared_mem(sh_fd);
	if(sh_addr == NULL)
	{
		std::cout<<"failed to create shared mem"<<std::endl;
	}
	struct region *ptr = (struct region *)sh_addr;
	std::cout<<ptr->region_desc<<std::endl;
	writer_post(sh_addr);
	unsigned int frame_count = 0;
	while(stop == false)
	{
		writer_wait(sh_addr);

		usleep(1000*1000);
		set_frame_count(sh_addr,frame_count);
		frame_count += 1;
		reader_post(sh_addr);
	}
	
	destory_sem(sh_addr);
	recycle_shared_mem(sh_fd,false);
	return 0;
}
