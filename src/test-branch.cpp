
#include <iostream>

#include <libchain/apply.hpp>
#include <libchain/branch.hpp>
#include <libchain/dsl-sequence.hpp>
#include <libchain/run.hpp>

int main() {
	auto chainable = libchain::sequence()
	& libchain::apply([] () -> bool {
		return false;
	})
	& libchain::branch(
		libchain::apply([] () {
			std::cout << "The condition was true" << std::endl;
		}),
		
		libchain::apply([] () {
			std::cout << "The condition was false" << std::endl;
		})
	);
	
	run(chainable, [] () {
		std::cout << "Finally" << std::endl;
	});
}

