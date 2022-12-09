# Reader/Writer Lock

This repo implements a formal reader/writer lock, with the following requirements:
- Basic reader/writer lock implementation
- Tackling the herd effect in this reader/writer lock
- Writing-preferring(The writers take precedence)
- Preserving the main invariant of a reader/wirter lock
- Using signals to avoid live locks
- The readers are serviced in their order of arrival

## Design

The design rules:
1. All Readers could access/read the data at the same time(shared mode).
2. When one writer is writing the data, no other writers or readers could access the data.(exclusive mode) => 
There are explicit mutual extension relationships between the writer and reader, and also among the writers.
3. Tackling the herd effect => Avoiding waking up many waiting threads at the same time when there is no enough
resources, because they all try to access limited resources and it cause the problem of large-scale concurrent 
competition in clusters => Avoiding unnecessary context switches.
4. No dead locks => No circle waitings
5. No live locks => Signal mechanism + condition
6. No Starvation(for writers) => Writers have higher priority than readers 

7. **Special design:** all readers should be serviced in their order of arrival => the waking order of readers should
be exactly the same as the blocked order.

**I choose to use a mutex and two signals(conditional variables) to implement this reader/writer lock.**
The mutex is used to protect the conditions of signals for this lock, it cooperates with signals. In the end, 
I encapsulated the lock into a specific struct by C programming, where I offer four internal counters to construct well the conditions:
the number of reading threads, the number of writing threads(0 or 1), the number of waiting readers, and the number of waiting writers.

## Implemenation

**IMPORTANT:**
I create two ways of acquiring locks for readers. The first doesn't follow rule 7, and the second satisfies rule 7 but has lower performance.
Please check the code in `rw_lock.c` and `rw_lock.h`.

Mini tests and tests 1,2 are based on unordered readers.
Test 3 and the complete test are based on ordered readers.

### Readers

**First Lock:**
- STEP 1: if writers already have the lock, or they want the lock, then readers wait the reading signal(**writing-preferring**)
- STEP 2: increase the number of current reading readers

**Second Lock In Order:**
- STEP 1: if writers already have the lock, or they want the lock, then readers wait the reading signal(**writing-preferring**)
- STEPS IN MIDDLE : before continuing, check the local order of reader, it will wait until local order becomes the front of the waiting queue, 
then broadcast the reading signals again to other waiting readers.
- STEP 2: increase the number of current reading readers

**Unlock:**
- STEP 1: decrement the number of reading readers
- STEP 2: if it is the last reading thread, and at the same time, there are at least one waiting writer, and there is no
writing thread, then signal/wake up one of waiting writers in the waiting queue

**Note** : there is always a mutex to protect the internal counters.

### Writers

**Lock:**
- STEP 1: if there are writers waiting or readers reading, then the writers wait until the writing signal comes(no writing 
writers and reading readers)
- STEP 2: increase the number of waiting/current writers

**Unlock:**
- STEP 1: decrement the number of writers
- STEP 2: if there are other writers waiting, signal/wake up one writer in the queue, else if there are readers waiting, 
**broadcast** the reading signal to all readers.

**Note** : there is always a mutex to protect the internal counters.

## Correctness

### Shared mode of readers

Proved. Because readers only are blocked by writers, there are no mutual exclusion relationships between readers.

Please check `mini_test1`.

### Exclusive mode of writers

Proved. Because writers can be blocked by other reading or writing threads, and only one of them would get signal
with the condition of no writing threads or reading threads.

Please check `mini_test2`.

### Writing-preferring/No starvation for writers

Proved. The readers are easily blocked by a writing thread or a waiting writer and get signals(in the way of broadcasting)
once all waiting and writing writers disappear/close.

Please check `mini_test3`.

### No Herd Effect

Ensured by the mechanism of signals.

I use two separate condition variables for writers and readers, this is one of the methods to prevent spurious wakeups and herd effects.

According to the definitions of `pthread_cond_signal()` and `pthread_cond_broadcast`([ref](##Reference)), the first shall unblock 
at least one of the threads that are blocked on the specified condition variable, and the second will unblock all threads waiting 
for the same signals. 

For writers, using `broadcast` would cause a **herd effect** as only one writer is allowed to access the resource
at the end. Using `pthread_cond_signal()` and `while` condition checking for waiting threads ensures activating only one thread waiting 
for the condition. When there are multiple waiting writers, it activates/wakes up only one writer.

For reading signals, even though using `pthread_cond_broadcast()` would wake up all waiting threads for readers, personally, I
don't count this as the herd effect. Since all readers should be allowed to access the resources in shared mode, so there are no unnecessary
context switches. But the `while` condition for all waiting threads is still needed.

There could also be spurious lock conflicts (on a multiprocessor). The solution is to drop the mutex before signaling.

Please check `test1`, and `test2`.

### Order of readers

Discutable.

According to [reference](##Reference), the scheduling policy determines the order in which blocked threads are awakened.
The default scheduling policy is `SCHED_OTHER`. In our case, the readers and writers have no priorities, so when `pthread_cond_signal` or 
`pthread_cond_broadcast` call, the order of threads is not determined.

**IF WE DON'T MODIFY THE SCHEDULING POLICY**, we have to add a complicated mechanism to wake the readers up in order of arrival/blocking.
In this case, I choose to use another round of waiting and broadcast inside of all readers to ensure the order of current readers is the 
front one of the waiting queue.
This method has lower performance but it makes sure of the order of readers is as expected.

Please check `test3`.

### No Live Lock

Proved. If the readers signal one of the waiting writers, the writer would start to write. When it finishes, it would first signal other 
waiting writers then all readers. Once the readers get the signals, it means currently the writers finished their jobs, and readers
start to work until being interrupted or all done. Ther is no live lock.

Please check `complet_test`.

## Tests

Mini tests and test 1,2 are based on unordered readers.
Test 3 and complet test are based on ordered readers.

### Mini Test 1
```
#####################################
#      Mini Test 1 : Only readers   #
# * Check the concurrency of readers#
#   by the same reading time        #
# * Check the reading order and     #
#   arriving order                  #
#####################################
```

### Mini Test 2
```
#######################################
#   Mini Test 2 : Mutual Exclusion    #
# * Check the unique mutual exclusion #
#   of writers                        #
#######################################
```

### Mini Test 3
```
#######################################
#   Mini Test 3 : Writing Preferring  #
# * Check the preference of writers   #
#   by a specific order of request    #
#   Request: R 2;W 1;R 1;W 1;R 2;W 2. #
#######################################
```

### Test 1
```
#######################################
#               Test 1                #
# * Given the request of 10 reading   #
#   threads first and then 10 writing #
#   threads                           #
# * Check the order of writing threads#
#   in the waiting list and the order #
#   of signaled/executed              #
# * Check unique lock of writers      #
# * Check concurrent readers          #
#######################################
```


### Test 2
```
#######################################
#               Test 2                #
# * Given the request of R 5,W 5,R 5, #
#   W 5.                              #
# * Check the writing preferring      #
# * Check the order of writing threads#
#   in the waiting list and the order #
#   of signaled/executed              #
# * Check unique lock of writers      #
# * Check concurrent readers          #
#(!)Check the reading order waking up #
#   and the arriving order            #
#######################################
```

Readers not in order :
```
...
>>>> order:5, reading thread id: 140379889649408 <<<<
[reader_lock] waiting readers [1]
>>>> order:6, reading thread id: 140379881256704 <<<<
[reader_lock] waiting readers [2]
>>>> order:7, reading thread id: 140379872864000 <<<<
[reader_lock] waiting readers [3]
>>>> order:8, reading thread id: 140379864471296 <<<<
[reader_lock] waiting readers [4]
>>>> order:9, reading thread id: 140379856078592 <<<<
...
>>>> num = 10, reading thread id: 140379881256704, @ 1670546069 (time) <<<<
>>>> num = 10, reading thread id: 140379856078592, @ 1670546069 (time) <<<<
>>>> num = 10, reading thread id: 140379872864000, @ 1670546069 (time) <<<<
>>>> num = 10, reading thread id: 140379889649408, @ 1670546069 (time) <<<<
>>>> num = 10, reading thread id: 140379864471296, @ 1670546069 (time) <<<<
```


### Test 3
```
#######################################
#       Test 3 : Readers Order        #
# * Given the request of R 5,W 5,R 5, #
#   W 5.                              #
# * Check the writing preferring      #
# * Check the order of writing threads#
#   in the waiting list and the order #
#   of signaled/executed              #
# * Check unique lock of writers      #
# * Check concurrent readers          #
# * Check the reading order waking up #
#   and the arriving order            #
#######################################

Readers in order:

```
...
>>>> order:5, reading thread id: 140369449318144 <<<<
[reader_lock_inorder] waiting readers [1]
>>>> order:6, reading thread id: 140369440925440 <<<<
[reader_lock_inorder] waiting readers [2]
>>>> order:7, reading thread id: 140369432532736 <<<<
[reader_lock_inorder] waiting readers [3]
>>>> order:8, reading thread id: 140369424140032 <<<<
[reader_lock_inorder] waiting readers [4]
>>>> order:9, reading thread id: 140369415747328 <<<<
[reader_lock_inorder] waiting readers [5]
...
>>>> num = 10, reading thread id: 140369449318144, @ 1670546449 (time) <<<<
>>>> num = 10, reading thread id: 140369440925440, @ 1670546449 (time) <<<<
>>>> num = 10, reading thread id: 140369432532736, @ 1670546449 (time) <<<<
>>>> num = 10, reading thread id: 140369424140032, @ 1670546449 (time) <<<<
>>>> num = 10, reading thread id: 140369415747328, @ 1670546449 (time) <<<<
```

### Complet Test with readers in order

10 writing threads and 40 reading threads

```
#######################################
#  Complet Test with readers in order #
# * Given the request of R 5,W 2,R 10,#
#   (sleep) R 5,W 2,R10,W 1,R 10, W 5 #
# * Check the writing preferring      #
# * Check the order of writing threads#
#   in the waiting list and the order #
#   of signaled/executed              #
# * Check unique lock of writers      #
# * Check concurrent readers          #
# * Check the reading order waking up #
#   and the arriving order            #
#######################################
```


## Reference

1. [The open group based specification](https://pubs.opengroup.org/onlinepubs/009695399/functions/pthread_cond_broadcast.html)
2. [UNIX man7](https://man7.org/linux/man-pages/man7/sched.7.html)
3. [Oracle Guide](https://docs.oracle.com/cd/E19120-01/open.solaris/816-5137/6mba5vq3s/index.html)
