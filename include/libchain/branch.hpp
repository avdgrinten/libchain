
#ifndef LIBCHAIN_BRANCH_HPP
#define LIBCHAIN_BRANCH_HPP

namespace libchain {

template<typename ThenChainable, typename ElseChainable>
struct Branch {
public:
	template<typename P>
	using Signature = typename ThenChainable::template Signature<P>;

	template<typename P, typename Next>
	struct Chain;

	template<typename... Args, typename Next>
	struct Chain<void(Args...), Next> {
	private:	
		template<typename S>
		struct Resume;
		
		template<typename... Results>
		struct Resume<void(Results...)> {
			Resume(Chain *chain)
			: _chain(chain) { }

			void operator() (Results... results) {
				_chain->_next(std::forward<Results>(results)...);
			}

		private:
			Chain *_chain;
		};

		using ThenSignature = typename ThenChainable::template Signature<void()>;
		using ThenChain = typename ThenChainable::template Chain<void(), Resume<ThenSignature>>;
		using ElseSignature = typename ElseChainable::template Signature<void()>;
		using ElseChain = typename ElseChainable::template Chain<void(), Resume<ElseSignature>>;

	public:
		Chain(const Branch &bp, Next &&next)
		: _thenChain(bp._thenChainable, Resume<ThenSignature>(this)),
				_elseChain(bp._elseChainable, Resume<ElseSignature>(this)),
				_next(std::move(next)) { }
		
		void operator() (bool check) {
			if(check) {
				_thenChain();
			}else{
				_elseChain();
			}
		}

	private:
		ThenChain _thenChain;
		ElseChain _elseChain;
		Next _next;
	};

	Branch(ThenChainable then_chainable, ElseChainable else_chainable)
	: _thenChainable(std::move(then_chainable)),
			_elseChainable(std::move(else_chainable)) { }

private:
	ThenChainable _thenChainable;
	ElseChainable _elseChainable;
};

template<typename ThenChainable, typename ElseChainable>
Branch<ThenChainable, ElseChainable>
branch(ThenChainable then_chainable, ElseChainable else_chainable) {
	return Branch<ThenChainable, ElseChainable>(std::move(then_chainable),
			std::move(else_chainable));
}

} // namespace libchain

#endif // LIBCHAIN_BRANCH_HPP

