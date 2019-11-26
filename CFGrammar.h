#pragma once

#include "AlphaString.h"
#include "Alphabet.h"
#include "AlphabetTouple.h"
#include <algorithm>

#include <memory>
#include <vector>

namespace context_free {

template <typename CN, typename CT = CN> struct Rule
{
	using C = std::conditional<std::is_same_v<CN, CT>, CN, CharUnion<CN, CT>>;

	using FromType = CN;
	using ToType = typename C::type;

	const FromType& from;
	const AlphaString<ToType> to;

	Rule(const FromType& from, AlphaString<ToType>&& to)
	    : from(from), to(std::move(to))
	{
	}
};

template <typename CN, typename CT> struct GrammarTouple
{
	const shared_ptr<AlphabetToupleDistinct<CN, CT>> alphabets;

	const CN* start;
	const std::vector<Rule<CN, CT>> rules;

	GrammarTouple(decltype(alphabets) alphabets, CN const& start,
	              decltype(rules) rules)
	    : alphabets(alphabets), start(alphabets->N->findChar(start)),
	      rules(rules)
	{
		if (!this->start) {
			std::ostringstream error;
			error << "Start character '";
			start.print(error);
			error << "' not present in the Non-terminals alphabet.";

			throw std::invalid_argument(error.str());
		}
	};

	GrammarTouple(GrammarTouple const&) = delete;
	auto operator=(GrammarTouple const&) = delete;
};

template <typename T, typename P>
static bool all_of(T const& container, P const& predicate)
{
	return std::all_of(container.begin(), container.end(), predicate);
}

template <typename CN, typename CT>
struct CFGrammarTouple : GrammarTouple<CN, CT>
{
	using parent = GrammarTouple<CN, CT>;

	CFGrammarTouple(decltype(parent::alphabets) alphabets, CN const& start,
	                decltype(parent::rules) rules)
	    : parent(alphabets, start, rules)
	{
		if (!all_of(rules, [& N = *alphabets->N](auto& rule) {
			    return N.findChar(rule.from);
		    })) {
			throw std::invalid_argument(
			    "All rules must satisfy N.findChar(rule.from) != nullptr");
		}

		if (!all_of(rules, [& alphabets = *alphabets](auto& rule) {
			    return rule.to.alphabet->subsetOf(alphabets);
		    })) {
			throw std::invalid_argument("The 'to' part of all rules must be a "
			                            "string from the alphabet union N+T.");
		}
	}
};

} // namespace context_free
