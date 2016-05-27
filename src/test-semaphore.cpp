
#include <iostream>

#include <libchain/async-semaphore.hpp>
#include <libchain/dsl-sequence.hpp>
#include <libchain/apply.hpp>
#include <libchain/run.hpp>

int main() {
	libchain::Semaphore semaphore(4);

	auto chainable = libchain::sequence()
	& libchain::makeUniqueAsyncLock(semaphore)
	& libchain::apply([] (auto lock) -> int {
		return 0;
	});
	
	run(chainable, [] (int) {
		std::cout << "Finally" << std::endl;
	});
}

