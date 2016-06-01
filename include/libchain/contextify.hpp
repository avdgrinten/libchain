
#ifndef LIBCHAIN_CONTEXTIFY_HPP
#define LIBCHAIN_CONTEXTIFY_HPP

#include <utility>
#include <tuple>

#include <libchain/common.hpp>

namespace libchain {

template<typename Functor, typename... T>
struct Contextify {
private:
	template<typename P>
	struct ResolveSignature;
	
	template<typename... Args>
	struct ResolveSignature<void(Args...)> {
		using Type = typename std::result_of_t<Functor(T *...)>::template Signature<void(Args...)>;
	};

public:
	template<typename P>
	using Signature = typename ResolveSignature<P>::Type;

	template<typename P, typename Next>
	struct Chain;

	template<typename... Args, typename Next>
	struct Chain<void(Args...), Next> {
		using ContextChainable = std::result_of_t<Functor(T *...)>;
		using ContextChain = typename ContextChainable::template Chain<void(Args...), Next>;

		template<typename... E>
		Chain(const Contextify &bp, E &&... emplace)
		: Chain(std::index_sequence_for<T...>(), bp, std::forward<E>(emplace)...) { }

		void operator() (Args... args) {
			_contextChain(std::forward<Args>(args)...);
		}

	private:
		template<size_t... I, typename... E>
		Chain(std::index_sequence<I...>, const Contextify &bp, E &&... emplace)
		: _context(bp._context), _contextChain(bp._functor(&std::get<I>(_context)...),
				std::forward<E>(emplace)...) { }

		std::tuple<T...> _context;
		ContextChain _contextChain;
	};

	Contextify(Functor functor, T... contexts)
	: _functor(functor), _context(std::move(contexts)...) { }

private:
	Functor _functor;
	std::tuple<T...> _context;
};

template<typename Functor, typename... T>
struct CanSequence<Contextify<Functor, T...>>
: public std::true_type { };

template<typename Functor, typename... T>
auto contextify(Functor functor, T... contexts) {
	return Contextify<Functor, T...>(std::move(functor), std::move(contexts)...);
}

} // namespace libchain

#endif // LIBCHAIN_CONTEXTIFY_HPP

