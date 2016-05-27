
#ifndef LIBCHAIN_APPLY_HPP
#define LIBCHAIN_APPLY_HPP

namespace libchain {

template<typename Functor>
struct ApplyNullary {
	template<typename P>
	using Signature = void();

	template<typename P, typename Next>
	struct Chain;
	
	template<typename... Args, typename Next>
	struct Chain<void(Args...), Next> {
		Chain(const ApplyNullary &bp, Next &&next)
		: _functor(bp._functor), _next(std::move(next)) { }

		void operator() (Args &&... args) {
			_functor(std::forward<Args>(args)...);
			_next();
		}

	private:
		Functor _functor;
		Next _next;
	};

	ApplyNullary(Functor functor)
	: _functor(std::move(functor)) { }

private:
	Functor _functor;
};

template<typename Functor>
struct ApplyUnary {
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
		Chain(const ApplyUnary &bp, Next &&next)
		: _functor(bp._functor), _next(std::move(next)) { }

		void operator() (Args &&... args) {
			_next(_functor(std::forward<Args>(args)...));
		}

	private:
		Functor _functor;
		Next _next;
	};

	ApplyUnary(Functor functor)
	: _functor(std::move(functor)) { }

private:
	Functor _functor;
};

struct Nullary { };
struct Unary { };

constexpr Nullary nullary;
constexpr Unary unary;

template<typename Functor>
ApplyNullary<Functor> apply(Functor functor, Nullary tag) {
	return ApplyNullary<Functor>(std::move(functor));
}

template<typename Functor>
ApplyUnary<Functor> apply(Functor functor, Unary tag) {
	return ApplyUnary<Functor>(std::move(functor));
}

} // namespace libchain

#endif // LIBCHAIN_APPLY_HPP

