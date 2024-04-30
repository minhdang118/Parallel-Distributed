#include <mutex>
#include <atomic>

using namespace std;

class MyMutex
{
private:
    atomic<bool> interlock;
public:
    void lock()
    {
        while(interlock.exchange(true, memory_order_acquire))
        {
            // spin
        }
    }

    void unlock()
    {
        interlock.store(false, memory_order_release);
    }
};

class MyMutexLock
{
private:
    MyMutex my_mutex;
public:
    void init()
    {

    }
    void readLock()
    {
        my_mutex.lock();
    }

    void readUnlock()
    {
        my_mutex.unlock();
    }

    void writeLock()
    {
        my_mutex.lock();
    }

    void writeUnlock()
    {
        my_mutex.unlock();
    }
};

