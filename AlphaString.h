#pragma once

#include "Alphabet.h"

namespace context_free {
using std::shared_ptr;

template <typename C> struct AlphaString
{
	const shared_ptr<AlphabetLike<C>> alphabet;

	/*
	 * Invariant: all chars string[i] are in the alphabet S
	 * i.e. std::all_of(begin(string), end(string), S.hasChar) == true
	 */
	const std::vector<const C*> string;

	void print(std::ostream& out) const
	{
		for (auto c : string) {
			c->print(out);
		}
	}

	size_t size() const;

	static AlphaString<LetterChar>
	parseString(shared_ptr<Alphabet<LetterChar>>, std::string const&);

	static AlphaString<C> parseString(shared_ptr<AlphabetLike<C>> alphabet,
	                                  std::string const& str)
	{
		std::vector<const C*> new_string;

		for (C const& c : str) {
			auto in_alphabet = alphabet->findChar(c);

			if (in_alphabet == nullptr) {
				throw std::invalid_argument(
				    "Attempted to parse a character not in alphabet.");
			}

			new_string.push_back(in_alphabet);
		}

		return {alphabet, std::move(new_string)};
	}

private:
	AlphaString(shared_ptr<AlphabetLike<C>> alphabet,
	            const std::vector<const C*>&& string)
	    : alphabet(alphabet), string(std::move(string))
	{
	}
};

} // namespace context_free
