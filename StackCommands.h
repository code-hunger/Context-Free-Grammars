#pragma once

#include <ostream>

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
	const C with;

	Replace(C const& with) : with(with) {}

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

		CPtrBox inAlphabet = stack.alphabet->findChar(with);

		if (!inAlphabet)
			throw std::runtime_error("Got an error while executing "
			                         "Replace() command on the stack. "
			                         "Stack left unchanged.");

		auto valueReplaced = stack.top();

		try {
			stack.pop();
		} catch (...) {
			std::throw_with_nested(std::runtime_error(
			    "pop() failed during execution of a Replace() operation. No "
			    "push will be executed."));
		}

		try {
			stack.push(with);
		} catch (...) {
			try {
				stack.push(*valueReplaced);
			} catch (...) {
				std::throw_with_nested(std::runtime_error(
				    "push() operation during Replace execution failed. "
				    "Restoring previous value failed, too :("));
			}
			std::throw_with_nested(
			    std::runtime_error("push() operation during Replace execution "
			                       "failed. Previous value restored."));
		}
	}

	~Replace() {}
};

} // namespace context_free
