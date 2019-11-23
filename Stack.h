#include <variant>
#include <stack>
#include <stdexcept>

namespace context_free {

template <typename C> struct Stack
{
	struct Pop {};
	struct Sleep {};
	struct Push { C what; };
	struct Replace { C with; };

	using Command = const std::variant<Push, Pop, Replace, Sleep>;

	void fire(Command& command)
	{
		// Magic!
		std::visit([*this](auto const& arg) { execute(arg); }, command);
	}

private:
	std::stack<C> stack;

	void execute(Pop)
	{
		if (stack.empty()) throw std::runtime_error("Cannot call Pop() on an empty stack");
		stack.pop();
	}

	void execute(Sleep&) {}

	void execute(Push const& arg) { stack.push(arg.what); }

	void execute(Replace const& arg)
	{
		if (stack.empty()) {
			stack.push(arg.with);
		} else {
			stack.top() = arg.with;
		}
	}
};

} // namespace context_free
