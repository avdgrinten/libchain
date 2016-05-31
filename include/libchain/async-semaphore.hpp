
#ifndef LIBCHAIN_ASYNC_SEMAPHORE
#define LIBCHAIN_ASYNC_SEMAPHORE

#include <assert.h>

#include <queue>
#include <mutex>

#include <libchain/apply.hpp>
#include <libchain/await.hpp>
#include <libchain/dsl-sequence.hpp>

namespace libchain {

template<typename R>
struct UniqueAsyncLock {
	friend void swap(UniqueAsyncLock &a, UniqueAsyncLock &b) {
		std::swap(a._resource, b._resource);
		std::swap(a._locked, b._locked);
	}

	UniqueAsyncLock()
	: _resource(nullptr), _locked(false) { }

	UniqueAsyncLock(R &resource, std::defer_lock_t tag)
	: _resource(&resource), _locked(false) { }

	UniqueAsyncLock(R &resource, std::adopt_lock_t tag)
	: _resource(&resource), _locked(true) { }
	
	UniqueAsyncLock(const UniqueAsyncLock &other) = delete;

	UniqueAsyncLock(UniqueAsyncLock &&other)
	: UniqueAsyncLock() {
		swap(*this, other);
	}

	~UniqueAsyncLock() {
		if(_locked) {
			assert(_resource);
			_resource->unlock();
		}
	}

	UniqueAsyncLock &operator= (UniqueAsyncLock other) {
		swap(*this, other);
		return *this;
	}

private:
	R *_resource;
	bool _locked;
};

template<typename R>
auto makeUniqueAsyncLock(R &resource) {
	return sequence()
	+ resource.lockAsync()
	+ apply([&resource] () -> auto {
		return UniqueAsyncLock<R>(resource, std::adopt_lock);
	});
}

struct Semaphore {
	Semaphore(int count)
	: _counter(count) { }

	auto lockAsync() {
		return await<void()>([this] (Callback<void()> callback) {
			std::unique_lock<std::mutex> lock(_mutex);
			if(_counter > 0) {
				_counter--;
				lock.unlock();
				callback();
			}else{
				_queue.push(callback);
			}
		});
	}

	void unlock() {
		std::unique_lock<std::mutex> lock(_mutex);
		if(_queue.empty()) {
			_counter++;
		}else{
			Callback<void()> callback = _queue.front();
			_queue.pop();
			lock.unlock();
			callback();
		}
	}

private:
	int _counter;
	std::queue<Callback<void()>> _queue;
	std::mutex _mutex;
};

} // namespace libchain

#endif // LIBCHAIN_ASYNC_SEMAPHORE

