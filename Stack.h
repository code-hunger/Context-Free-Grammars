#pragma once

#include <memory>
#include <optional>
#include <stack>
#include <stdexcept>

#include "Alphabet.h"

namespace context_free {

template <typename C, typename CPtrBox> struct Stack
{
private:
	std::stack<CPtrBox> stack{};

protected:
	/* A Stack should be copied using clone() to ensure derived classes are
	 * properly handled because implicit calls of the copy ctor may lead to
	 * confusion (as it happened already to me) */
	Stack(Stack const&) = default;

public:
	const std::shared_ptr<AlphabetLike<C, CPtrBox>> alphabet;

	Stack(decltype(alphabet) alphabet) : alphabet{alphabet} {}

	Stack(Stack &&) = default;

	virtual unique_ptr<Stack> clone() const
	{
		return std::make_unique<Stack>(Stack{*this});
	}

	virtual std::optional<C> top() const
	{
		if (stack.empty()) return {};

		return *stack.top();
	}

	virtual bool empty() const { return stack.empty(); }

	virtual void push(C const& c)
	{
		CPtrBox inAlphabet = alphabet->findChar(c);
		if (!inAlphabet)
			throw std::runtime_error("Tried to add an element to the stack "
			                         "which is not in the specified alphabet!");

		stack.push(inAlphabet);
	}

	virtual void pop()
	{
		if (stack.empty())
			throw std::runtime_error("Cannot call Pop() on an empty stack");
		stack.pop();
	}

	virtual ~Stack() {}
};

} // namespace context_free
