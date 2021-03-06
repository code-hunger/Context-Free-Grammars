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

public:
	const std::shared_ptr<AlphabetLike<C, CPtrBox>> alphabet;

	Stack(decltype(alphabet) alphabet) : alphabet{alphabet} {}

	std::optional<CPtrBox> top() const
	{
		if (stack.empty()) return {};

		return stack.top();
	}

	bool empty() const { return stack.empty(); }

	void push(C const& c)
	{
		CPtrBox inAlphabet = alphabet->findChar(c);
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
