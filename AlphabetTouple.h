#include <memory>
#include <sstream>
#include <variant>

#include "Alphabet.h"

namespace context_free {

template <typename C1, typename C2>
using types_not_same = std::negation<std::is_same<C1, C2>>;

template <typename C1, typename C2,
          typename = typename std::enable_if<types_not_same<C1, C2>::value>>
struct CharUnion
{
	const std::variant<const C1*, const C2*> value;

	void print(std::ostream& out) const
	{
		constexpr auto CharPrinter = [&out](const auto* t) { out << *t; };

		std::visit(CharPrinter, value);
	};
};

template <typename C>
bool pairwiseDistinct(Alphabet<C> const& A, Alphabet<C> const& B)
{
	return all_of(A, [&B](const C* c) { return !B.findChar(*c); });
}

template <typename CN, typename CT = CN>
struct AlphabetTouple : public AlphabetLike<CharUnion<CN, CT>>
{
	using C = std::enable_if_t<std::is_same<CN, CT>::value, CN>;

	const std::shared_ptr<Alphabet<CN>> N;
	const std::shared_ptr<Alphabet<CT>> T;

	const CN* findChar(CN const& c) const override { return N->findChar(c); };

	const CT* findChar(CT const& c) const override { return T->findChar(c); };

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

	AlphabetTouple(decltype(N) N, decltype(T) T) : N(N), T(T)
	{
		if (!pairwiseDistinct(*N, *T))
			throw std::invalid_argument(
			    "The two alphabets must be pairwise distinct.");
	}
};

} // namespace context_free
