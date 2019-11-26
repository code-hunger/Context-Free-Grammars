#pragma once

#include <memory>
#include <optional>
#include <stack>
#include <stdexcept>

#include "Alphabet.h"

namespace context_free {

template <typename C> struct Stack
{
	// template <typename _> friend struct StackCommand;

	std::optional<C> top() const
	{
		if (stack.empty()) return {};

		return *stack.top();
	}

	bool empty() const { return stack.empty(); }

	const std::shared_ptr<AlphabetLike<C>> alphabet;

	Stack(decltype(alphabet) alphabet, std::optional<C> bottom)
	    : alphabet(alphabet)
	{
		if (bottom) push(*bottom);
	}

	void push(C const& c)
	{
		const C* inAlphabet = alphabet->findChar(c);
		if (!inAlphabet)
			throw std::runtime_error("Tried to add an element to the stack "
			                         "which is not in the specified alphabet!");

		stack.push(inAlphabet);
	}

	void pop()
	{
		if (stack.empty())
			throw std::runtime_error("Cannot call Pop() on an empty stack");
		stack.pop();
	}

private:
	std::stack<const C*> stack{};
};

} // namespace context_free
