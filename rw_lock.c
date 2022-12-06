#include "rw_lock.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // mutex
pthread_cond_t signal = PTHREAD_COND_INITIALIZER;  // conditional signal

int r = 0, w = 0;

void reader_lock(){
	// 1. lock the mutex
	pthread_mutex_lock(&mutex);
	
	// 2. check if there are writers waiting (writing-preferring)
	while(w!=0) 
		pthread_cond_wait(&signal,&mutex);

	// 3. if no writers waiting, increase the number of current readers
	r++;

	// 4. unlock the mutex
	pthread_mutex_unlock(&mutex);
}


void reader_unlock(){
	// 1. lock the mutex
	pthread_mutex_lock(&mutex);

	// 2. descrise the number of current readers
	r--;

	// 3. check if this is the last readers, if so, signal the signal
	if(r==0)
		pthread_cond_signal(&signal);

	// 4. unlock the mutex
	pthread_mutex_unlock(&mutex);
}


void writer_lock(){
	// 1. lock the mutex
	
	// 2. check if there are writers waiting or readers reading
	while(w!=0 || r > 0)
		pthread_cond_wait(&signal,&mutex);

	// 3. set the writing writer as 1
	w = 1;

	// 4. unlock the mutex
	pthread_mutex_unlock(&mutex);
}

void writer_unlock(){
	// 1. lock the mutex
	pthread_mutex_lock(&mutex);

	// 2. set the writing writer as 0
	w = 0;

	// 3. broadcast the signal
	pthread_cond_broadcast(&signal);

	// 4. unlock the mutex
	pthread_mutex_unlock(&mutex);	
}

