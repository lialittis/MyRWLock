#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "rw_lock.h"

/* Mini Test 2
 *
 * We create 5 reading threads and 5 writing threads, we test the unique mutual exclusion of writers
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
	sleep(1); // sleep 1 second
    	printf(">>>> num = %d, reading thread id: %ld, @ %ld (time) <<<<\n", num, pthread_self(),time(NULL));
	reader_unlock(&lock);
	return NULL;
}

int main()
{

	// Initialize RW lock
	init_RWlock(&lock);

	printf("\n#######################################\n");
	printf("#   Mini Test 2 : Mutual Exclusion    #\n");
	printf("# * Check the unique mutual exclusion #\n");
	printf("#   of writers                        #\n");
	printf("#######################################\n\n");

	// create threads
    	pthread_t readers[5] = {0};
    	pthread_t writers[5] = {0};
	
    	for (int i = 0; i < 5; i++) {
        	pthread_create(&(readers[i]), NULL, read_num, NULL);
        	pthread_create(&(writers[i]), NULL, add_one, NULL);
    	}
	
    	for (int i = 0; i < 5; i++) {
    		pthread_join(readers[i], NULL);
    		pthread_join(writers[i], NULL);
    	}
	return 0;
}
