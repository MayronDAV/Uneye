#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <functional>
#include <future>

namespace Uneye
{
	class ThreadPool 
	{
		public:
			ThreadPool(size_t threads);
			~ThreadPool();

			template<class F, class... Args>
			auto enqueue(F&& f, Args&&... args)
				-> std::future<typename std::result_of<F(Args...)>::type>;

			void stopThreads(); // Função para parar as threads

		private:
			// workers
			std::vector<std::thread> workers;
			// task queue
			std::queue<std::function<void()>> tasks;

			// synchronization
			std::mutex queue_mutex;
			std::condition_variable condition;
			bool stop;
	};

	inline ThreadPool::ThreadPool(size_t threads)
		: stop(false)
	{
		for (size_t i = 0; i < threads; ++i)
			workers.emplace_back(
				[this]
				{
					for (;;)
					{
						std::function<void()> task;

						{
							std::unique_lock<std::mutex> lock(this->queue_mutex);
							this->condition.wait(lock,
								[this] { return this->stop || !this->tasks.empty(); });
							if (this->stop && this->tasks.empty())
								return;
							if (!this->tasks.empty()) {
								task = std::move(this->tasks.front());
								this->tasks.pop();
							}
						}

						if (task)
							task();
					}
				}
			);
	}

	inline ThreadPool::~ThreadPool()
	{
		stopThreads();
	}

	inline void ThreadPool::stopThreads()
	{
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			stop = true;
		}
		condition.notify_all();
		for (std::thread& worker : workers)
			if (worker.joinable())
				worker.join();
	}

	template<class F, class... Args>
	auto ThreadPool::enqueue(F&& f, Args&&... args)
		-> std::future<typename std::result_of<F(Args...)>::type>
	{
		using return_type = typename std::result_of<F(Args...)>::type;

		auto task = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

		std::future<return_type> res = task->get_future();
		{
			std::unique_lock<std::mutex> lock(queue_mutex);

			// don't allow enqueueing after stopping the pool
			if (stop)
				throw std::runtime_error("enqueue on stopped ThreadPool");

			tasks.emplace([task]() { (*task)(); });
		}
		condition.notify_one();
		return res;
	}

}
