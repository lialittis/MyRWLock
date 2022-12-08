cc:=gcc
CFLAGS:= -Wall -lpthread

.PHONY:all clean

all:test1 test2 mini_test1 mini_test2 mini_test3

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

clean:
	-rm  test1 test2 mini_test1 mini_test2 mini_test3 
