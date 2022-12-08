#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "rw_lock.h"

/* Mini Test 1
 *
 * We create 10 reading threads, we test the concurrent of all readers, and also
 * compare the order of reading threads and the order of their arrival
 * 
 * */

// One Public Reader Writer Lock
struct RWLock lock;

// target critical data
int num = 0;

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

	printf("\n#####################################\n");
	printf("#      Mini Test 1 : Only readers   #\n");
	printf("# * Check the concurrency of readers#\n");
	printf("#   by the same reading time        #\n");
	printf("# * Check the reading order and     #\n");
	printf("#   arriving order                  #\n");
	printf("#####################################\n\n");

	// create threads
    	pthread_t readers[10] = {0};
	
    	for (int i = 0; i < 10; i++) {
        	pthread_create(&(readers[i]), NULL, read_num, NULL);
        	printf(">>>> order:%d, reading thread id: %ld <<<<\n", i, readers[i]);
    	}
	
    	for (int i = 0; i < 10; i++) {
    		pthread_join(readers[i], NULL);
    	}
	return 0;
}
