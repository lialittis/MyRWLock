# Reader/Writer Lock

This repo is trying to implement a formal reader/writer lock, with the following requirements:
- Basic reader/writer lock implementation
- Tackling the herd effect in this reader/writer lock
- The readers are serviced in their order of arrival
- Writing-preferring(The writers take precedence)
- Preserving the main invariant of a reader/wirter lock
- Using signals to avoid live locks

## Design

The design rules:
1. All Readers could access/read the data at the same time.
2. When one writer is writing the data, no other writers or readers could access the data. => 
There are explicit mutual extension relationships between the writer and reader, and also among the writers.
3. Tackling the herd effect => Avoiding waking up many threads at the same time, and they all try to access
the resource and cause the problem of large-scale concurrent competition in clusters => 
4. No dead locks
5. No live locks
6. No Starvation

**I choose to use a mutex and two signals(conditional) to implement this reader/writer lock.**
The mutex is used to protect internal counters for this lock. At then end, I encapulate the lock into a specific
struct by C programming.
I offer four counters to count : the number of reading threads, the number of writing threads(0 or 1), the number
of waiting readers and the number of waiting writers.

### Readers

**Lock:**
- STEP 1: if writers already have the lock, or they want the lock, then readers wait the reading signal(**writing-preferring**)
- STEP 2: increase the number of current reading readers

**Unlock:**
- STEP 1: decrement the number of reading readers
- STEP 2: if it is the last reading thread, and at the same time, there are at least one waiting writer, and there is no
writing thread, then signal/wake up the front writer in the waiting queue

Note : there is always a mutex to protect the internal counters.

### Writers

**Lock:**
- STEP 1: if there are writers waiting or readers reading, then the writers wait until the writing signal comes(no writing 
writers and reading readers)
- STEP 2: increase the number of waiting/current writers

**Unlock:**
- STEP 1: decrement the number of writers
- STEP 2: if there are other writers waiting, signal/wake up the front writer in the queue, else if there are readers waiting, 
**broadcast** the reading signal to all readers

Note : there is always a mutex to protect the internal counters.

## Correctness

### Shared mode of readers

Proved. Because readers only are blocked by writers, there is no mutual exclusion relationships between readers.

### Exclusive mode of writers

Proved. Because writers can be blocked by other reading or writing threads, and only one of them would get signal
with the condition of no writing threads or reading threads.

### Writing-preferring

Proved. The readers are easily blocked by a writing thread or a waiting writer, and get signals(in the way of broadcasting)
once all waiting and writing writers disappear/close.

### No Herd Effect

Ensured by the mechanism of signals.

For writing signals, using`pthread_cond_signal()` activates only one thread waiting for the condition. When there are multiple waiting threads, 
it activates/wakes up the front one in the queue order.
For reading signals, even though using `pthread_cond_broadcast()` would wake up all waiting threads for readers and this may lead
to the **herd effect**... //TODO : may be using semaphore or limite the number of readers to solve this

### Order of readers

Correct.

### No Live Lock

Proved.


## Tests

Mini Test 1: 
```
#####################################
#      Mini Test 1 : Only readers   #
# * Check the concurrency of readers#
#   by the same reading time        #
# * Check the reading order and     #
#   arriving order                  #
#####################################
```

Mini Test 2:
```
#######################################
#   Mini Test 2 : Mutual Exclusion    #
# * Check the unique mutual exclusion #
#   of writers                        #
#######################################
```

Mini Test 3:
```
#######################################
#   Mini Test 3 : Writing Preferring  #
# * Check the preference of writers   #
#   by a specific order of request    #
#   Request: R 2;W 1;R 1;W 1;R 2;W 2. #
#######################################
```

Test 1:
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


Test 2:
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
# * Check the reading order waking up #
#   and the arriving order            #
#######################################
```

