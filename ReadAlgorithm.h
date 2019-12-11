#pragma once

#include "AlphaString.h"
#include "Stack.h"
#include "MeatBall.h"

namespace context_free {

template <typename CTerminal, typename CStack,
          typename It = typename AlphaString<CTerminal>::string::const_iterator>
bool readWord(It readFrom, It readTo, MeatBall<CTerminal, CStack> const& meatBall,
              Stack<CStack>& stack)
{
	if (readFrom == readTo) return stack.empty();

	const CTerminal* nextChar = *readFrom;

	auto const& transitions = meatBall.next(stack, *nextChar);

	for (auto const& [cmd, targetMeatBall] : transitions) {
		cmd->execute(stack);
		if (readWord(readFrom + 1, readTo, *targetMeatBall, stack)) return true;

		cmd->undo(stack);
	}

	return false;
}

} // namespace context_free
