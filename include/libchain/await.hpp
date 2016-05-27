
#ifndef LIBCHAIN_AWAIT_HPP
#define LIBCHAIN_AWAIT_HPP

#include <utility>

namespace libchain {

template<typename S>
struct Callback;

template<typename... Results>
struct Callback<void(Results...)> {
	struct Interface {
		virtual void callback(Results... results) = 0;
	};

	Callback(Interface *implementation)
	: _implementation(implementation) { }

	void operator() (Results... results) {
		_implementation->callback(std::forward<Results>(results)...);
	}

private:
	Interface *_implementation;
};

template<typename S, typename Functor>
struct Await;

template<typename... Results, typename Functor>
struct Await<void(Results...), Functor> {
	template<typename P>
	using Signature = void(Results...);

	template<typename P, typename Next>
	struct Chain;
	
	template<typename... Args, typename Next>
	struct Chain<void(Args...), Next> {
		template<typename... E>
		Chain(const Await &bp, E &&... emplace)
		: _functor(bp._functor), _continuation(std::forward<E>(emplace)...) { }

		void operator() (Args &&... args) {
			_functor(Callback<void(Results...)>(&_continuation), std::forward<Args>(args)...);
		}

	private:
		struct Continuation : public Callback<void(Results...)>::Interface {
			template<typename... E>
			Continuation(E &&... emplace)
			: _next(std::forward<E>(emplace)...) { }
			
			void callback(Results &&... results) override {
				_next(std::forward<Results>(results)...);
			}
		
		private:
			Next _next;
		};

		Functor _functor;
		Continuation _continuation;
	};

	Await(Functor functor)
	: _functor(std::move(functor)) { }

private:
	Functor _functor;
};

template<typename S, typename Functor>
auto await(Functor functor) {
	return Await<S, Functor>(std::move(functor));
}

} // namespace libchain

#endif // LIBCHAIN_AWAIT_HPP

