//
// Copyright (c) 2013 Juan Palacios juan.palacios.puyana@gmail.com
// Subject to the BSD 2-Clause License
// - see < http://opensource.org/licenses/BSD-2-Clause>
//

#ifndef CONCURRENT_QUEUE_
#define CONCURRENT_QUEUE_

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

/*! \brief Internal class. A thread-safe implementation of std::queue
*
* Based on https://github.com/juanchopanza/cppblog/blob/master/Concurrency/Queue/Queue.h
*/
template <typename T>
class SGB_Queue
{
public:

	/*! \brief Waits for and returns the next element and pop it from the queue.
	*
	* \returns The next element in the queue.
	*
	* If the queue is empty it will wait until an element is inserted on it.
	*
	* Use empty() to check for new elements without blocking.
	*/
	T pop()
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.empty())
		{
			cond_.wait(mlock);
		}
		auto val = queue_.front();
		queue_.pop();
		return val;
	}

	/*! \brief Waits for and sets the next element and pop it from the queue.
	*
	* \param item The next item in the queue.
	*
	* If the queue is empty it will wait until an element is inserted on it.
	*
	* Use empty() to check for new elements without blocking.
	*/
	void pop(T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.empty())
		{
			cond_.wait(mlock);
		}
		item = queue_.front();
		queue_.pop();
	}

	/*! \brief Pushes an element at the end of the queue
	*
	* \param item The item to be added at the end of the queue.
	*/
	void push(const T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		queue_.push(item);
		mlock.unlock();
		cond_.notify_one();
	}

	/*! \brief Checks if there is currently no elements on the queue.
	*
	* \returns <b>true</b> if the queue have no elements, <b>false</b> otherwise.
	*/
	bool empty()
	{
		bool val = false;

		std::unique_lock<std::mutex> mlock(mutex_);
		val = queue_.empty();
		mlock.unlock();

		return val;
	}

	/*! \brief Returns the current size of the queue.
	*
	* \returns The queue's current size.
	*/
	uint32_t size()
	{
		uint32_t val = 0;

		std::unique_lock<std::mutex> mlock(mutex_);
		val = queue_.size();
		mlock.unlock();

		return val;
	}

	SGB_Queue() = default;
	SGB_Queue(const SGB_Queue&) = delete;            // disable copying
	SGB_Queue& operator=(const SGB_Queue&) = delete; // disable assignment

private:
	std::queue<T> queue_;
	std::mutex mutex_;
	std::condition_variable cond_;
};

#endif