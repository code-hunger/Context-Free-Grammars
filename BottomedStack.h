#pragma once

#include <optional>
#include <stdexcept>

#include "Stack.h"

namespace context_free {

template <typename C> struct BottomedStack : Stack<C>
{
private:
	bool hasBottom = true;
	using parent = Stack<C>;

public:
	const std::optional<C> bottom;

	BottomedStack(decltype(parent::alphabet) alphabet, std::optional<C> bottom)
	    : parent{alphabet}, bottom{bottom}
	{
	}

	std::optional<C> top() const
	{
		return parent::empty() ? bottom : parent::top();
	}

	bool empty() const { return parent::empty() && !hasBottom; }

	bool reachedBottom() const { return parent::empty() && hasBottom; }

	void push(C const& c)
	{
		if (bottom.has_value() && *bottom == c)
			throw std::runtime_error(
			    "Refusing to push the stack bottom on top. The stack bottom "
			    "char is reserved for checking if a word has been read.");

		if (!hasBottom) {
			throw std::runtime_error(
			    "Refusing to push() to an empty BottomedStack. This stack's "
			    "bottom was already pop()ped and the stack is now exhausted. "
			    "No further push()es allowed.");
		}

		parent::push(c);
	}

	void pop()
	{
		if (!parent::empty())
			parent::pop();
		else if (hasBottom)
			hasBottom = false;
		else
			throw std::runtime_error("Cannot call Pop() on an empty stack");
	}
};

} // namespace context_free
