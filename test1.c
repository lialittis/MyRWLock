#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "rw_lock.h"

/* Test 1
 *
 * We create 10 writing threads and 10 reading threads,
 * with order of R 10, then W 10;
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
	reader_lock(&lock);
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
	printf("#               Test 1                #\n");
	printf("# * Given the request of 10 reading   #\n");
	printf("#   threads first and then 10 writing #\n");
	printf("#   threads                           #\n");
	printf("# * Check the order of writing threads#\n");
	printf("#   in the waiting list and the order #\n");
	printf("#   of signaled/executed              #\n");
	printf("# * Check unique lock of writers      #\n");
	printf("# * Check concurrent readers          #\n");
	printf("#######################################\n\n");
	

	// create threads
    	pthread_t writers[10] = {0};
    	pthread_t readers[10] = {0};
	
    	for (int i = 0; i < 10; i++) {
        	pthread_create(&(readers[i]), NULL, read_num, NULL);
    	}
    	
	for (int i = 0; i < 10; i++) {
        	pthread_create(&(writers[i]), NULL, add_one, NULL);
        	printf(">>>> order:%d, writing thread id: %ld <<<<\n", i, writers[i]);
    	}
	
    	for (int i = 0; i < 10; i++) {
    		pthread_join(writers[i], NULL);
    		pthread_join(readers[i], NULL);
    	}
	return 0;
}
