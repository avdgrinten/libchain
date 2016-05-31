
#include <iostream>

#include <libchain/apply.hpp>
#include <libchain/await.hpp>
#include <libchain/dsl-sequence.hpp>
#include <libchain/run.hpp>

int main() {
	auto chainable = libchain::sequence()
	& libchain::apply([] () -> int {
		return 42;
	})
	& libchain::apply([] (int n) {
		std::cout << "The answer is " << n << std::endl;
	});
	
	run(chainable, [] () {
		std::cout << "Finally" << std::endl;
	});
}

