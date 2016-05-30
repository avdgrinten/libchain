
#ifndef LIBCHAIN_DSL_SEQUENCE_HPP
#define LIBCHAIN_DSL_SEQUENCE_HPP

#include <libchain/then.hpp>

namespace libchain {

template<typename Delegate>
struct Sequence {
	template<typename P>
	using Signature = typename Delegate::template Signature<P>;

	template<typename P, typename Next>
	struct Chain;

	template<typename... Args, typename Next>
	struct Chain<void(Args...), Next> {
		using DelegateChain = typename Delegate::template Chain<void(Args...), Next>;

		template<typename... E>
		Chain(const Sequence &bp, E &&... emplace)
		: _delegateChain(bp._delegate, std::forward<E>(emplace)...) { }

		void operator() (Args &&... args) {
			_delegateChain(std::forward<Args>(args)...);
		}

	private:
		DelegateChain _delegateChain;
	};

	Sequence(Delegate delegate)
	: _delegate(std::move(delegate)) { }

	template<typename Follow>
	auto operator& (Follow follow) {
		using Combined = Then<Sequence, Follow>;
		return Sequence<Combined>(then(*this, std::move(follow)));
	}

private:
	Delegate _delegate;
};

struct EmptySequence {
	template<typename First>
	auto operator& (First first) {
		return Sequence<First>(first);
	}
};

inline auto sequence() {
	return EmptySequence();
}

} // namespace libchain

#endif // LIBCHAIN_DSL_SEQUENCE_HPP

