#pragma once

#include "CFGrammar.h"
#include <iostream>
#include <memory>
#include <ostream>

namespace context_free {

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

template <typename CN, typename CT = CN>
using AlphabetsPtr = std::shared_ptr<AlphabetToupleDistinct<CN, CT>>;

template <typename CN, typename CT>
Rule<CN, CT> parseRule(istream& input, AlphabetsPtr<CN, CT> alphabets)
{
	return {parseLetterChar(input, *alphabets->N),
	        parseString(input, alphabets)};
}

bool streamFinished(istream& input)
{
	input >> std::ws;
	return input.eof();
}

template <typename CN, typename CT>
auto parseRules(istream& input, AlphabetsPtr<CN, CT> alphabets)
{
	std::vector<Rule<CN, CT>> rules;

	while (input && !streamFinished(input)) {
		try {
			rules.push_back(parseRule(input, alphabets));
		} catch (std::exception const& e) {
			std::cerr << "Failed to parse a rule. Error: " << std::endl
			          << e.what() << std::endl
			          << "Won't read for more rules.\n" << std::endl;
			break;
		}
	}

	return rules;
}

template <typename CN, typename CT>
CFGrammarTouple<CN, CT> parseGrammar(istream& input,
                                     AlphabetsPtr<CN, CT> alphabets)
{
	return {alphabets, parseLetterChar(input, *alphabets->N),
	        parseRules(input, alphabets)};
}

} // namespace context_free
