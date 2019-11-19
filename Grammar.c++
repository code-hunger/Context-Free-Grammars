// #pragma once

#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace context_free {

using std::optional;
using std::shared_ptr;
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

	void print(std::ostream& out = std::cout) const override { out << value; }

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

	~Alphabet()
	{
		for (const C* c : chars) {
			delete c;
		}
	}
};

template <typename C> struct AlphaString
{
	const shared_ptr<Alphabet<C>> alphabet;

	/*
	 * Invariant: all chars string[i] are in the alphabet S
	 * i.e. std::all_of(begin(string), end(string), S.hasChar) == true
	 */
	const std::vector<const C*> string;

	void print(std::ostream& out = std::cout) const
	{
		for (auto c : string) {
			c->print(out);
		}
	}

	size_t size() const;

	static AlphaString<LetterChar>
	parseString(std::shared_ptr<Alphabet<LetterChar>> alphabet,
	            std::string const& str)
	{
		std::vector<const LetterChar*> new_string;

		for (LetterChar const& c : str) {
			auto in_alphabet = alphabet->findChar(c);

			if (in_alphabet == nullptr) {
				std::ostringstream error;
				error << "Attempted to parse a character not in alphabet: ";
				error << '"' << c.value << "\".";
				throw std::invalid_argument(error.str());
			}

			new_string.push_back(in_alphabet);
		}

		return {alphabet, std::move(new_string)};
	}

private:
	AlphaString(shared_ptr<Alphabet<C>> alphabet,
	            const std::vector<const C*>&& string)
	    : alphabet(alphabet), string(std::move(string))
	{
	}
};

} // namespace context_free

using namespace context_free;

std::vector<unique_ptr<LetterChar>> english()
{
	using std::make_unique;

	const std::string letters{"abcdefghijklmnopqrstuvwxyz"};

	std::vector<unique_ptr<LetterChar>> somechars;

	std::transform(begin(letters), end(letters), std::back_inserter(somechars),
	               [](char c) { return make_unique<LetterChar>(c); });

	return somechars;
}

int main()
{
	auto mylphabet = std::make_shared<Alphabet<LetterChar>>(english());

	std::string x;
	getline(std::cin, x);

	try {
		auto mystr = AlphaString<LetterChar>::parseString(mylphabet, x);
		mystr.print(std::cout);
		std::cout << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << "Couldn't parse the string '" << x << "' :(" << std::endl;
		std::cout << "Got error: " << e.what() << std::endl;
	}

	std::cout << "\nBye!" << std::endl;

	return 0;
}
