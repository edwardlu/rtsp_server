#include "shared_mem.h"

unsigned char * create_shared_mem(int shm_fd)
{
	unsigned char *shm_region = NULL;

	shm_fd = shm_open(SHARED_MEM_REGION_NAME, O_RDWR | O_CREAT, 0777 );
	if(shm_fd == -1)
	{
		printf("Open failed:%s\n", strerror(errno));
		return NULL;
	}

	if(ftruncate(shm_fd, MAX_DATA_AREA_LEN) == -1 )
	{
		printf("ftruncate: %s\n", strerror(errno));
		return NULL;
	}

	/* Map the memory object */

	shm_region = (unsigned char *)mmap(0,MAX_DATA_AREA_LEN,
			PROT_READ | PROT_WRITE,
			MAP_SHARED, shm_fd, 0);
	if(shm_region == MAP_FAILED)
	{
		printf("mmap failed: %s\n", strerror(errno));
		return NULL;
	}

	return shm_region;
}

void recycle_shared_mem(int shm_fd,bool need_recycle)
{
	close(shm_fd);
	if(need_recycle == true)
		shm_unlink(SHARED_MEM_REGION_NAME);
}

bool init_sem(unsigned char *shm_addr)
{
	int ret = 0;
	struct region *addr = (struct region *)shm_addr;

	ret = sem_init(&addr->sem_write,1,0);
	if(ret < 0)
	{
		printf("sem write init failed\n");
		return false;
	}

	ret = sem_init(&addr->sem_read,1,0);
	if(ret < 0)
	{
		printf("sem read init failed\n");
		return false;
	}

	return true;
}

bool destory_sem(unsigned char *shm_addr)
{
	int ret = 0;
	struct region *addr = (struct region *)shm_addr;
	ret = sem_destroy(&addr->sem_write);
	if(ret < 0)
	{
		printf("sem write destory failed\n");
		return false;
	}
	ret = sem_destroy(&addr->sem_read);
	if(ret < 0)
	{
		printf("sem read destory failed\n");
		return false;
	}

	return true;
}

void init_shared_mem(unsigned char *shm_addr)
{
	struct region *addr = (struct region *)shm_addr;
	if(addr == NULL)
		printf("Invaild addr\n");
	else 
		strncpy(addr->region_desc,"RTP_REGION",MAX_DESC_LEN);
}

int writer_wait(unsigned char *shm_addr)
{
	struct region *addr = (struct region *)shm_addr;
	return sem_trywait(&addr->sem_write);
}

void writer_post(unsigned char *shm_addr)
{
	struct region *addr = (struct region *)shm_addr;
	sem_post(&addr->sem_write);
}

void reader_wait(unsigned char *shm_addr)
{
	struct region *addr = (struct region *)shm_addr;
	sem_wait(&addr->sem_read);
}

void reader_post(unsigned char *shm_addr)
{
	struct region *addr = (struct region *)shm_addr;
	sem_post(&addr->sem_read);
}

unsigned char *get_data_area(unsigned char *shm_addr)
{
	return shm_addr + DATA_OFFSET;
}

void set_byte_used(unsigned char *shm_addr,unsigned int size)
{
	struct region *addr = (struct region *)shm_addr;
	addr->byte_used = size;
}

unsigned int get_byte_used(unsigned char *shm_addr)
{
	struct region *addr = (struct region *)shm_addr;
	return addr->byte_used;
}

void set_frame_count(unsigned char *shm_addr,unsigned int counts)
{
	struct region *addr = (struct region *)shm_addr;
	addr->frame_count = counts;
}

unsigned int get_frame_count(unsigned char *shm_addr)
{
	struct region *addr = (struct region *)shm_addr;
	return addr->frame_count;
}

