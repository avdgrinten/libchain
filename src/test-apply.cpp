
#include <iostream>

#include <libchain/apply.hpp>
#include <libchain/await.hpp>
#include <libchain/dsl-sequence.hpp>
#include <libchain/run.hpp>

int main() {
	auto chainable = libchain::sequence()
	& libchain::await<void(int)>([] (auto cb) {
		cb(21);
	})
	& libchain::apply([] (int n) -> int {
		return n * 2;
	});
	
	run(chainable, [] (int result) {
		std::cout << "Finally the result is " << result << std::endl;
	});
}

