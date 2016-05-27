
#ifndef LIBCHAIN_THEN_HPP
#define LIBCHAIN_THEN_HPP

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

		Chain(const Then &bp, Next &&next)
		: _firstChain(bp._first, FollowChain(bp._follow, std::move(next))) { }

		void operator() (Args... args) {
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
auto then(First first, Follow follow) {
	return Then<First, Follow>(std::move(first), std::move(follow));
}

} // namespace libchain

#endif // LIBCHAIN_THEN_HPP

