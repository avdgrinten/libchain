
#ifndef LIBCHAIN_THEN_HPP
#define LIBCHAIN_THEN_HPP

#include <libchain/common.hpp>

namespace libchain {

template<typename First, typename Follow>
struct Then {
private:
	template<typename P>
	using FirstSignature = typename First::template Signature<P>;

public:
	template<typename P>
	using Signature = typename Follow::template Signature<FirstSignature<P>>;

	template<typename P, typename Next>
	struct Chain;

	template<typename... Args, typename Next>
	struct Chain<void(Args...), Next> {
		using FollowChain = typename Follow::template Chain<FirstSignature<void(Args...)>, Next>;
		using FirstChain = typename First::template Chain<void(Args...), FollowChain>;

		template<typename... E>
		Chain(const Then &bp, E &&... emplace)
		: _firstChain(bp._first, bp._follow, std::forward<E>(emplace)...) { }

		void operator() (Args &&... args) {
			_firstChain(std::forward<Args>(args)...);
		}

	private:
		FirstChain _firstChain;
	};

	Then(First first, Follow follow)
	: _first(std::move(first)), _follow(std::move(follow)) { }

private:
	First _first;
	Follow _follow;
};

template<typename First, typename Follow>
struct CanSequence<Then<First, Follow>>
: public std::true_type { };

template<typename T>
using EnableSequence = std::enable_if_t<CanSequence<T>::value>;

template<typename First, typename Follow,
		typename = EnableSequence<First>>
Then<First, Follow> operator+ (First first, Follow follow) {
	return Then<First, Follow>(std::move(first), std::move(follow));
}

template<typename First, typename Follow>
Then<First, Follow> then(First first, Follow follow) {
	return Then<First, Follow>(std::move(first), std::move(follow));
}

} // namespace libchain

#endif // LIBCHAIN_THEN_HPP

