
#ifndef LIBCHAIN_ASYNC_READY_QUEUE
#define LIBCHAIN_ASYNC_READY_QUEUE

#include <vector>
#include <mutex>

#include <libchain/await.hpp>
#include <libchain/dsl-sequence.hpp>

namespace libchain {

struct ReadyQueue {
public:
	ReadyQueue()
	: _isReady(false) { }

	auto wait() {
		return libchain::await<void()>([this] (auto callback) -> auto {
			std::unique_lock<std::mutex> lock(_mutex);
			if(_isReady) {
				lock.unlock();
				callback();
			}else{
				_waiters.push_back(callback);
			}
		});
	}
	
	void fire() {
		std::unique_lock<std::mutex> lock(_mutex);
		_isReady = true;
		auto callbacks = std::move(_waiters);
		lock.unlock();

		for(auto it = callbacks.begin(); it != callbacks.end(); ++it)
			(*it)();
	}

private:
	bool _isReady;
	std::vector<Callback<void()>> _waiters;
	std::mutex _mutex;
};

} // namespace libchain

#endif // LIBCHAIN_ASYNC_READY_QUEUE

