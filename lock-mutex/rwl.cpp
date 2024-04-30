#include <mutex>
#include <atomic>
#include <condition_variable>

using namespace std;

class ReadWriteLock {
	private:
		mutex RW_Mutex;
		condition_variable condvar;
		atomic<int> numReaders;
		atomic<int> numWriters;

	public:
		ReadWriteLock()
		{
			//TODO
		}
		
		void init()
		{
			numReaders = 0;
			numWriters = 0;
		}
		
		void readLock()
		{
			unique_lock<mutex> lock(RW_Mutex);
			condvar.wait(lock, [this]{
				return numWriters == 0;
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
				condvar.notify_one();
			}
		}
		
		void writeLock()
		{
			unique_lock<mutex> lock(RW_Mutex);
			condvar.wait(lock, [this]{
				return numReaders == 0 && numWriters == 0;
			});
			numWriters++;
		}
		
		void writeUnlock()
		{
			unique_lock<mutex> lock(RW_Mutex);
			numWriters--;
			condvar.notify_all();
		}
	
};