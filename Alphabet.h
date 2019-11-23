#pragma once

#include <algorithm>
#include <cassert>
#include <memory>
#include <sstream>
#include <variant>
#include <vector>

namespace context_free {

using std::unique_ptr;

template <typename T, typename P>
static bool all_of(T const& container, P predicate)
{
	return std::all_of(container.begin(), container.end(), predicate);
}

struct Char
{
	virtual void print(std::ostream&) const = 0;
	virtual ~Char(){};
};

template <typename C>
typename std::enable_if_t<std::is_base_of<Char, C>::value, bool>
operator==(C const& c, C const&);

struct LetterChar : Char
{
	const char value;

	LetterChar(char c) : value(c) {}

	void print(std::ostream& out) const override { out << value; }

	friend bool operator==(LetterChar a, LetterChar b)
	{
		return a.value == b.value;
	}
};

template <typename C1, typename C2>
using types_not_same = std::negation<std::is_same<C1, C2>>;

template <typename C1, typename C2,
          typename = typename std::enable_if<types_not_same<C1, C2>::value>>
struct CharUnion
{
	const std::variant<const C1*, const C2*> value;

	void print(std::ostream& out) const
	{
		auto CharPrinter = [&out](const auto* t) { out << *t; };
		std::visit(CharPrinter, value);
	};
};

inline std::ostream& operator<<(std::ostream& out, LetterChar const& c)
{
	c.print(out);
	return out;
}

template <typename C> struct AlphabetLike
{
	virtual const C* findChar(C const&) const = 0;
	virtual bool subsetOf(AlphabetLike const& other) const = 0;
	virtual void print(std::ostream&) const = 0;

	AlphabetLike() = default;
	AlphabetLike(AlphabetLike&&) = default;
	AlphabetLike(AlphabetLike const&) = delete;

	virtual ~AlphabetLike() {}
};

template <typename C>
std::vector<unique_ptr<C>> stringToPtrVec(std::string const& string)
{
	std::vector<unique_ptr<C>> vector{string.size()};

	std::transform(begin(string), end(string), begin(vector),
	               [](char c) { return std::make_unique<C>(c); });

	return vector;
}

template <typename C> class Alphabet : public AlphabetLike<C>
{
	/*
	 * An immutable object representing a finite alphabet
	 */
	using C_ptr_vec = std::vector<const C*>;

	const C_ptr_vec chars{};

	static std::shared_ptr<Alphabet<C>> emptyAlphabet;

	static C_ptr_vec rawifyChars(std::vector<unique_ptr<C>>&& chars)
	{
		C_ptr_vec rawified{};
		rawified.reserve(chars.size());
		for (unique_ptr<C>& c : chars) {
			if (std::find_if(std::begin(rawified), std::end(rawified),
			                 [& c = *c](const C* b) { return c == *b; }) ==
			    std::end(rawified))
				rawified.push_back(c.release());
		}
		return rawified;
	}

	enum DummyEmpty { DummyEmpty };

	Alphabet(enum DummyEmpty) : chars{} {}

public:
	Alphabet(std::vector<unique_ptr<C>>&& chars)
	    : chars(rawifyChars(std::move(chars)))
	{
		if (chars.size() == 0)
			throw std::invalid_argument(
			    "The empty alphabet must be constructed using "
			    "Alphabet::constructEmpty().");
	}

	explicit Alphabet(std::string const& string)
	    : Alphabet(stringToPtrVec<C>(string)){};

	Alphabet(Alphabet const&) = delete;
	Alphabet(Alphabet&&) = default;

	static auto constructEmpty()
	{
		if (!emptyAlphabet)
			emptyAlphabet = std::make_shared<Alphabet>(Alphabet<C>{DummyEmpty});

		return emptyAlphabet;
	}

	size_t size() const { return chars.size(); }

	const C* findChar(C const& c) const override
	{
		auto what_found =
		    std::find_if(begin(), end(), [c](const C* x) { return *x == c; });

		return what_found == end() ? nullptr : *what_found;
	}

	Alphabet operator+(Alphabet const&) const
	{
		throw std::runtime_error("Union operation unimplemented.");
	}

	bool subsetOf(AlphabetLike<C> const& other) const override
	{
		return all_of(*this,
		              [&other](const C* c) { return other.findChar(*c); });
	}

	auto begin() const { return std::begin(chars); }
	auto end() const { return std::end(chars); }

	template <typename P> void for_each(P const& p) const
	{
		std::for_each(begin(), end(), p);
	}

	void print(std::ostream& out) const override
	{
		for_each([&out](const C* c) { c->print(out); });
	}

	~Alphabet()
	{
		for_each([](const C* c) { delete c; });
	}
};

template <typename C> std::shared_ptr<Alphabet<C>> Alphabet<C>::emptyAlphabet{};

template <typename C>
std::ostream& operator<<(std::ostream& out, AlphabetLike<C> const& alphabet)
{
	alphabet.print(out);
	return out;
}

template <typename C>
bool pairwiseDistinct(Alphabet<C> const& A, Alphabet<C> const& B)
{
	return all_of(A, [&B](const C* c) { return !B.findChar(*c); });
}

template <typename CN, typename CT>
struct AlphabetTouple : public AlphabetLike<CharUnion<CN, CT>>
{
	using C = std::enable_if_t<std::is_same<CN, CT>::value, CN>;

	static constexpr auto CharPrinter = [](std::ostream& out) {
		return [&out](const auto* c) { c->print(out); };
	};

	const std::shared_ptr<Alphabet<CN>> N;
	const std::shared_ptr<Alphabet<CT>> T;

	const CN* findChar(CN const& c) const override { return N->findChar(c); };

	const CT* findChar(CT const& c) const override { return T->findChar(c); };

	bool subsetOf(AlphabetLike<C> const& other) const override
	{
		auto otherHasChar = [&other](auto const* c) {
			return other.findChar(*c);
		};

		return all_of(*N, otherHasChar) && all_of(*T, otherHasChar);
	};

	void print(std::ostream& out) const override
	{
		N->for_each(CharPrinter(out));
		T->for_each(CharPrinter(out));
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
	};

	bool subsetOf(AlphabetLike<C> const& other) const override
	{
		auto otherHasChar = [&other](const C* c) { return other.findChar(*c); };

		return all_of(*N, otherHasChar) && all_of(*T, otherHasChar);
	};

	void print(std::ostream& out) const override
	{
		auto printChar = [&out](const C* c) { c->print(out); };
		N->for_each(printChar);
		T->for_each(printChar);
	}

	AlphabetTouple(decltype(N) N, decltype(T) T) : N(N), T(T)
	{
		if (!pairwiseDistinct(*N, *T))
			throw std::invalid_argument(
			    "The two alphabets must be pairwise distinct.");
	}
};

} // namespace context_free
