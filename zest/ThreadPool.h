#pragma once
#ifndef ZEST_LIB_THREADPOOL_H
#define ZEST_LIB_THREADPOOL_H

#include <cstdint>
#include <future>
#include <mutex>
#include <queue>
#include <vector>
#include <type_traits>

namespace Zest { namespace Lib {

class Thread
{
public:
	enum class ThreadStatus
	{
		Busy,
		Stop,
		Shutting,
	};

	Thread()
		: m_status{ ThreadStatus::Stop }
	{
	}

	template<typename TFunc, typename... TArgs>
	Thread(TFunc&& func, TArgs... args) noexcept
		: m_status{ ThreadStatus::Stop }
	{
		Start(std::forward<TFunc>(func), std::forward<TArgs>(args)...);
	}

	template<typename TFunc>
	Thread(TFunc&& func) noexcept
		: m_status{ ThreadStatus::Stop }
	{
		Start(std::forward<TFunc>(func));
	}

	void Stop(bool isWaitComplete = true) noexcept
	{
		std::unique_lock<std::mutex> lock{ m_mutex };
		if (m_status == ThreadStatus::Stop)
			return;

		if (!isWaitComplete)
		{
			m_thread.detach();
		}
		else
		{
			if (m_thread.joinable())
			{
				m_thread.join();
			}
		}

		m_status = ThreadStatus::Stop;
	}

	bool IsJoinable() noexcept
	{
		std::unique_lock<std::mutex> lock{ m_mutex };
		return m_thread.joinable();
	}

	template<typename TFunc, typename... TArgs>
	bool Start(TFunc&& func, TArgs... args) noexcept
	{
		{
			std::unique_lock<std::mutex> lock{ m_mutex };
			if (m_status == ThreadStatus::Busy)
			{
				return false;
			}
			if (m_thread.joinable())
			{
				m_thread.join();
			}
			m_status = ThreadStatus::Busy;
			lock.unlock();
		}

		m_thread = std::thread(std::forward<TFunc>(func), std::forward<TArgs>(args)...);
		return true;
	}

	template<typename TFunc>
	bool Start(TFunc&& func) noexcept
	{
		{
			std::unique_lock<std::mutex> lock{ m_mutex };
			if (m_status == ThreadStatus::Busy)
			{
				return false;
			}
			if (m_thread.joinable())
			{
				m_thread.join();
			}
			m_status = ThreadStatus::Busy;
			lock.unlock();
		}

		m_thread = std::thread(std::forward<TFunc>(func));
		return true;
	}

	ThreadStatus GetThreadStatus() noexcept
	{
		std::unique_lock<std::mutex> lock{ m_mutex };
		return m_status;
	}

	~Thread() noexcept
	{
		std::unique_lock<std::mutex> lock{ m_mutex };
		m_status = ThreadStatus::Shutting;
		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}
private:

	ThreadStatus m_status;
	std::mutex m_mutex;
	std::thread m_thread;
};

template <typename T>
struct ITaskQueue
{
	virtual void Push(T& item) noexcept = 0;
	virtual void Push(const T& item) noexcept = 0;
	virtual void Pop() noexcept = 0;
	virtual T& Front() noexcept = 0;
	virtual size_t Size() noexcept = 0;
	virtual void Clear() noexcept = 0;
	virtual bool IsEmpty() noexcept = 0;
	virtual bool FrontAndPop(T& item) noexcept = 0;
};

template <typename T>
class TaskQueue final
	: public ITaskQueue<T>
{
public:
	TaskQueue() = default;
	~TaskQueue() = default;

	void Push(T& item) noexcept override
	{
		std::unique_lock<std::mutex> lock{ m_mutex };
		m_queue.push(item);
	}

	void Push(const T& item) noexcept override
	{
		std::unique_lock<std::mutex> lock{ m_mutex };
		m_queue.push(item);
	}

	void Pop() noexcept override
	{
		std::unique_lock<std::mutex> lock{ m_mutex };
		m_queue.pop();
	}

	T& Front() noexcept override
	{
		std::unique_lock<std::mutex> lock{ m_mutex };
		return m_queue.front();
	}

	bool FrontAndPop(T& item) noexcept override
	{
		std::unique_lock<std::mutex> lock{ m_mutex };
		if (m_queue.empty())
		{
			return false;
		}
		item = m_queue.front();
		m_queue.pop();
		return true;
	}

	void Clear() noexcept override
	{
		std::unique_lock<std::mutex> lock{ m_mutex };
		std::queue<T>().swap(m_queue);
	}


	size_t Size() noexcept override
	{
		std::unique_lock<std::mutex> lock{ m_mutex };
		return m_queue.size();
	}

	bool IsEmpty() noexcept override
	{
		return !Size();
	}

private:
	std::mutex m_mutex;
	std::queue<T> m_queue;
};

// Notice: ThreadPool itself is not threadsafe
class ThreadPool
{
public:
	using Task = std::function<void(void)>;

	ThreadPool()
		: ThreadPool(std::thread::hardware_concurrency())
	{
	}

	ThreadPool(uint32_t poolSize)
		: m_taskQueue{ std::make_unique<TaskQueue<std::shared_ptr<Task>>>() }
	{
		;
		for (uint32_t i = 0; i < poolSize; i++)
		{
			AddThread();
		}
	}

	template<typename TFunc, typename... TArgs>
	auto Post(std::promise<std::result_of_t<TFunc(TArgs...)>>& promise, TFunc&& func, TArgs&&... args)
		-> std::future<std::result_of_t<TFunc(TArgs...)>>
	{
		auto task = std::bind(std::forward<TFunc>(func), std::forward<TArgs>(args)...);
		auto taskWrapper = std::make_shared<Task>( [task, &promise]() noexcept
		{
			SetPromise(promise, task);
		});

		std::unique_lock<std::mutex> lock{ m_mutex };
		m_taskQueue->Push(taskWrapper);
		m_taskQueueChangeSignalBus.notify_one();

		return promise.get_future();
	}

	template<typename TFunc>
	auto Post(std::promise<std::result_of_t<TFunc()>>& promise, TFunc&& func)
		-> std::future<std::result_of_t<TFunc()>>
	{
		auto task = std::bind(std::forward<TFunc>(func));
		auto taskWrapper = std::make_shared<Task>( [task, &promise]() noexcept
		{
			SetPromise(promise, task);
		});

		std::unique_lock<std::mutex> lock{ m_mutex };
		m_taskQueue->Push(taskWrapper);
		m_taskQueueChangeSignalBus.notify_one();

		return promise.get_future();
	}

	void AddThread() noexcept
	{
		std::shared_ptr<std::atomic<ThreadCommand>> command{ std::make_shared<std::atomic<ThreadCommand>>(ThreadCommand::Run) };
		m_threadCommands.push_back(command);
		auto thread = std::make_unique<Thread>([this, command]() noexcept
		{
			for (;;)
			{
				// Exit loop
				if ((*command) != ThreadCommand::Run)
				{
					return;
				}

				std::shared_ptr<Task> spFunc;
				if (m_taskQueue->FrontAndPop(spFunc))
				{
					(*spFunc)();
				}

				std::unique_lock<std::mutex> lock(m_mutex);
				m_taskQueueChangeSignalBus.wait(lock, [this]() {
					return !m_taskQueue->IsEmpty() || m_isStop || m_isDone;
				});
			}
		});

		m_concurrentQueue.push_back(std::move(thread));
	}

	void Stop(bool isWaitComplete) noexcept
	{
		if (!isWaitComplete)
		{
			if (m_isStop)
			{
				return;
			}
			m_isStop = true;

			for (auto& command : m_threadCommands)
			{
				*command = ThreadCommand::Stop;
			}

		}
		else
		{
			if (m_isStop || m_isDone)
			{
				return;
			}
			m_isDone = true;

		}

		m_taskQueue->Clear();

		for (auto& command : m_threadCommands)
		{
			*command = ThreadCommand::Stop;
		}

		{
			std::unique_lock<std::mutex> lock{ m_mutex };
			m_taskQueueChangeSignalBus.notify_all();
		}

		for (auto& thread : m_concurrentQueue)
		{
			thread->Stop(isWaitComplete);
		}

		m_threadCommands.clear();
		m_concurrentQueue.clear();
	}

	~ThreadPool()
	{
		Stop(true);
	}

private:
	enum class ThreadCommand: uint32_t
	{
		Run,
		Stop,
	};

	template<typename TFunc, typename TResult>
	static void SetPromise(std::promise<TResult> & promise, TFunc && func) noexcept
	{
		promise.set_value(func());
	}

	template<typename TFunc>
	static void SetPromise(std::promise<void> & promise, TFunc && func) noexcept
	{
		func();
		promise.set_value();
	}

	std::atomic<bool> m_isDone;
	std::atomic<bool> m_isStop;
	std::mutex m_mutex;
	std::condition_variable m_taskQueueChangeSignalBus;
	std::vector<std::unique_ptr<Thread>> m_concurrentQueue;
	std::unique_ptr<ITaskQueue<std::shared_ptr<Task>>> m_taskQueue;
	std::vector<std::shared_ptr<std::atomic<ThreadCommand>>> m_threadCommands;
};

}}

#endif

