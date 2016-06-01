
#include <iostream>

#include <libchain/async-semaphore.hpp>
#include <libchain/then.hpp>
#include <libchain/apply.hpp>
#include <libchain/run.hpp>

int main() {
	libchain::Semaphore semaphore(4);

	auto chainable
	= libchain::makeUniqueAsyncLock(semaphore)
	+ libchain::apply([] (auto lock) {
		std::cout << "Lock acquired" << std::endl;
	});
	
	run(chainable, [] () {
		std::cout << "Finally" << std::endl;
	});
}

