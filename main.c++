#include "AlphaString.h"
#include "Automata.h"
#include "CFGrammar.h"
#include "CFParser.h"
#include "PDAlgorithms.h"

#include <iostream>

using namespace context_free;

auto toSharedAlphabet(std::string const& string)
{
	return std::make_shared<Alphabet<LetterChar>>(string);
}

auto createMeatBalls()
{
	std::remove_const_t<decltype(
	    Automata<LetterChar, LetterChar, const LetterChar*>::meatBalls)>
	    meatBalls;

	meatBalls.emplace_front("f");

	meatBalls.emplace_front("q");
	auto& q = meatBalls.front();

	meatBalls.emplace_front("p");
	auto& p = meatBalls.front();

	auto sleep = std::make_shared<Sleep<LetterChar, const LetterChar*>>();

	p.addTransition({'a'}, {'E'}, sleep, q);
	p.addTransition({'a'}, {'E'}, sleep, p);

	return meatBalls;
}

int main()
{
	std::string terminals = "abcd", variables = "SABCD";

	CFGrammarTouple grammar = parseGrammar(
	    std::cin,
	    std::make_shared<AlphabetToupleDistinct<LetterChar, LetterChar>>(
	        toSharedAlphabet(variables), toSharedAlphabet(terminals)));

	auto& alphabets = grammar.alphabets;

	std::cout << "Non-termianls: " << *alphabets->N << std::endl;
	std::cout << "Termianls: " << *alphabets->T << std::endl;

	for (auto& rule : grammar.rules) {
		std::cout << "The grammar has a rule from " << rule.from << " to "
		          << rule.to << std::endl;
	}

	//SingletonAlphabet<StackBottomChar, const StackBottomChar*>{ StackBottomChar{}};

	//std::make_shared<
		//SingletonAlphabet<StackBottomChar, const StackBottomChar*>>(
		//);

	auto automata = grammarToAutomata(grammar);

	std::cout << "Start: " << automata.start.human_name << std::endl;

	for (auto const& s : automata.meatBalls) {
		std::cout << "MeatBall: " << s.human_name << std::endl;
		s.printTransitions(std::cout);
	}

	while (!streamFinished(std::cin)) {
		auto word = parseString(std::cin, alphabets->T);
		auto reader = automata.createReader(word);

		int n = 0;
		bool found = false;
		if (!(std::cin >> n)) break;
		for (int i = 0; i < n; ++i) {
			const auto result = reader.advance();

			if (result) {
				found = true;
				std::cout << "WORD \"" << word << "\" RECOGNIZED!" << std::endl;
			}
		}

		if (!found)
			std::cout << "Word \"" << word << "\" not recognized." << std::endl;
	}

	std::cout << "Bye!" << std::endl;

	return 0;
}
