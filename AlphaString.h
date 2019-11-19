#pragma once

#include "Alphabet.h"

namespace context_free {
using std::shared_ptr;

template <typename C> struct AlphaString
{
	const shared_ptr<Alphabet<C>> alphabet;

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
