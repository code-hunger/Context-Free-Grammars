#pragma once

#include "AlphaString.h"
#include "Alphabet.h"
#include <algorithm>

#include <memory>
#include <string>
#include <vector>

namespace context_free {

using std::string;
using std::unique_ptr;

struct Grammar
{
	virtual bool contains(string) const = 0;

	virtual unique_ptr<Grammar> union_with(Grammar const&) const = 0;
	virtual unique_ptr<Grammar> concat_with(Grammar const&) const = 0;
	virtual unique_ptr<Grammar> star() const = 0;

	virtual bool empty() const = 0;
	virtual bool infinite() const = 0;

	virtual ~Grammar() = default;
};

template <typename C> class Rule
{
	const C* const from;

	const AlphaString<C> to;
};

template <typename C> class GrammarTouple
{
	Alphabet<C> N, T;

	C start;
	std::vector<Rule<C>> rules;
};

} // namespace context_free
