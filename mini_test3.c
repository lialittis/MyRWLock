#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "rw_lock.h"

/* Mini Test 3
 *
 * We create 5 reading threads and 5 writing threads, we test the preference of writers
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
	printf("#   Mini Test 3 : Writing Preferring  #\n");
	printf("# * Check the preference of writers   #\n");
	printf("#   by a specific order of request    #\n");
	printf("#   Request: R 2;W 1;R 1;W 1;R 2;W 2. #\n");
	printf("#######################################\n\n");

	// create threads
    	pthread_t readers[5] = {0};
    	pthread_t writers[5] = {0};

	// A specific order of request :
	// Request : read twice, write once, read once, write once, read twice, write twice
	// But the order of execution of critical section is not like request
        pthread_create(&(readers[0]), NULL, read_num, NULL); // reading thread 1s
        pthread_create(&(readers[1]), NULL, read_num, NULL); // reading thread 1s
	pthread_create(&(writers[0]), NULL, add_one, NULL);  // writing thread 2s
        pthread_create(&(readers[2]), NULL, read_num, NULL); // reading thread 1s
	pthread_create(&(writers[1]), NULL, add_one, NULL);  // writing thread 2s
        pthread_create(&(readers[3]), NULL, read_num, NULL); // reading thread 1s
        pthread_create(&(readers[4]), NULL, read_num, NULL); // reading thread 1s
	pthread_create(&(writers[3]), NULL, add_one, NULL);  // writing thread 2s
	pthread_create(&(writers[4]), NULL, add_one, NULL);  // writing thread 2s
	
    	for (int i = 0; i < 5; i++) {
    		pthread_join(readers[i], NULL);
    		pthread_join(writers[i], NULL);
    	}
	return 0;
}
