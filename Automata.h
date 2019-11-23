#include <map>
#include <string>
#include <vector>

#include "Stack.h"

namespace context_free {

template <typename CN, typename CT> struct State
{
	const std::string human_name = std::to_string(static_cast<int>(this));

	std::map<std::pair<CN, CT>,
	         std::vector<std::pair<typename Stack<CN>::Command, State*>>>
	    transitions{};
};

template <typename CN, typename CT = CN> struct Automata
{
	std::vector<State<CN, CT>> states;
	State<CN, CT>& start = states.front();
};

template <typename CN, typename CT> struct Instant
{
	Automata<CN, CT>& A;
	Stack<CN> stack;

	State<CN, CT>& state;

	std::string wordToRead;
};

} // namespace context_free
