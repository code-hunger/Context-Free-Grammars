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

template <typename C> class Alphabet
{
	/*
	 * An immutable object representing a finite alphabet
	 */
	using C_ptr_vec = std::vector<const C*>;

	const C_ptr_vec chars{};

	static C_ptr_vec rawifyChars(std::vector<unique_ptr<C>>&& chars)
	{
		C_ptr_vec rawified{};
		rawified.reserve(chars.size());
		for (unique_ptr<C>& c : chars) {
			if (std::find_if(begin(rawified), end(rawified), [&c](const C* b) {
				    return *c == *b;
			    }) == end(rawified))
				rawified.push_back(c.release());
		}
		return rawified;
	}

public:
	Alphabet(std::vector<unique_ptr<C>>&& chars)
	    : chars(rawifyChars(std::move(chars)))
	{
	}

	size_t size() const { return chars.size(); }

	const C* findChar(C const& c) const
	{
		auto what_found = std::find_if(chars.begin(), end(chars),
		                               [c](const C* x) { return *x == c; });

		return what_found == end(chars) ? nullptr : *what_found;
	}

	unique_ptr<Alphabet> operator+(Alphabet const&) const;
	bool subsetOf(Alphabet const& other)
	{
		return std::all_of(begin(chars), end(chars),
		                   [&other](auto c) { return other.findChar(c); });
	}

	~Alphabet()
	{
		for (const C* c : chars) {
			delete c;
		}
	}
};

} // namespace context_free
