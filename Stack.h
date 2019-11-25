#include <memory>
#include <optional>
#include <ostream>
#include <stack>
#include <stdexcept>
#include <variant>

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

template <typename C> struct StackCommand
{
	struct Pop
	{
		void print(std::ostream& out) const { out << "Pop"; }
	};
	struct Sleep
	{
		void print(std::ostream& out) const { out << "Sleep"; }
	};
	struct Push
	{
		C what;
		void print(std::ostream& out) const { out << "Push<" << what << ">"; }
	};
	struct Replace
	{
		C with;
		void print(std::ostream& out) const
		{
			out << "Replace<" << with << ">";
		}
	};

	const std::variant<Push, Pop, Replace, Sleep> value;

	void fire(Stack<C>& stack) const
	{
		// Magic!
		std::visit(
		    [this, &stack](auto const& arg) mutable { execute(arg, stack); },
		    value);
	}

	void execute(Pop const&, Stack<C>& stack) const
	{
		if (stack.empty())
			throw std::runtime_error("Cannot call Pop() on an empty stack");
		stack.pop();
	}

	void execute(Sleep const&, Stack<C> const&) const {}

	void execute(Push const& arg, Stack<C>& stack) const
	{
		stack.push(arg.what);
	}

	void execute(Replace const& arg, Stack<C>& stack) const
	{
		if (stack.empty())
			throw std::runtime_error(
			    "Tried to replace the top of an empty stack!");

		const C* inAlphabet = stack.alphabet->findChar(arg.with);

		if (!inAlphabet)
			throw std::runtime_error(
			    "Got an error while executing Replace() command on the stack. "
			    "Restored to previous state");

		stack.pop();
		stack.push(*inAlphabet);
	}

	auto invert(Stack<C>& stack) const
	{
		// Magic!
		return std::visit(
		    [this, &stack](auto const& arg) { return invert(arg, stack); },
		    value);
	}

	StackCommand invert(Sleep const&, Stack<C> const&) const
	{
		return {Sleep{}};
	}
	StackCommand invert(Push const&, Stack<C> const&) const { return {Pop{}}; }
	StackCommand invert(Replace const&, Stack<C> const& stack) const
	{
		if (stack.empty()) return {Sleep{}};

		return {Replace{*stack.top()}};
	}
	StackCommand invert(Pop const&, const Stack<C>& stack) const
	{
		if (stack.empty())
			throw std::runtime_error("Cannot call Pop() on an empty stack");
		return {Push{*stack.top()}};
	}
};

template <typename C>
void printCommand(std::ostream& out, StackCommand<C> const& command)
{
	std::visit([&out](const auto& x) { x.print(out); }, command.value);
}

} // namespace context_free
