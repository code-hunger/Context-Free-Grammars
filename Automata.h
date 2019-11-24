#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "AlphaString.h"
#include "AlphabetTouple.h"
#include "Stack.h"

namespace context_free {

template <typename CN, typename CT> struct State
{
	const std::string human_name = std::to_string(static_cast<int>(this));

	std::map<std::pair<std::optional<CN>, CT>,
	         std::vector<std::pair<typename Stack<CN>::Command, State*>>>
	    transitions{};

	auto& next(Stack<CN> const& stack, CT charToRead) const
	{
		static typename decltype(transitions)::mapped_type emptyTransition = {};
		auto x = transitions.find(std::make_pair(stack.top(), charToRead));

		return x != transitions.end() ? x->second : emptyTransition;
	}

	static void printOrMissing(std::ostream& out, std::optional<CN> c)
	{
		if (c.has_value())
			out << *c;
		else
			out << "-";
	}

	void printTransitions(std::ostream& out)
	{
		for (auto const& [from, to] : transitions) {
			for (auto to : to) {
				const typename Stack<CN>::Command& cmd = to.first;
				out << " | ";
				printOrMissing(out, from.first);
				out << ", " << from.second << " --> ";
				printCommand<CN>(out, cmd);
				out << " -> " << to.second->human_name << std::endl;
			}
		}
	}
};

template <typename CN, typename CT,
          typename It = typename AlphaString<CT>::string::iterator>
bool readWord(It readFrom, It readTo, State<CN, CT>& state, Stack<CN>& stack)
{
	if (readFrom == readTo) return true;

	const CT* nextChar = *readFrom;

	auto const& transitions = state.next(stack, *nextChar);

	for (const auto& [cmd, targetState] : transitions) {
		const auto& invertedCommand = stack.invertCommand(cmd);

		stack.fire(cmd);
		if (readWord(readFrom + 1, readTo, *targetState, stack)) return true;

		stack.fire(invertedCommand);
	}
	return false;
}

template <typename CN, typename CT = CN> struct Automata
{
	std::shared_ptr<AlphabetTouple<CN, CT>> alphabets;

	std::vector<State<CN, CT>> states;
	State<CN, CT>& start = states.front();

	bool readWord(AlphaString<CT> const& string)
	{
		Stack<CN> stack{alphabets->N};
		return context_free::readWord(string.string.begin(),
		                              string.string.end(), start, stack);
	}
};

} // namespace context_free
