#pragma once

#include "AlphaString.h"
#include "MeatBall.h"
#include "Stack.h"
#include <forward_list>

namespace context_free {

template <typename CTerminal, typename CStack> class ReadState
{
	using Word = AlphaString<CTerminal>;
	using WordPtr = typename Word::iterator;

	using StateHead =
	    std::tuple<MeatBall<CTerminal, CStack>*, Stack<CStack>, WordPtr>;

	std::forward_list<StateHead> heads;

	using HeadsIt = typename decltype(heads)::iterator;

public:
	const Word word;

	bool advance(HeadsIt head)
	{
		throw std::runtime_error("To be implemented.");
	}
};

} // namespace context_free
