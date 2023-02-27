// ---------------------------------------
// Based on: https://github.com/oviano/sokol-multithread
// ---------------------------------------


#ifndef Semaphore_H
#define Semaphore_H

// ----------------------------------------------------------------------------------------------------

#include <mutex>
#include <condition_variable>

// ----------------------------------------------------------------------------------------------------

class Semaphore
{
public:
	Semaphore() {}
	~Semaphore() {}
	
	void release()
	{
		std::scoped_lock<std::mutex> lock(m_mutex);
		m_count ++;
		m_cv.notify_one();
	}
	
	void acquire()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		while(!m_count)
		{
			m_cv.wait(lock);
		}
		m_count --;
	}
	
private:
	uint32_t m_count = 0;
	std::mutex m_mutex;
	std::condition_variable m_cv;
};

#endif
