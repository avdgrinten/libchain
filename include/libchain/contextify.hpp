
#ifndef LIBCHAIN_CONTEXTIFY_HPP
#define LIBCHAIN_CONTEXTIFY_HPP

namespace libchain {

template<typename Functor, typename T>
struct Contextify {
private:
	template<typename P>
	struct ResolveSignature;
	
	template<typename... Args>
	struct ResolveSignature<void(Args...)> {
		using Type = typename std::result_of_t<Functor(T &)>::template Signature<void(Args...)>;
	};

public:
	template<typename P>
	using Signature = typename ResolveSignature<P>::Type;

	template<typename P, typename Next>
	struct Chain;

	template<typename... Args, typename Next>
	struct Chain<void(Args...), Next> {
		using Chainable = std::result_of_t<Functor(T &)>;
		using Continuation = typename Chainable::template Chain<void(Args...), Next>;

		Chain(const Contextify &bp, Next &&next)
		: _context(bp._context), _continuation(bp._functor(_context), std::move(next)) { }
		
		void operator() (Args... args) {
			_continuation(std::forward<Args>(args)...);
		}

	private:
		T _context;
		Continuation _continuation;
	};

	Contextify(Functor functor, T context)
	: _functor(functor), _context(context) { }

private:
	Functor _functor;
	T _context;
};

template<typename Functor, typename T>
auto contextify(Functor functor, T context) {
	return Contextify<Functor, T>(std::move(functor), std::move(context));
}

} // namespace libchain

#endif // LIBCHAIN_CONTEXTIFY_HPP

