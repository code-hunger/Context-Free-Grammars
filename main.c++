#include "AlphaString.h"
#include "CFGrammar.h"
#include "CFParser.h"

#include <iostream>

using namespace context_free;

auto toSharedAlphabet(std::string const& string)
{
	return std::make_shared<Alphabet<LetterChar>>(string);
}

int main()
{
	std::string terminals = "abcdefghijklmnopqrstuvwxyz",
	            variables = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	auto another =
	    parseGrammar(std::cin, std::make_shared<AlphabetTouple<LetterChar>>(
	                               toSharedAlphabet(variables),
	                               toSharedAlphabet(terminals)));

	another.alphabets->N->print(std::cout);
	another.alphabets->T->print(std::cout);

	std::cout << "\nBye!" << std::endl;

	return 0;
}
