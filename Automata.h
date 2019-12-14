#pragma once

#include <forward_list>

#include "AlphaString.h"
#include "AlphabetTouple.h"
#include "BottomedStack.h"
#include "MeatBall.h"
#include "ReadAlgorithm.h"
#include "StackCommands.h"

namespace context_free {

template <typename CTerminal, typename CStack = CTerminal> struct Automata
{
	const std::shared_ptr<AlphabetLike<CStack>> stackAlphabet;
	const std::shared_ptr<AlphabetLike<CTerminal>> wordAlphabet;

	const std::forward_list<MeatBall<CTerminal, CStack>> meatBalls;

	const MeatBall<CTerminal, CStack>& start = meatBalls.front();

	auto createReader(AlphaString<CTerminal> const& word,
	                  std::optional<CStack> bottom = std::nullopt)
	{
		BottomedStack<CStack> stack{stackAlphabet, bottom};
		return ReadState{word, start, stack};
	}
};

} // namespace context_free
