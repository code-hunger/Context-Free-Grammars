#pragma once

#include <memory>
#include <optional>
#include <stack>
#include <stdexcept>

#include "Alphabet.h"

namespace context_free {

template <typename C> struct Stack
{
private:
	std::stack<const C*> stack{};

public:
	const std::shared_ptr<AlphabetLike<C>> alphabet;

	const std::optional<C> bottom = std::nullopt;

	Stack(decltype(alphabet) alphabet, std::optional<C> bottom)
	    : alphabet{alphabet}, bottom{bottom}
	{
		if (bottom) push(*bottom);
	}

	std::optional<C> top() const
	{
		if (stack.empty()) return {};

		return *stack.top();
	}

	bool empty() const { return stack.empty(); }

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
};

} // namespace context_free
