#pragma once

#include "AlphaString.h"
#include "MeatBall.h"
#include "Stack.h"
#include <iostream>
#include <list>

namespace context_free {

template <typename CStack, typename CTerminal, typename CStackPtrBox>
class ReadState
{
	using Word = AlphaString<CTerminal>;
	using WordPtr = typename Word::const_iterator;
	using MeatBallT = const MeatBall<CStack, CTerminal, CStackPtrBox>;

	using StateHead =
	    std::tuple<MeatBallT*, std::unique_ptr<Stack<CStack, CStackPtrBox>>,
	               WordPtr>;

	std::list<StateHead> heads{};

	using HeadsIt = typename decltype(heads)::iterator;

	bool advance(StateHead const& head)
	{
		auto& [meatBall, stack, nextCharIt] = head;

		if (stack->empty()) { // No more chars to read.
			// Hope we found it
			return nextCharIt == word.string.end();
		}

		const auto nextChar = nextCharIt == word.string.end()
		                          ? std::nullopt
		                          : std::make_optional(**nextCharIt);

		auto const& eating = meatBall->next(*stack, nextChar);

		for (auto const& [cmd, targetMeatBall] : eating) {
			auto newStack = stack->clone();
			cmd->execute(*newStack);

			heads.push_front(
			    std::make_tuple(targetMeatBall, std::move(newStack),
			                    nextChar ? std::next(nextCharIt) : nextCharIt));
		}

		if (nextChar)
			for (auto const& [cmd, targetMeatBall] :
			     meatBall->next(*stack, std::nullopt)) {
				auto newStack = stack->clone();
				cmd->execute(*newStack);

				heads.push_front(std::make_tuple(
				    targetMeatBall, std::move(newStack), nextCharIt));
			}

		return false;
	}

public:
	const Word word;

	ReadState(Word const& word, MeatBallT const& meatBall,
	          std::unique_ptr<Stack<CStack, CStackPtrBox>> stack)
	    : word(word)
	{
		heads.push_back(
		    {&meatBall, std::move(stack), this->word.string.begin()});
	}

	struct AdvanceResult
	{
		const bool finished;
		const std::optional<MeatBallT*> acceptingState = nullptr;
	};

	AdvanceResult advance()
	{
		for (HeadsIt it = heads.begin(); it != heads.end();) {
			if (advance(*it)) {
				return {true, {std::get<0>(*it)}};
			}

			auto old = it;
			++it;
			heads.erase(old);
		}

		if (heads.empty()) return {true, std::nullopt};

		return {false, std::nullopt};
	}
};
} // namespace context_free
