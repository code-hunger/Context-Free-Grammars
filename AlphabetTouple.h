#pragma once

#include <memory>
#include <sstream>
#include <variant>

#include "Alphabet.h"

namespace context_free {

template <typename C1, typename C2> struct CharUnion
{
	const std::variant<const C1*, const C2*> value;

	void print(std::ostream& out) const
	{
		constexpr auto CharPrinter = [&out](const auto* t) { out << *t; };

		std::visit(CharPrinter, value);
	};
};

/*
 * A CharUnion makes no sense when C1 == C2.
 * Its only intended usage is as an argument ot AlphabetTouple<C1, C2> when C1
 * and C2 are different, otherwise AlphabetTouple uses a single C directly,
 * instead of `CharUnion<C, C>`.
 */
template <typename C> struct CharUnion<C, C>;

template <typename CN, typename CT = CN>
struct AlphabetTouple : public AlphabetLike<CharUnion<CN, CT>>
{
	using C = std::enable_if_t<std::is_same<CN, CT>::value, CN>;

	const std::shared_ptr<Alphabet<CN>> N;
	const std::shared_ptr<Alphabet<CT>> T;

	const CN* findChar(CN const& c) const override { return N->findChar(c); };

	const CT* findChar(CT const& c) const override { return T->findChar(c); };

	AlphabetTouple(decltype(N) N, decltype(T) T) : N(N), T(T) {}
};

template <typename C> struct AlphabetTouple<C, C> : public AlphabetLike<C>
{
	const std::shared_ptr<Alphabet<C>> N, T;

	const C* findChar(C const& c) const override
	{
		// c is guaranteed to be at most in one of N and T.
		if (auto a = N->findChar(c)) return a;
		return T->findChar(c);
	}

	bool all_of(std::function<bool(const C*)> const& predicate) const override
	{
		return N->all_of(predicate) && T->all_of(predicate);
	}

	void for_each(std::function<void(const C*)> const& predicate) const override
	{
		N->for_each(predicate);
		T->for_each(predicate);
	}

	AlphabetTouple(decltype(N) N, decltype(T) T) : N(N), T(T) {}
};

template <typename CN, typename CT = CN>
struct AlphabetToupleDistinct : public AlphabetTouple<CN, CT>
{
	/*
	 * A touple of 2 alphabets of different Char types is trivially distinct,
	 * no need for any checks.
	 */
	using parent = AlphabetTouple<CN, CT>;
	AlphabetToupleDistinct(decltype(parent::N) N, decltype(parent::T) T)
	    : parent(N, T)
	{
	}
};

template <typename C>
bool pairwiseDistinct(Alphabet<C> const& A, Alphabet<C> const& B)
{
	return A.all_of([&B](const C* c) { return !B.findChar(*c); });
}

template <typename C>
struct AlphabetToupleDistinct<C, C> : public AlphabetTouple<C, C>
{
	/*
	 * A touple of 2 alphabets of the same Char type must be manually checked
	 * for uniqueness of the elemnets of the 2 alphabets.
	 */
	using parent = AlphabetTouple<C, C>;
	AlphabetToupleDistinct(decltype(parent::N) N, decltype(parent::T) T)
	    : parent(N, T)
	{
		if (!pairwiseDistinct(*N, *T))
			throw std::invalid_argument(
			    "The two alphabets must be pairwise distinct.");
	}
};

} // namespace context_free
