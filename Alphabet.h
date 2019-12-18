#pragma once

#include <algorithm>
#include <memory>
#include <sstream>
#include <vector>

namespace context_free {

using std::unique_ptr;

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

inline bool operator<(LetterChar const& a, LetterChar const& b)
{
	return a.value < b.value;
}

inline std::ostream& operator<<(std::ostream& out, LetterChar const& c)
{
	c.print(out);
	return out;
}

template <typename T> struct FunctorLike
{
	virtual bool all_of(std::function<bool(T const&)> const&) const = 0;
	virtual void for_each(std::function<void(T const&)> const&) const = 0;

	virtual ~FunctorLike() = default;
};

template <typename C, typename CPtrBox = const C*>
struct AlphabetLike : FunctorLike<C>
{
	using char_type = C;
	using char_box_type = CPtrBox;

	virtual CPtrBox findChar(C const&) const = 0;

	bool subsetOf(AlphabetLike<C> const& other) const
	{
		return this->all_of([&other](C const& c) { return other.findChar(c); });
	}

	void print(std::ostream& out) const
	{
		this->for_each([&out](C const& c) { c.print(out); });
	}

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

template <typename C> class Alphabet : public AlphabetLike<C, const C*>
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

	auto begin() const { return std::begin(chars); }
	auto end() const { return std::end(chars); }

	bool all_of(std::function<bool(C const&)> const& predicate) const override
	{
		for (const C* c : *this)
			if (!predicate(*c)) return false;
		return true;
	}

	void for_each(std::function<void(C const&)> const& predicate) const override
	{
		for (const C* c : *this)
			predicate(*c);
	}

	~Alphabet()
	{
		for (const C* c : *this) {
			delete c;
		}
	}
};

template <typename C> std::shared_ptr<Alphabet<C>> Alphabet<C>::emptyAlphabet{};

template <typename C, typename CPtrBox>
std::ostream& operator<<(std::ostream& out,
                         AlphabetLike<C, CPtrBox> const& alphabet)
{
	alphabet.print(out);
	return out;
}

} // namespace context_free
