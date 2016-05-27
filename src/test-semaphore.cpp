
#include <iostream>

#include <libchain/async-semaphore.hpp>
#include <libchain/dsl-sequence.hpp>
#include <libchain/apply.hpp>
#include <libchain/run.hpp>

int main() {
	libchain::Semaphore semaphore(4);

	auto chainable = libchain::sequence()
	& libchain::makeUniqueAsyncLock(semaphore)
	& libchain::apply([] (auto lock) {
		std::cout << "Lock acquired" << std::endl;
	}, libchain::nullary);
	
	run(chainable, [] () {
		std::cout << "Finally" << std::endl;
	});
}

