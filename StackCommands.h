#pragma once

#include <ostream>

#include "Stack.h"

namespace context_free {

template <typename C> struct StackCommand
{
	virtual void print(std::ostream&) const = 0;
	virtual void execute(Stack<C>& stack) = 0;

	virtual void undo(Stack<C>& stack) = 0;

	virtual ~StackCommand() {}
};

template <typename C> struct Pop : StackCommand<C>
{
	void print(std::ostream& out) const override { out << "Pop"; }

	void execute(Stack<C>& stack) override
	{
		if (stack.empty())
			throw std::runtime_error("Cannot call Pop() on an empty stack");

		valuePopped = stack.top();
		stack.pop();
	}

	void undo(Stack<C>& stack) override
	{
		if (!valuePopped.has_value())
			throw std::runtime_error("This Pop command has not been executed "
			                         "or has been already undone.");

		stack.push(*valuePopped);
		valuePopped = std::nullopt;
	}

	~Pop() {}

private:
	std::optional<C> valuePopped;
};

template <typename C> struct Sleep : StackCommand<C>
{
	void print(std::ostream& out) const override { out << "Sleep"; }

	void execute(Stack<C>&) override {}

	void undo(Stack<C>&) override {}

	~Sleep() {}
};

template <typename C> struct Push : StackCommand<C>
{
	const C what;
	void print(std::ostream& out) const override
	{
		out << "Push<" << what << ">";
	}

	void execute(Stack<C>& stack) override { stack.push(what); }

	void undo(Stack<C>& stack) override
	{
		if (!stack.top().has_value())
			throw std::runtime_error(
			    "Can't undo a 'Push' operation on an empty stack.");

		if (*stack.top() != what)
			throw std::runtime_error(
			    "The stack top is not how this Push command would have left "
			    "it. Refusing to undo.");

		stack.pop();
	}

	~Push() {}
};

template <typename C> struct Replace : StackCommand<C>
{
	const C with;
	void print(std::ostream& out) const override
	{
		out << "Replace<" << with << ">";
	}

	void execute(Stack<C>& stack) override
	{
		if (stack.empty())
			throw std::runtime_error(
			    "Tried to replace the top of an empty stack!");

		const C* inAlphabet = stack.alphabet->findChar(with);

		if (!inAlphabet)
			throw std::runtime_error("Got an error while executing "
			                         "Replace() command on the stack. "
			                         "Stack left unchanged.");

		valueReplaced = stack.top();

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
				stack.push(valueReplaced);
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

	void undo(Stack<C>& stack)
	{
		// @TODO Sane error checking
		throw std::runtime_error("undo() on a Replace operation unimplemented");

		if (!valueReplaced.has_value())
			throw std::runtime_error("This Replace command has not been "
			                         "executed or has been already undone.");

		if (!stack.top().has_value())
			throw std::runtime_error(
			    "Can't undo a 'Replace' operation on an empty stack.");

		if (*stack.top() != with)
			throw std::runtime_error(
			    "The stack top is not how this Replace command would have left "
			    "it. Refusing to undo.");

		stack.pop();
		stack.push(valueReplaced);
		valueReplaced = std::nullopt;
	}

	~Replace() {}

private:
	std::optional<C> valueReplaced;
};

} // namespace context_free
