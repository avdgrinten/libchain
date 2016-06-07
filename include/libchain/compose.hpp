
#ifndef LIBCHAIN_COMPOSE_HPP
#define LIBCHAIN_COMPOSE_HPP

#include <assert.h>

#include <libchain/common.hpp>
#include <libchain/run.hpp>

namespace libchain {

template<typename Functor>
struct ComposeDynamic {
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
		Chain(const ComposeDynamic &bp, E &&... emplace)
		: _functor(bp._functor), _next(std::forward<E>(emplace)...) { }
		
		void operator() (Args &&... args) {
			auto chainable = _functor(std::forward<Args>(args)...);
			run(std::move(chainable), Resume<ComposedSignature>(this));
		}

	private:
		Functor _functor;
		Next _next;
	};

	ComposeDynamic(Functor functor)
	: _functor(std::move(functor)) { }

private:
	Functor _functor;
};

template<typename Functor>
struct CanSequence<ComposeDynamic<Functor>>
: public std::true_type { };

template<typename Functor>
struct ComposeOnce {
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
		using ComposedChain = typename ComposedChainable::template Chain<void(), Next>;
	
	public:
		template<typename... E>
		Chain(const ComposeOnce &bp, E &&... emplace)
		: _constructData(bp._functor, std::forward<E>(emplace)...),
				_hasConstructData(true), _hasComposedChain(false) { }
	
		~Chain() {
			if(_hasConstructData)
				_constructData.~ConstructData();
			if(_hasComposedChain)
				_composedChain.~ComposedChain();
		}

		void operator() (Args &&... args) {
			assert(_hasConstructData);

			// destruct the construction data to make room for the chainable
			Functor functor = std::move(_constructData.functor);
			Next next = std::move(_constructData.next);
			_hasConstructData = false;
			_constructData.~ConstructData();

			// construct the chain in-place and invoke it
			auto chainable = functor(std::forward<Args>(args)...);
			auto chain = new (&_composedChain) ComposedChain(chainable, std::move(next));
			_hasComposedChain = true;

			(*chain)();
		}

	private:
		struct ConstructData {
			template<typename... E>
			ConstructData(Functor functor, E &&... emplace)
			: functor(std::move(functor)), next(std::forward<E>(emplace)...) { }

			Functor functor;
			Next next;
		};

		union {
			ConstructData _constructData;
			ComposedChain _composedChain;
		};

		bool _hasConstructData, _hasComposedChain;
	};

	ComposeOnce(Functor functor)
	: _functor(std::move(functor)) { }

private:
	Functor _functor;
};

template<typename Functor>
struct CanSequence<ComposeOnce<Functor>>
: public std::true_type { };

struct Once { };

constexpr Once once;

template<typename Functor>
auto compose(Functor functor) {
	return ComposeDynamic<Functor>(std::move(functor));
}

template<typename Functor>
auto compose(Functor functor, Once tag) {
	return ComposeOnce<Functor>(std::move(functor));
}

} // namespace libchain

#endif // LIBCHAIN_COMPOSE_HPP

