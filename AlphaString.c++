#include "AlphaString.h"

using namespace context_free;

template<> AlphaString<LetterChar> AlphaString<LetterChar>::parseString(
    std::shared_ptr<Alphabet<LetterChar>> alphabet, std::string const& str)
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
