#include <ostream>
#include <stack>
#include <stdexcept>
#include <variant>

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
		void print(std::ostream& out) const { out << "Replace<" << with << ">"; }
	};

	using Command = const std::variant<Push, Pop, Replace, Sleep>;

	void fire(Command& command)
	{
		// Magic!
		std::visit([*this](auto const& arg) { execute(arg); }, command);
	}

private:
	std::stack<C> stack;

	void execute(Pop const&)
	{
		if (stack.empty())
			throw std::runtime_error("Cannot call Pop() on an empty stack");
		stack.pop();
	}

	void execute(Sleep const&) {}

	void execute(Push const& arg) { stack.push(arg.what); }

	void execute(Replace const& arg)
	{
		if (stack.empty()) {
			stack.push(arg.with);
		} else {
			stack.top() = arg.with;
		}
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
