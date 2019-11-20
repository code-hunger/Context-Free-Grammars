#include "CFGrammar.h"
#include <iostream>
#include <memory>
#include <ostream>

namespace context_free {

namespace {
using std::istream;

AlphaString<LetterChar>
parseString(istream& in, std::shared_ptr<AlphabetLike<LetterChar>> alphabet)
{
	std::string string;
	in >> string;

	return AlphaString<LetterChar>::parseString(alphabet, string);
}

template <typename C>
const C& parseLetterChar(istream& input, AlphabetLike<C> const& alphabet)
{
	char from;
	input >> from;

	const C* in_alphabet = alphabet.findChar(from);

	if (!in_alphabet) {
		throw std::runtime_error("Couldn't read a Non-terminal from the input");
	}

	return *in_alphabet;
}

template <typename C = LetterChar>
using AlphabetsPtr = std::shared_ptr<AlphabetTouple<C>>;

template <typename C>
Rule<C> parseRule(istream& input, AlphabetsPtr<C> alphabets)
{
	return {parseLetterChar(input, *alphabets->N),
	        parseString(input, alphabets)};
}

bool streamFinished(istream& input)
{
	input >> std::ws;
	return input.eof();
}

template <typename C> auto parseRules(istream& input, AlphabetsPtr<C> alphabets)
{
	std::vector<Rule<C>> rules;

	while (input && !streamFinished(input)) {
		try {
			rules.push_back(parseRule(input, alphabets));
		} catch (std::runtime_error const& e) {
			std::cerr << "Failed to parse a rule. Error: " << std::endl
			          << e.what() << std::endl
			          << "Won't read for more rules." << std::endl;
			break;
		}
	}

	return rules;
}

template <typename C>
CFGrammarTouple<C> parseGrammar(istream& input, AlphabetsPtr<C> alphabets)
{
	return {alphabets, parseLetterChar(input, *alphabets->N),
	        parseRules(input, alphabets)};
}

} // namespace

} // namespace context_free
