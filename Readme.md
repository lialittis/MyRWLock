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

### Readers

**Lock:**
- STEP 1: if writers already have the lock, or they want the lock, readers wait(**writing-preferring**)
- STEP 2: increase the number of current readers


**Unlock:**
- STEP 1: decrement the number of readers
- STEP 2: if it is the last reading thread, and at the same time, there are at least one waiting writer,
signal/wake up the front writer in the waiting queue

### Writers

**Lock:**
- STEP 1: if there are writers waiting or readers reading, wait until the writing signal comes(no waiting 
writers or reading readers)
- STEP 2: increase the number of waiting/current writers // TODO : wired definition // writing writers and waiting writers


**Unlock:**
- STEP 1: decrement the number of writers
- STEP 2: if there are other writers waiting, signal/wake up the fron one in the queue, else there are readers waiting, 
broadcast the signal



