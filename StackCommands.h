#pragma once

#include <ostream>

#include "AlphaString.h"
#include "Stack.h"

namespace context_free {

template <typename C, typename CPtrBox> struct StackCommand
{
	virtual void print(std::ostream&) const = 0;
	virtual void execute(Stack<C, CPtrBox>& stack) const = 0;

	virtual ~StackCommand() {}
};

template <typename C, typename CPtrBox> struct Pop : StackCommand<C, CPtrBox>
{
	Pop() = default;

	void print(std::ostream& out) const override { out << "Pop"; }

	void execute(Stack<C, CPtrBox>& stack) const override
	{
		if (stack.empty())
			throw std::runtime_error("Cannot call Pop() on an empty stack");

		stack.pop();
	}

	~Pop() {}
};

template <typename C, typename CPtrBox> struct Sleep : StackCommand<C, CPtrBox>
{
	void print(std::ostream& out) const override { out << "Sleep"; }

	void execute(Stack<C, CPtrBox>&) const override {}

	~Sleep() {}
};

template <typename C, typename CPtrBox> struct Push : StackCommand<C, CPtrBox>
{
	const C what;

	Push(C const& what) : what(what) {}

	void print(std::ostream& out) const override
	{
		out << "Push<";
		what.print(out);
		out << ">";
	}

	void execute(Stack<C, CPtrBox>& stack) const override { stack.push(what); }

	~Push() {}
};

template <typename C, typename CPtrBox>
struct Replace : StackCommand<C, CPtrBox>
{
	const AlphaString<C, CPtrBox>& with;

	Replace(AlphaString<C, CPtrBox> const& with) : with(with) {}

	void print(std::ostream& out) const override
	{
		out << "Replace<";
		with.print(out);
		out << ">";
	}

	void execute(Stack<C, CPtrBox>& stack) const override
	{
		if (stack.empty())
			throw std::runtime_error(
			    "Tried to replace the top of an empty stack!");

		try {
			stack.pop();
		} catch (...) {
			std::throw_with_nested(std::runtime_error(
			    "pop() failed during execution of a Replace() operation. No "
			    "push will be executed."));
		}

		with.for_each([&stack](CPtrBox c) { stack.push(*c); });
	}

	~Replace() {}
};

} // namespace context_free
