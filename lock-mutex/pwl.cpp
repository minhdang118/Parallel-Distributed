#include <mutex>
#include <atomic>
#include <condition_variable>

using namespace std;

class PrioritizeWriteLock {
	private:
		mutex RW_Mutex;
		condition_variable condvar;
		atomic<int> numReaders;
		atomic<int> numWriters;
        atomic<bool> isWriterAcquired;

	public:
		PrioritizeWriteLock()
		{
			//TODO
		}
		
		void init()
		{
			numReaders = 0;
			numWriters = 0;
            isWriterAcquired = false;
		}
		
		void readLock()
		{
			unique_lock<mutex> lock(RW_Mutex);
			condvar.wait(lock, [this]{
				return numWriters == 0 && !isWriterAcquired;
			});
			numReaders++;
			lock.unlock();
		}
		
		void readUnlock()
		{
			unique_lock<mutex> lock(RW_Mutex);
			numReaders--;
			if (numReaders == 0)
			{
				condvar.notify_all();
			}
		}
		
		void writeLock()
		{
			unique_lock<mutex> lock(RW_Mutex);
            numWriters = 1;
			condvar.wait(lock, [this]{
				return numReaders == 0 && !isWriterAcquired;
			});
			isWriterAcquired = true;
		}
		
		void writeUnlock()
		{
			unique_lock<mutex> lock(RW_Mutex);
			numWriters = 0;
            isWriterAcquired = false;
			condvar.notify_one();
		}
	
};