#pragma once

#include "AlphaString.h"
#include "MeatBall.h"
#include "Stack.h"
#include <list>

namespace context_free {

template <typename CTerminal, typename CStack> class ReadState
{
	using Word = AlphaString<CTerminal>;
	using WordPtr = typename Word::const_iterator;
	using MeatBallT = const MeatBall<CTerminal, CStack>;

	using StateHead = std::tuple<MeatBallT*, Stack<CStack>, WordPtr>;

	std::list<StateHead> heads{};

	using HeadsIt = typename decltype(heads)::iterator;

	bool advance(HeadsIt head)
	{
		auto& [meatBall, stack, nextChar] = *head;

		if (nextChar == word.string.end()) { // No more chars to read.
			// Hope we found it
			return stack.empty();
		}

		auto const& transitions = meatBall->next(stack, **nextChar);

		for (auto const& [cmd, targetMeatBall] : transitions) {
			auto newStack{stack};
			cmd->execute(newStack);

			heads.push_front(
			    std::make_tuple(targetMeatBall, newStack, nextChar + 1));
		}

		return false;
	}

public:
	const Word word;

	ReadState(Word const& word, MeatBallT const& meatBall,
	          Stack<CStack> const& stack)
	    : word(word)
	{
		StateHead init(&meatBall, stack, word.string.begin());
		heads.push_back(init);
	}

	std::optional<StateHead> advance()
	{
		for (HeadsIt it = heads.begin(); it != heads.end(); ) {
			if (advance(it)) {
				return *it;
			}

			auto old = it;
			++it;
			heads.erase(old);
		}

		return std::nullopt;
	}
};

} // namespace context_free
