
#include <iostream>

#include <libchain/compose.hpp>
#include <libchain/await.hpp>
#include <libchain/run.hpp>

int main() {
	auto chainable = libchain::compose([] () {
		return libchain::await<void()>([] (libchain::Callback<void()> cb) {
			std::cout << "In composed chain" << std::endl;
			cb();
		});
	}, libchain::dynamic);

	run(chainable, [] () {
		std::cout << "Finally" << std::endl;
	});
	
	auto chainable2 = libchain::compose([] () {
		return libchain::await<void()>([] (libchain::Callback<void()> cb) {
			std::cout << "In composed chain" << std::endl;
			cb();
		});
	}, libchain::once);

	run(chainable2, [] () {
		std::cout << "Finally" << std::endl;
	});
}

