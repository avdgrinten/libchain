
#include <iostream>

#include <libchain/await.hpp>
#include <libchain/run.hpp>

int main() {
	auto chainable = libchain::await<void()>([] (libchain::Callback<void()> cb) {
		cb();
	});
	
	run(chainable, [] () {
		std::cout << "Finally" << std::endl;
	});
}

