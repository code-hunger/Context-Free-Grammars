#pragma once

#include "AlphaString.h"
#include "Alphabet.h"
#include <algorithm>

#include <memory>
#include <vector>

namespace context_free {

template <typename C> struct Rule
{
	const C& from;

	const AlphaString<C> to;

	Rule(C const& from, AlphaString<C>&& to) : from(from), to(std::move(to)) {}
};

template <typename C> struct GrammarTouple
{
	using SharedAlphabet = shared_ptr<Alphabet<C>>;

	const shared_ptr<AlphabetTouple<C>> alphabets;

	const C* start;
	const std::vector<Rule<C>> rules;

	GrammarTouple(decltype(alphabets) alphabets, C const& start,
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

template <typename C> struct CFGrammarTouple : GrammarTouple<C>
{
	CFGrammarTouple(decltype(GrammarTouple<C>::alphabets) alphabets,
	                C const& start, decltype(GrammarTouple<C>::rules) rules)
	    : GrammarTouple<C>(alphabets, start, rules)
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
