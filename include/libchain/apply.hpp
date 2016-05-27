
#ifndef LIBCHAIN_APPLY_HPP
#define LIBCHAIN_APPLY_HPP

namespace libchain {

template<typename Functor>
struct Apply {
private:
	template<typename P>
	struct ResolveSignature;
	
	template<typename... Args>
	struct ResolveSignature<void(Args...)> {
		using Type = void(std::result_of_t<Functor(Args...)>);
	};

public:
	template<typename P>
	using Signature = typename ResolveSignature<P>::Type;

	template<typename P, typename Next>
	struct Chain;
	
	template<typename... Args, typename Next>
	struct Chain<void(Args...), Next> {
		Chain(const Apply &bp, Next &&next)
		: _functor(bp._functor), _next(std::move(next)) { }

		void operator() (Args &&... args) {
			_next(_functor(std::forward<Args>(args)...));
		}

	private:
		Functor _functor;
		Next _next;
	};

	Apply(Functor functor)
	: _functor(std::move(functor)) { }

private:
	Functor _functor;
};

template<typename Functor>
Apply<Functor> apply(Functor functor) {
	return Apply<Functor>(std::move(functor));
}

} // namespace libchain

#endif // LIBCHAIN_APPLY_HPP

