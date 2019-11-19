#include "AlphaString.h"

#include <iostream>

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

