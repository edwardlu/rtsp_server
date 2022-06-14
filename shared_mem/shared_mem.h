#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdbool.h>

#define SHARED_MEM_REGION_NAME "/RTP_DATA"
#define MAX_DESC_LEN 32
#define MAX_DATA_AREA_LEN 1024*1024*4

struct region
{
	char region_desc[MAX_DESC_LEN];
	sem_t sem_read;
	sem_t sem_write;

	unsigned int byte_used;
	unsigned int frame_count;
};

#define DATA_OFFSET 512


unsigned char *create_shared_mem(int shm_fd);
void recycle_shared_mem(int shm_fd,bool need_recycle);
void init_shared_mem(unsigned char *shm_addr);

bool init_sem(unsigned char *shm_addr);
bool destory_sem(unsigned char *shm_addr);

int writer_wait(unsigned char *shm_addr);
void writer_post(unsigned char *shm_addr);
//int reader_wait(unsigned char *shm_addr,int ms);
void reader_wait(unsigned char *shm_addr);
void reader_post(unsigned char *shm_addr);

unsigned char *get_data_area(unsigned char *shm_addr);
void set_byte_used(unsigned char *shm_addr,unsigned int size);
unsigned int get_byte_used(unsigned char *shm_addr);
void set_frame_count(unsigned char *shm_addr,unsigned int counts);
unsigned int get_frame_count(unsigned char *shm_add);
