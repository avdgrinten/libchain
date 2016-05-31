
#ifndef LIBCHAIN_APPLY_HPP
#define LIBCHAIN_APPLY_HPP

namespace libchain {

template<typename T>
struct SwitchOnResult {
	static constexpr bool nullary = false;
	using Signature = void(T);
};

template<>
struct SwitchOnResult<void> {
	static constexpr bool nullary = true;
	using Signature = void();
};

template<typename Functor>
struct ApplyUnary {
private:
	template<typename P>
	struct Resolve;
	
	template<typename... Args>
	struct Resolve<void(Args...)> {
		using Result = std::result_of_t<Functor(Args...)>;

		static constexpr bool nullary = SwitchOnResult<Result>::nullary;
		using Signature = typename SwitchOnResult<Result>::Signature;
	};

public:
	template<typename P>
	using Signature = typename Resolve<P>::Signature;

private:
	template<typename... Args>
	using EnableNullary = std::enable_if_t<Resolve<void(Args...)>::nullary>;

	template<typename... Args>
	using EnableUnary = std::enable_if_t<!Resolve<void(Args...)>::nullary>;

public:
	template<typename P, typename Next, typename = void>
	struct Chain;
	
	template<typename... Args, typename Next>
	struct Chain<void(Args...), Next, EnableNullary<Args...>> {
		template<typename... E>
		Chain(const ApplyUnary &bp, E &&...emplace)
		: _functor(bp._functor), _next(std::forward<E>(emplace)...) { }

		void operator() (Args &&... args) {
			_functor(std::forward<Args>(args)...);
			_next();
		}

	private:
		Functor _functor;
		Next _next;
	};
	
	template<typename... Args, typename Next>
	struct Chain<void(Args...), Next, EnableUnary<Args...>> {
		template<typename... E>
		Chain(const ApplyUnary &bp, E &&...emplace)
		: _functor(bp._functor), _next(std::forward<E>(emplace)...) { }

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

template<typename Functor>
ApplyUnary<Functor> apply(Functor functor) {
	return ApplyUnary<Functor>(std::move(functor));
}

} // namespace libchain

#endif // LIBCHAIN_APPLY_HPP

