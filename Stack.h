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

	using Command = const std::variant<Push, Pop, Replace, Sleep>;

	void fire(Command& command)
	{
		// Magic!
		std::visit([this](auto const& arg) mutable { execute(arg); }, command);
	}

	Command invertCommand(Command& command)
	{
		// Magic!
		return std::visit([this](auto const& arg) { return invert(arg); },
		                  command);
	}

	std::optional<C> top() const
	{
		if (stack.empty()) return {};

		return stack.top();
	}

	bool empty() const { return stack.empty(); }

	const std::shared_ptr<AlphabetLike<C>> alphabet;

	Stack(decltype(alphabet) alphabet, std::optional<C> bottom)
	    : alphabet(alphabet)
	{
		if (bottom) push(*bottom);
	}

	void push(C c)
	{
		if (!alphabet->findChar(c))
			throw std::runtime_error("Tried to add an element to the stack "
			                         "which is not in the specified alphabet!");

		stack.push(c);
	}

private:
	std::stack<C> stack{};

	void execute(Pop const&)
	{
		if (stack.empty())
			throw std::runtime_error("Cannot call Pop() on an empty stack");
		stack.pop();
	}

	void execute(Sleep const&) {}

	void execute(Push const& arg) { push(arg.what); }

	void execute(Replace const& arg)
	{
		if (!alphabet->findChar(arg.with))
			throw std::runtime_error("Tried to add an element to the stack "
			                         "which is not in the specified alphabet!");
		if (!stack.empty()) {
			stack.pop();
		}
		stack.push(arg.with);
	}

	Command invert(Sleep const&) const { return Sleep{}; }
	Command invert(Push const&) const { return Pop{}; }
	Command invert(Replace const&) const
	{
		if (stack.empty()) {
			return Pop{};
		}
		return Replace{stack.top()};
	}
	Command invert(Pop const&) const
	{
		if (stack.empty())
			throw std::runtime_error("Cannot call Pop() on an empty stack");
		return Push{stack.top()};
	}
};

template <typename C>
void printCommand(std::ostream& out, typename Stack<C>::Command const& command)
{
	std::visit([&out](const auto& x) { x.print(out); }, command);
}

} // namespace context_free