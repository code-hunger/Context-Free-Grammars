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

template <typename T, typename P> static bool all_of(T container, P predicate)
{
	return std::all_of(container.begin(), container.end(), predicate);
}

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

template <typename C> struct Rule
{
	const C* const from;

	const AlphaString<C> to;

	Rule(C const& from, AlphaString<C>&& to) : from(&from), to(std::move(to)) {}
};

template <typename C>
bool pairwiseDistinct(Alphabet<C> const& A, Alphabet<C> const& B)
{
	return all_of(A, [&B](const C* c) { return !B.findChar(*c); });
}

template <typename C> struct GrammarTouple
{
	using SharedAlphabet = shared_ptr<Alphabet<C>>;

	const SharedAlphabet N, T;
	const Alphabet<C> N_union_T = (*N) + (*T);

	const C start;
	const std::vector<Rule<C>> rules;

	GrammarTouple(decltype(N) N, decltype(T) T, C const& start,
	              decltype(rules) rules)
	    : N(N), T(T), start(start), rules(rules)
	{
		if (!N->findChar(start)) {
			std::ostringstream error;
			error << "Start character '";
			start.print(error);
			error << "' not present in the Non-terminals alphabet.";

			throw std::invalid_argument(error.str());
		}

		if (!pairwiseDistinct(*N, *T)) {
			throw std::invalid_argument("The Terminals and the Non-terminals "
			                            "alphabets must be pairwise distinct.");
		}
	};
};

template <typename C> struct CFGrammarTouple : GrammarTouple<C>
{
	CFGrammarTouple(decltype(GrammarTouple<C>::N) N,
	                decltype(GrammarTouple<C>::T) T, C const& start,
	                decltype(GrammarTouple<C>::rules) rules)
	    : GrammarTouple<C>(N, T, start, rules)
	{
		if (!all_of(rules,
		            [&N](auto& rule) { return N.findChar(rule.from); })) {
			throw std::invalid_argument(
			    "All rules must satisfy N.findChar(rule.from) != nullptr");
		}

		if (!all_of(rules, [& N_union_T = this->N_union_T](auto& rule) {
			    return rule.to.alphabet.subsetOf(N_union_T);
		    })) {
			throw std::invalid_argument("The 'to' part of all rules must be a "
			                            "string from the alphabet union N+T.");
		}
	}
};

} // namespace context_free