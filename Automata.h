#pragma once

#include <forward_list>

#include "AlphaString.h"
#include "AlphabetTouple.h"
#include "BottomedStack.h"
#include "MeatBall.h"
#include "ReadAlgorithm.h"
#include "StackCommands.h"

namespace context_free {

template <typename CStack, typename CTerminal, typename CStackPtrBox> struct Automata
{
	const std::shared_ptr<AlphabetLike<CStack, CStackPtrBox>> stackAlphabet;
	const std::shared_ptr<AlphabetLike<CTerminal>> wordAlphabet;

	const std::forward_list<MeatBall<CStack, CTerminal, CStackPtrBox>> meatBalls;

	const MeatBall<CStack, CTerminal, CStackPtrBox>& start = meatBalls.front();

	auto createReader(AlphaString<CTerminal> const& word,
	                  std::optional<CStack> bottom = std::nullopt)
	{
		BottomedStack<CStack, CStackPtrBox> stack{stackAlphabet, bottom};
		return ReadState<CStack, CTerminal, CStackPtrBox>{word, start, stack};
	}
};

} // namespace context_free
