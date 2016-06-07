
#ifndef LIBCHAIN_COMPOSE_HPP
#define LIBCHAIN_COMPOSE_HPP

#include <assert.h>

#include <libchain/common.hpp>
#include <libchain/run.hpp>

namespace libchain {

template<typename Functor>
struct Compose {
private:
	template<typename P>
	struct ResolveSignature;
	
	template<typename... Args>
	struct ResolveSignature<void(Args...)> {
		using Type = typename std::result_of_t<Functor(Args...)>::template Signature<void()>;
	};

public:
	template<typename P>
	using Signature = typename ResolveSignature<P>::Type;

	template<typename P, typename Next>
	struct Chain;

	template<typename... Args, typename Next>
	struct Chain<void(Args...), Next> {
	private:
		struct Resume {
			Resume(Chain *chain)
			: _chain(chain) { }

			template<typename... Results>
			void operator() (Results &&... results) {
				assert(_chain->_hasComposedChain);
				_chain->_composedChain.~ComposedChain();
				_chain->_hasComposedChain = false;

				_chain->_next(std::forward<Results>(results)...);
			}

		private:
			Chain *_chain;
		};

		using ComposedChainable = std::result_of_t<Functor(Args...)>;
		using ComposedChain = typename ComposedChainable::template Chain<void(), Resume>;
	
	public:
		template<typename... E>
		Chain(const Compose &bp, E &&... emplace)
		: _functor(bp._functor), _next(std::forward<E>(emplace)...),
				_hasComposedChain(false) { }
		
		template<typename... E>
		Chain(Compose &&bp, E &&... emplace)
		: _functor(std::move(bp._functor)), _next(std::forward<E>(emplace)...),
				_hasComposedChain(false) { }

		Chain(const Chain &other) = delete;

		~Chain() {
			assert(!_hasComposedChain);
		}

		Chain &operator= (Chain other) = delete;

		void operator() (Args &&... args) {
			assert(!_hasComposedChain);

			// construct the chain in-place and invoke it
			auto chainable = _functor(std::forward<Args>(args)...);
			new (&_composedChain) ComposedChain(std::move(chainable), this);
			_hasComposedChain = true;

			_composedChain();
		}

	private:
		Functor _functor;
		Next _next;

		union {
			ComposedChain _composedChain;
		};

		bool _hasComposedChain;
	};

	Compose(Functor functor)
	: _functor(std::move(functor)) { }

private:
	Functor _functor;
};

template<typename Functor>
struct CanSequence<Compose<Functor>>
: public std::true_type { };

template<typename Functor>
auto compose(Functor functor) {
	return Compose<Functor>(std::move(functor));
}

template<typename Functor>
struct Dynamic {
private:
	template<typename P>
	struct ResolveSignature;
	
	template<typename... Args>
	struct ResolveSignature<void(Args...)> {
		using Type = typename std::result_of_t<Functor(Args...)>::template Signature<void()>;
	};

public:
	template<typename P>
	using Signature = typename ResolveSignature<P>::Type;

	template<typename P, typename Next>
	struct Chain;

	template<typename... Args, typename Next>
	struct Chain<void(Args...), Next> {
	private:
		using ComposedChainable = std::result_of_t<Functor(Args...)>;
		using ComposedSignature = typename ComposedChainable::template Signature<void()>;
	
		template<typename S>
		struct Resume;
		
		template<typename... Results>
		struct Resume<void(Results...)> {
			Resume(Chain *chain)
			: _chain(chain) { }

			void operator() (Results &&... results) {
				_chain->_next(std::forward<Results>(results)...);
			}

		private:
			Chain *_chain;
		};

	public:
		template<typename... E>
		Chain(const Dynamic &bp, E &&... emplace)
		: _functor(bp._functor), _next(std::forward<E>(emplace)...) { }
		
		void operator() (Args &&... args) {
			auto chainable = _functor(std::forward<Args>(args)...);
			run(std::move(chainable), Resume<ComposedSignature>(this));
		}

	private:
		Functor _functor;
		Next _next;
	};

	Dynamic(Functor functor)
	: _functor(std::move(functor)) { }

private:
	Functor _functor;
};

template<typename Functor>
struct CanSequence<Dynamic<Functor>>
: public std::true_type { };

template<typename Functor>
auto dynamic(Functor functor) {
	return Dynamic<Functor>(std::move(functor));
}

} // namespace libchain

#endif // LIBCHAIN_COMPOSE_HPP

