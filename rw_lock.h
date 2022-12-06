#ifndef __RW_LOCK_H__
#define __RW_LOCK_H__

#include <pthread.h>

struct RWLock{
	int readers; 
	int writers;

	pthread_mutex_t	mutex; // a protector

	pthread_cond_t signal_read;
	pthread_cond_t signal_write;

};


#endif //__RW_LOCK_H__
