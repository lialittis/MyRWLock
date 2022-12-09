#ifndef __RW_LOCK_H__
#define __RW_LOCK_H__

#include <pthread.h>

struct RWLock{
	int waiting_readers;
	int reading_threads;
	int waiting_writers;
	int writing_threads; 	// {0,1}

	pthread_mutex_t	mutex;  // protector for global variables

	pthread_cond_t signal_read; 	// condition signal to read
	pthread_cond_t signal_write;	// condition signal to write

};

void reader_lock(struct RWLock *lock_p);

void reader_lock_inorder(struct RWLock *lock_p);

void reader_unlock(struct RWLock *lock_p);

void writer_lock(struct RWLock *lock_p);

void writer_unlock(struct RWLock *lock_p);

void init_RWlock(struct RWLock *lock_p);

#endif //__RW_LOCK_H__
