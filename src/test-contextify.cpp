
#include <iostream>

#include <libchain/contextify.hpp>
#include <libchain/await.hpp>
#include <libchain/run.hpp>

int main() {
	auto chainable = libchain::contextify([] (int *answer, int *half) {
		return libchain::await<void()>([answer, half] (libchain::Callback<void()> cb) {
			std::cout << "The answer is " << *answer
					<< "; half of that is " << *half << std::endl;
			cb();
		});
	}, 42, 21);

	run(chainable, [] () {
		std::cout << "Finally" << std::endl;
	});
}

