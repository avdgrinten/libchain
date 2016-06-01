
#ifndef LIBCHAIN_REPEAT_HPP
#define LIBCHAIN_REPEAT_HPP

#include <libchain/common.hpp>

namespace libchain {

template<typename Delegate>
struct Repeat {
public:
	template<typename P>
	using Signature = void();

	template<typename P, typename Next>
	struct Chain;

	template<typename... Args, typename Next>
	struct Chain<void(Args...), Next> {
	private:	
		struct Resume {
			Resume(Chain *chain)
			: _chain(chain) { }

			void operator() (bool again) {
				if(again) {
					_chain->_delegate();
				}else{
					_chain->_next();
				}
			}

		private:
			Chain *_chain;
		};

		using DelegateChain = typename Delegate::template Chain<void(), Resume>;

	public:
		template<typename... E>
		Chain(const Repeat &bp, E &&... emplace)
		: _delegate(bp._delegate, this),
				_next(std::forward<E>(emplace)...) { }
		
		Chain(Chain &&other) = delete;
		Chain &operator= (Chain &&other) = delete;

		void operator() () {
			_delegate();
		}

	private:
		DelegateChain _delegate;
		Next _next;
	};

	Repeat(Delegate delegate)
	: _delegate(std::move(delegate)) { }

private:
	Delegate _delegate;
};

template<typename Delegate>
struct CanSequence<Repeat<Delegate>>
: public std::true_type { };

template<typename Delegate>
Repeat<Delegate> repeat(Delegate delegate) {
	return Repeat<Delegate>(std::move(delegate));
}

} // namespace libchain

#endif // LIBCHAIN_REPEAT_HPP

