
#ifndef LIBCHAIN_CLOSURE_HPP
#define LIBCHAIN_CLOSURE_HPP

#include <utility>

namespace libchain {

template<typename Chainable, typename Finally>
struct Closure {
	template<typename S>
	struct Complete;

	template<typename... Results>
	struct Complete<void(Results...)> {
		Complete(Closure *closure)
		: _closure(closure) { }

		void operator() (Results &&... results) {
			_closure->finally(std::forward<Results>(results)...);
			delete _closure;
		}

	private:
		Closure *_closure;
	};

	using Signature = typename Chainable::template Signature<void()>;
	using Continuation = typename Chainable::template Chain<void(), Complete<Signature>>;

	Closure(Chainable chainable, Finally finally)
	: continuation(std::move(chainable), this), finally(std::move(finally)) { }

	Continuation continuation;
	Finally finally;
};

template<typename Chainable, typename Finally>
void run(Chainable chainable, Finally finally) {
	auto closure = new Closure<Chainable, Finally>(std::move(chainable), std::move(finally));
	closure->continuation();
}

template<typename Chainable>
void run(Chainable chainable) {
	run(std::move(chainable), [] () { });
}

} // namespace libchain

#endif // LIBCHAIN_CLOSURE_HPP

