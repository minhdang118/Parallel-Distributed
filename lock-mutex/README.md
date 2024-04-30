# Read-Write Lock

## How to run
1. Open `test.cpp` and modify the type of the lock l (`ReadWriteLock`, `PrioritizeWriteLock`, or `MyMutexLock`).
2. Add the modified `test.cpp` and the `tests` folder to the submission folder.
3. Open a terminal and navigate to the submission folder.
4. Compile the files using `g++ -o out *.cpp -lpthread`.
5. Run the program using `./out`.
6. Observe `results.txt` for the results of that specific lock type.

## Explanation
1. ReadWriteLock (Also prioritizes readers)
    - This lock is implemented using 2 atomic variables, `numReaders` and `numWriters`. A condition variable `condvar` is used to make the thread wait until the lock is available.
    - A unique lock is also used to protect the condition variable. After the main lock is released, the condition variable notifies the waiting thread.
    - The lock already prioritizes readers, so no additional implementation for a lock prioritizing readers is needed.

2. PrioritizeWriteLock
    - This lock is implemented similarly to the `ReadWriteLock` but with an additional atomic boolean variable `isWriterAcquired`. This variable is used to prioritize writers. The variable `numWriters` also changes in purpose to signify if there is a writer wanting to acquire the lock.

3. MyMutexLock
    - This lock is implemented as a simple spin lock in a newly implemented standard mutex. An atomic boolean variable `interlock` is used to signify if the lock is acquired. If the lock is acquired, the thread will spin until the lock is released. `readLock` and `writeLock` is simply just the lock of the new mutex lock. `readUnlock` and `writeUnlock` is simply just the unlock of the mutex lock.

## Results
Recorded in `summary.txt`.

## Comments
The results for the standard mutex is faster than the ReadWriteLock and PrioritizeWriteLock.