#include <map>
#include <ostream>
#include <string>
#include <vector>
#include <forward_list>

#include "AlphaString.h"
#include "AlphabetTouple.h"
#include "Stack.h"
#include "StackCommands.h"

namespace context_free {

template <typename CTerminal, typename CStack> struct State
{
	const std::string human_name;

	State(std::string name) : human_name(name) {}
	State(State&&) = delete; //{ throw std::runtime_error("Should never happen"); }; // Needed if the automata uses std::vector for storing States
	State(State const&) = delete;

	std::map<
	    std::pair<std::optional<CStack>, CTerminal>,
	    std::vector<std::pair<std::shared_ptr<StackCommand<CStack>>, State*>>>
	    transitions{};

	auto& next(Stack<CStack> const& stack, CTerminal charToRead) const
	{
		static typename decltype(transitions)::mapped_type emptyTransition = {};
		auto x = transitions.find(std::make_pair(stack.top(), charToRead));

		return x != transitions.end() ? x->second : emptyTransition;
	}

	static void printOrMissing(std::ostream& out,
	                           std::optional<CStack> const& c)
	{
		out << c.has_value() ? *c : CStack{'-'};
	}

	void printTransitions(std::ostream& out) const
	{
		for (auto const& [from, to] : transitions) {
			for (auto const& [command, targetState] : to) {
				out << " | ";
				printOrMissing(out, from.first);
				out << ", " << from.second << " --> ";
				command->print(out);
				out << " -> " << targetState->human_name << std::endl;
			}
		}
	}
};

template <typename CTerminal, typename CStack,
          typename It = typename AlphaString<CTerminal>::string::const_iterator>
bool readWord(It readFrom, It readTo, State<CTerminal, CStack> const& state,
              Stack<CStack>& stack)
{
	if (readFrom == readTo) return stack.empty();

	const CTerminal* nextChar = *readFrom;

	auto const& transitions = state.next(stack, *nextChar);

	for (auto const& [cmd, targetState] : transitions) {
		cmd->execute(stack);
		if (readWord(readFrom + 1, readTo, *targetState, stack)) return true;

		cmd->undo(stack);
	}

	return false;
}

template <typename CTerminal, typename CStack = CTerminal> struct Automata
{
	const std::shared_ptr<AlphabetTouple<CTerminal, CStack>> alphabets;

	const std::forward_list<State<CTerminal, CStack>> states;

	const State<CTerminal, CStack>& start = states.front();
	const std::optional<CStack> stackBottom{};

	bool readWord(AlphaString<CTerminal> const& string)
	{
		Stack<CStack> stack{alphabets->N, stackBottom};
		return context_free::readWord(string.string.begin(),
		                              string.string.end(), start, stack);
	}
};

} // namespace context_free
