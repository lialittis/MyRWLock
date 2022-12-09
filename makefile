cc:=gcc
CFLAGS:= -Wall -lpthread

.PHONY:all clean

all:complet_test test1 test2 test3 mini_test1 mini_test2 mini_test3

mini_test1:mini_test1.c rw_lock.c
	$(cc) -o mini_test1 $^ $(CFLAGS) 

mini_test2:mini_test2.c rw_lock.c
	$(cc) -o mini_test2 $^ $(CFLAGS) 

mini_test3:mini_test3.c rw_lock.c
	$(cc) -o mini_test3 $^ $(CFLAGS) 

test1:test1.c rw_lock.c
	$(cc) -o test1 $^ $(CFLAGS) 

test2:test2.c rw_lock.c
	$(cc) -o test2 $^ $(CFLAGS) 

test3:test3.c rw_lock.c
	$(cc) -o test3 $^ $(CFLAGS) 

complet_test:complet_test.c rw_lock.c
	$(cc) -o complet_test $^ $(CFLAGS) 
clean:
	-rm complet_test test1 test2 test3 mini_test1 mini_test2 mini_test3 
