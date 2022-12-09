#include "rw_lock.h"
#include <unistd.h>
#include <stdio.h>

int current_order = 0;
int global_order = 0;

void reader_lock(struct RWLock *lock_p){
	
	// set up the mutex protector
	pthread_mutex_lock(&(lock_p->mutex));
	
	// 1. check if there are writers waiting (writing-preferring) or writer writing
	// if there are waiting/writing writers, the reading threads will sleep and wait the reading signal from writers
	while(lock_p->waiting_writers > 0 || lock_p->writing_threads > 0){
		lock_p->waiting_readers++;
		printf("[%s] waiting readers [%d]\n",__func__, lock_p->waiting_readers);
		pthread_cond_wait(&(lock_p->signal_read),&(lock_p->mutex));
		lock_p->waiting_readers--;
		printf("[%s] waiting readers [%d]\n",__func__, lock_p->waiting_readers);
	}
	
	// 2. increase the number of current reading readers
	lock_p->reading_threads++;
	printf("[%s] current reading threads [%d]\n",__func__, lock_p->reading_threads);
	usleep(1000);

	// release the mutex protector
	pthread_mutex_unlock(&(lock_p->mutex));
}

void reader_lock_inorder(struct RWLock *lock_p){
	
	int local_order;

	// set up the mutex protector
	pthread_mutex_lock(&(lock_p->mutex));
	
	local_order = global_order;
	global_order++;
	//local_order = lock_p->waiting_readers;

	// 1. check if there are writers waiting (writing-preferring) or writer writing
	// if there are waiting/writing writers, the reading threads will sleep and wait the reading signal from writers
	while(lock_p->waiting_writers > 0 || lock_p->writing_threads > 0){
		
		lock_p->waiting_readers++;
		printf("[%s] waiting readers [%d]\n",__func__, lock_p->waiting_readers);
		pthread_cond_wait(&(lock_p->signal_read),&(lock_p->mutex));
		lock_p->waiting_readers--;
	
		// 1.1 check the readers order, if current order is not followed the arrival order, then wait
		while(local_order!=current_order){
			pthread_cond_wait(&(lock_p->signal_read),&(lock_p->mutex));
		}

		// 1.2 broadcast again to waiting readers
		pthread_cond_broadcast(&(lock_p->signal_read));
		
		printf("[%s] waiting readers [%d]\n",__func__, lock_p->waiting_readers);
	}

	// 1.3 increase the current order
	current_order++;
	
	// 2. increase the number of current reading readers
	lock_p->reading_threads++;
	printf("[%s] current reading threads [%d]\n",__func__, lock_p->reading_threads);
	usleep(1000);
	// release the mutex protector
	pthread_mutex_unlock(&(lock_p->mutex));
}

void reader_unlock(struct RWLock *lock_p){
	// set up the mutex protector
	pthread_mutex_lock(&(lock_p->mutex));

	// 1. decrement the number of current readers
	lock_p->reading_threads--;

	// 2. check if this is the last readers and there are waiting writers and no writing writers
	// if there is no reading threads and there are waiting writers and no writing writers, then send signal
	// to one writer in the waiting list
	if(lock_p->reading_threads == 0 && lock_p-> waiting_writers > 0 && lock_p->writing_threads == 0){
		pthread_cond_signal(&(lock_p->signal_write));
	}
		
	// release the mutex protector
	pthread_mutex_unlock(&(lock_p->mutex));
}

void writer_lock(struct RWLock *lock_p){
	// set up the mutex protector
	pthread_mutex_lock(&(lock_p->mutex));
	
	// 1. check if there are writers waiting or readers reading => unique mutual exclusion
	// the thread go sleep and wait the writing signal
	while(lock_p->reading_threads > 0 || lock_p->writing_threads > 0){
		lock_p->waiting_writers++;
		printf("[%s] waiting writers [%d]\n",__func__, lock_p->waiting_writers);
		pthread_cond_wait(&(lock_p->signal_write),&(lock_p->mutex));
		lock_p->waiting_writers--;
		printf("[%s] waiting writers [%d]\n",__func__, lock_p->waiting_writers);
	}

	// 2. increment the number of current writing threads // actually it should be 1
	lock_p->writing_threads++;
	printf("[%s] current writing threads [%d]\n",__func__, lock_p->writing_threads);
	
	// release the mutex protector
	pthread_mutex_unlock(&(lock_p->mutex));

}

void writer_unlock(struct RWLock *lock_p){
	// set up the mutex protector
	pthread_mutex_lock(&(lock_p->mutex));

	// 1. decrement the number of writing threads // actually, it is 0
	lock_p->writing_threads--;

	// 2. if there are other writers waiting, signal/wake up one writer in the waiting queue
	// else there are readers waiting, broadcast the reading signal to all waiting readers
	if(lock_p->waiting_writers){
		pthread_cond_signal(&(lock_p->signal_write));
	}else if(lock_p->waiting_readers){ // if there is no waiting writers but waiting readers
		pthread_cond_broadcast(&(lock_p->signal_read));
	}

	// release the mutex protector
	pthread_mutex_unlock(&(lock_p->mutex));
}

void init_RWlock(struct RWLock *lock_p){
	lock_p->waiting_readers=0;
	lock_p->waiting_writers=0;
	lock_p->reading_threads=0;
	lock_p->writing_threads=0;

	pthread_mutex_init(&(lock_p->mutex),NULL);
	pthread_cond_init(&(lock_p->signal_read),NULL);
	pthread_cond_init(&(lock_p->signal_write),NULL);
}
