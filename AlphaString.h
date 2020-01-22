#pragma once

#include <sstream>
#include "Alphabet.h"

namespace context_free {
using std::shared_ptr;

template <typename C, typename CPtrBox = const C*>
struct AlphaString : FunctorLike<C> //, FunctorLike<CPtrBox>
{
	const shared_ptr<AlphabetLike<C, CPtrBox>> alphabet;

	/*
	 * Invariant: all chars string[i] are in the alphabet S
	 * i.e. std::all_of(begin(string), end(string), S.hasChar) == true
	 */
	const std::vector<CPtrBox> string;

	using iterator = typename decltype(string)::iterator;
	using const_iterator = typename decltype(string)::const_iterator;

	void print(std::ostream& out) const
	{
		if (string.size() == 0) {
			out << "@";
			return;
		}

		for (auto c : string) {
			c->print(out);
		}
	}

	//void for_each(std::function<void(CPtrBox const&)> const& p) const override
	//{
		//std::for_each(string.begin(), string.end(), p);
	//}

	//bool all_of(std::function<bool(CPtrBox const&)> const& p) const override
	//{
		//return std::all_of(string.begin(), string.end(), p);
	//}

	void for_each(std::function<void(const C&)> const& p) const override
	{
		for (CPtrBox c : string) {
			p(*c);
		}
	}

	bool all_of(std::function<bool(const C&)> const& p) const override
	{
		for (CPtrBox c : string) {
			if (!p(*c)) {
				return false;
			}
		}

		return true;
	}

	size_t size() const;

	static AlphaString<LetterChar> parseString(shared_ptr<Alphabet<LetterChar>>,
	                                           std::string const&);

	static AlphaString<C, CPtrBox>
	parseString(shared_ptr<AlphabetLike<C, CPtrBox>> alphabet,
	            std::string const& str)
	{
		if (str.size() <= 1 && str[0] == '@')
			return {Alphabet<C, CPtrBox>::constructEmpty(), {}};

		std::vector<CPtrBox> new_string;

		for (char c : str) {
			auto in_alphabet = alphabet->findChar(C{c});

			if (in_alphabet == nullptr) {
				std::ostringstream error;
				error << "Attempted to parse a character not in alphabet: \"" << c;
				error << "\".";
				throw std::invalid_argument(error.str());
			}

			new_string.push_back(in_alphabet);
		}

		return {alphabet, std::move(new_string)};
	}

private:
	AlphaString(shared_ptr<AlphabetLike<C, CPtrBox>> alphabet,
	            const std::vector<CPtrBox>&& string)
	    : alphabet(alphabet), string(std::move(string))
	{
	}
};

template <typename C>
std::ostream& operator<<(std::ostream& out, AlphaString<C> const& string)
{
	string.print(out);
	return out;
}

} // namespace context_free
