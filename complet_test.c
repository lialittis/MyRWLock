#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "rw_lock.h"

/* Complet Test(with reader in order)
 *
 * We create 10 writing threads and 40 reading threads,
 * with order of R 5, then W 2, then R 10, then main thread sleeps 10 second, then
 * R 5, then W 1, then R 10, then W 2, then R 10, then W 5.
 * 
 * */

// One Public Reader Writer Lock
struct RWLock lock;

// target critical data
int num = 0;

/* 
 * add one to num and print out
 * */
void* add_one(void *arg){
	writer_lock(&lock);
	sleep(2); // sleep 2 seconds to block other threads
    	printf(">>>> num = %d, writing thread id: %ld, @ %ld (time) <<<<\n", ++num, pthread_self(),time(NULL));
	writer_unlock(&lock);
    	return NULL;
}

/*
 * read num and print out
 * */
void* read_num(){
	reader_lock_inorder(&lock);
	sleep(1); // sleep 1 seconds
    	printf(">>>> num = %d, reading thread id: %ld, @ %ld (time) <<<<\n", num, pthread_self(),time(NULL));
	reader_unlock(&lock);
	return NULL;
}


int main()
{
	// Initialize RW lock
	init_RWlock(&lock);

	printf("\n#######################################\n");
	printf("#  Complet Test with readers in order #\n");
	printf("# * Given the request of R 5,W 2,R 10,#\n");
	printf("#   (sleep) R 5,W 2,R10,W 1,R 10, W 5 #\n");
	printf("# * Check the writing preferring      #\n");
	printf("# * Check the order of writing threads#\n");
	printf("#   in the waiting list and the order #\n");
	printf("#   of signaled/executed              #\n");
	printf("# * Check unique lock of writers      #\n");
	printf("# * Check concurrent readers          #\n");
	printf("# * Check the reading order waking up #\n");
	printf("#   and the arriving order            #\n");
	printf("#######################################\n\n");
	

	// create threads
    	pthread_t writers[10] = {0};
    	pthread_t readers[40] = {0};
	int i;
    	
	
	for (i = 0; i < 5; i++) {
        	pthread_create(&(readers[i]), NULL, read_num, NULL);
        	printf(">>>> order:%d, reading thread id: %ld <<<<\n", i, readers[i]);
		usleep(10000);
    	}
    	
	for (i = 0; i < 2; i++) {
        	pthread_create(&(writers[i]), NULL, add_one, NULL);
        	//printf(">>>> order:%d, writing thread id: %ld <<<<\n", i, writers[i]);
		usleep(10000);
    	}

    	for (i = 5; i < 15; i++) {
        	pthread_create(&(readers[i]), NULL, read_num, NULL);
        	printf(">>>> order:%d, reading thread id: %ld <<<<\n", i, readers[i]);
		usleep(10000);
    	}

	sleep(10); // main thread sleeping 10 seconds
    	
	for (i = 15; i < 20; i++) {
        	pthread_create(&(readers[i]), NULL, read_num, NULL);
        	printf(">>>> order:%d, reading thread id: %ld <<<<\n", i, readers[i]);
		usleep(10000);
    	}

	for (i = 2; i < 4; i++) {
        	pthread_create(&(writers[i]), NULL, add_one, NULL);
        	//printf(">>>> order:%d, writing thread id: %ld <<<<\n", i, writers[i]);
		usleep(10000);
    	}
    	
    	for (i = 20; i < 30; i++) {
        	pthread_create(&(readers[i]), NULL, read_num, NULL);
        	printf(">>>> order:%d, reading thread id: %ld <<<<\n", i, readers[i]);
		usleep(10000);
    	}

	for (i = 4; i < 5; i++) {
        	pthread_create(&(writers[i]), NULL, add_one, NULL);
        	//printf(">>>> order:%d, writing thread id: %ld <<<<\n", i, writers[i]);
		usleep(10000);
    	}
    	
	for (i = 30; i < 40; i++) {
        	pthread_create(&(readers[i]), NULL, read_num, NULL);
        	printf(">>>> order:%d, reading thread id: %ld <<<<\n", i, readers[i]);
		usleep(10000);
    	}

	for (i = 5; i < 10; i++) {
        	pthread_create(&(writers[i]), NULL, add_one, NULL);
        	//printf(">>>> order:%d, writing thread id: %ld <<<<\n", i, writers[i]);
		usleep(10000);
    	}
	
    	for (i = 0; i < 10; i++) {
    		pthread_join(writers[i], NULL);
    	}
    	for (i = 0; i < 40; i++) {
    		pthread_join(readers[i], NULL);
    	}

	return 0;
}
