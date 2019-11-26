#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "AlphaString.h"
#include "AlphabetTouple.h"
#include "Stack.h"
#include "StackCommands.h"

namespace context_free {

template <typename CN, typename CT> struct State
{
	const std::string human_name = std::to_string(static_cast<int>(this));

	std::map<std::pair<std::optional<CN>, CT>,
	         std::vector<std::pair<StackCommand<CN>*, State*>>>
	    transitions{};

	auto& next(Stack<CN> const& stack, CT charToRead) const
	{
		static typename decltype(transitions)::mapped_type emptyTransition = {};
		auto x = transitions.find(std::make_pair(stack.top(), charToRead));

		return x != transitions.end() ? x->second : emptyTransition;
	}

	static void printOrMissing(std::ostream& out, std::optional<CN> const& c)
	{
		out << c.has_value() ? *c : CN{'-'};
	}

	void printTransitions(std::ostream& out) const
	{
		for (auto const& [from, to] : transitions) {
			for (auto const& to : to) {
				const StackCommand<CN>& cmd = *to.first;
				out << " | ";
				printOrMissing(out, from.first);
				out << ", " << from.second << " --> ";
				cmd.print(out);
				out << " -> " << to.second->human_name << std::endl;
			}
		}
	}
};

template <typename CN, typename CT,
          typename It = typename AlphaString<CT>::string::const_iterator>
bool readWord(It readFrom, It readTo, State<CN, CT> const& state,
              Stack<CN>& stack)
{
	if (readFrom == readTo) return stack.empty();

	const CT* nextChar = *readFrom;

	auto const& transitions = state.next(stack, *nextChar);

	for (auto const& [cmd, targetState] : transitions) {
		cmd->execute(stack);
		if (readWord(readFrom + 1, readTo, *targetState, stack)) return true;

		cmd->undo(stack);
	}
	return false;
}

template <typename CN, typename CT = CN> struct Automata
{
	const std::shared_ptr<AlphabetTouple<CN, CT>> alphabets;

	const std::vector<State<CN, CT>> states;

	const State<CN, CT>& start = states.front();
	const std::optional<CN> stackBottom{};

	bool readWord(AlphaString<CT> const& string)
	{
		Stack<CN> stack{alphabets->N, stackBottom};
		return context_free::readWord(string.string.begin(),
		                              string.string.end(), start, stack);
	}
};

} // namespace context_free
