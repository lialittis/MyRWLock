#include "rw_lock.h";

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // mutex
pthread_cond_t signal = PTHREAD_COND_INITIALIZER;  // conditional signal

int r = 0, w = 0;

void reader_lock(struct RWLock *lock_p){
	// set up the mutex protector
	pthread_mutex_lock(&(lock_p->mutex));
	
	// 1. check if there are writers waiting (writing-preferring)
	while(lock_p->writers>0) 
		pthread_cond_wait(&(lock_p->signal_read),&(lock_p->mutex));

	// 2. increase the number of current readers
	lock_p->readers++;

	// release the mutex protector
	pthread_mutex_unlock(&(lock_p->mutex));
}


void reader_unlock(struct RWLock *lock_p){
	// set up the mutex protector
	pthread_mutex_lock(&(lock_p->mutex));

	// 1. decrement the number of current readers
	lock_p->readers--;

	// 2. check if this is the last readers and there are waiting writers
	if(lock_p->readers == 0 || lock_p->writers > 0)
		pthread_cond_signal(&(lock_p->signal_write));
	
	// release the mutex protector
	pthread_mutex_unlock(&(lock_p->mutex));
}


void writer_lock(struct RWLock *lock_p){
	// set up the mutex protector
	pthread_mutex_lock(&(lock_p->mutex));
	
	// 1. check if there are writers waiting or readers reading => unique mutual exclusion
	while(lock_p->writers > 0 || lock_p->readers > 0)
		pthread_cond_wait(&signal,&mutex);

	// 2. increment the number of current writers
	lock_p->writers++;
	
	// release the mutex protector
	pthread_mutex_unlock(&(lock_p->mutex));

}

void writer_unlock(struct RWLock *lock_p){
	// set up the mutex protector
	pthread_mutex_lock(&(lock_p->mutex));

	// 1. decrement the number of writers
	lock_p->writers--;

	// 2. if there are other writers waiting, signal/wake up the fron one in the queue
	// else there are readers waiting, broadcast the signal
	if(lock_p->writers){
		pthread_cond_signal(&(lock_p->signal_write));
	}else if(lock_p->readers){
		pthread_cond_broadcast(&(lock_p->signal_read));
	}

	// release the mutex protector
	pthread_mutex_unlock(&(lock_p->mutex));
}

