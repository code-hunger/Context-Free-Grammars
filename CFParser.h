#pragma once

#include "CFGrammar.h"
#include <iostream>
#include <memory>
#include <ostream>

namespace context_free {

using std::istream;

template<typename C = LetterChar>
AlphaString<C>
parseString(istream& in, std::shared_ptr<AlphabetLike<C>> alphabet)
{
	std::string string;
	in >> string;

	return AlphaString<C>::parseString(alphabet, string);
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

template <typename CN, typename CT, typename CPtrBox>
Rule<CN, CT, CPtrBox> parseRule(istream& input, AlphabetsPtr<CN, CT> alphabets)
{
	return {parseLetterChar(input, *alphabets->N),
	        parseString<decltype(alphabets)::element_type::char_type>(input, alphabets)};
}

bool streamFinished(istream& input)
{
	input >> std::ws;
	return input.eof();
}

template <typename CN, typename CT, typename CPtrBox>
auto parseRules(istream& input, AlphabetsPtr<CN, CT> alphabets)
{
	std::vector<Rule<CN, CT, CPtrBox>> rules;

	while (input && !streamFinished(input)) {
		try {
			rules.push_back(parseRule<CN, CT, CPtrBox>(input, alphabets));
		} catch (std::exception const& e) {
			std::cerr << "Failed to parse a rule. Error: " << std::endl
			          << e.what() << std::endl
			          << "Won't read for more rules.\n" << std::endl;
			break;
		}
	}

	return rules;
}

template <typename CN, typename CT, typename CPtrBox>
CFGrammarTouple<CN, CT, CPtrBox> parseGrammar(istream& input,
                                     AlphabetsPtr<CN, CT> alphabets)
{
	return {alphabets, parseLetterChar(input, *alphabets->N),
	        parseRules<CN, CT, CPtrBox>(input, alphabets)};
}

} // namespace context_free
