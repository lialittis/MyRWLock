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
1. All Readers could access/read the data at the same time. => No need of mutual extension among readers.
2. When one writer is writing the data, no other writers or readers could access the data. => 
There are explicit mutual extension relationships between the writer and reader, and also among the writers.
=> Use binary semaphore/mutex to protect critical section of writers.
3. Tackling the herd effect => Avoiding waking up many threads at the same time, and they all try to access
the resource and cause the problem of large-scale concurrent competition in clusters => 
4. No dead locks
5. No live locks
6. No Starvation

## 
