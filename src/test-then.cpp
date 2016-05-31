
#include <iostream>

#include <libchain/await.hpp>
#include <libchain/dsl-sequence.hpp>
#include <libchain/run.hpp>

int main() {
	auto chainable = libchain::sequence()
	+ libchain::await<void()>([] (libchain::Callback<void()> cb) {
		std::cout << "First" << std::endl;
		cb();
	})
	+ libchain::await<void()>([] (libchain::Callback<void()> cb) {
		std::cout << "Follow" << std::endl;
		cb();
	});

	run(chainable, [] () {
		std::cout << "Finally" << std::endl;
	});
}
