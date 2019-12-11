#pragma once

#include <forward_list>

#include "AlphaString.h"
#include "AlphabetTouple.h"
#include "MeatBall.h"
#include "ReadAlgorithm.h"
#include "Stack.h"
#include "StackCommands.h"

namespace context_free {

template <typename CTerminal, typename CStack = CTerminal> struct Automata
{
	const std::shared_ptr<AlphabetLike<CStack>> stackAlphabet;
	const std::shared_ptr<AlphabetLike<CTerminal>> wordAlphabet;

	const std::forward_list<MeatBall<CTerminal, CStack>> meatBalls;

	const MeatBall<CTerminal, CStack>& start = meatBalls.front();
	const std::optional<CStack> stackBottom{};

	bool readWord(AlphaString<CTerminal> const&)
	{
		Stack<CStack> stack{stackAlphabet, stackBottom};
		throw std::runtime_error(
		    "Word recognition unimplemented. Look at ReadAlgorithm.h");
	}
};

} // namespace context_free
