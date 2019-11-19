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

template <typename C> struct Rule
{
	const C* const from;

	const AlphaString<C> to;

	Rule(C const& from, AlphaString<C>&& to) : from(&from), to(std::move(to)) {}
};

template <typename C>
bool pairwiseDistinct(Alphabet<C> const& A, Alphabet<C> const& B)
{
	return std::all_of(A.begin(), A.end(),
	                   [&B](const C* c) { return !B.findChar(*c); });
}

template <typename C> class GrammarTouple
{
	using SharedAlphabet = shared_ptr<Alphabet<C>>;

protected:
	const SharedAlphabet N, T;

	const Alphabet<C> N_union_T = N + T;

	C start;
	std::vector<Rule<C>> rules;

public:
	GrammarTouple(decltype(N) N, decltype(T) T, C start, decltype(rules) rules)
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

template <typename C> struct CFGrammarTouple : public GrammarTouple<C>
{
	CFGrammarTouple(decltype(GrammarTouple<C>::N) N,
	                decltype(GrammarTouple<C>::T) T, C start,
	                decltype(GrammarTouple<C>::rules) rules)
	    : GrammarTouple<C>(N, T, start, rules)
	{
		if (!std::all_of(begin(rules), end(rules), [&N](auto const& rule) {
			    return N.findChar(rule.from);
		    })) {
			throw std::invalid_argument(
			    "All rules must satisfy N.findChar(rule.from) != nullptr");
		}

		if (!std::all_of(begin(rules), end(rules),
		                 [& N_union_T = this->N_union_T](auto const& rule) {
			                 return rule.to.alphabet.subsetOf(N_union_T);
		                 })) {
			throw std::invalid_argument("The 'to' part of all rules must be a "
			                            "string from the alphabet union N+T.");
		}
	}
};

} // namespace context_free
