
#include <iostream>

#include <libchain/contextify.hpp>
#include <libchain/await.hpp>
#include <libchain/run.hpp>

int main() {
	auto chainable = libchain::contextify([] (int answer) {
		return libchain::await<void()>([answer] (libchain::Callback<void()> cb) {
			std::cout << "The answer is " << answer << std::endl;
			cb();
		});
	}, 42);

	run(chainable, [] () {
		std::cout << "Finally" << std::endl;
	});
}

