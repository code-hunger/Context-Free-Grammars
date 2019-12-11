#include "AlphaString.h"
#include "Automata.h"
#include "CFGrammar.h"
#include "CFParser.h"

#include <iostream>

using namespace context_free;

auto toSharedAlphabet(std::string const& string)
{
	return std::make_shared<Alphabet<LetterChar>>(string);
}

auto createMeatBalls()
{
	using Command = StackCommand<LetterChar>;

	std::remove_const_t<decltype(Automata<LetterChar>::meatBalls)> meatBalls;

	meatBalls.emplace_front("f");

	meatBalls.emplace_front("q");
	auto& q = meatBalls.front();

	meatBalls.emplace_front("p");
	auto& p = meatBalls.front();

	auto sleep = std::make_shared<Sleep<LetterChar>>();

	p.transitions[std::make_pair('a', 'E')] = std::vector<
	    std::pair<std::shared_ptr<Command>, MeatBall<LetterChar, LetterChar>*>>{
	    std::make_pair(sleep, &q), std::make_pair(sleep, &p)};

	return meatBalls;
}

int main()
{
	std::string terminals = "abcdefghijklmnopqrstuvwxyz",
	            variables = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	CFGrammarTouple grammar = parseGrammar(
	    std::cin,
	    std::make_shared<AlphabetToupleDistinct<LetterChar>>(
	        toSharedAlphabet(variables), toSharedAlphabet(terminals)));

	auto& alphabets = grammar.alphabets;

	std::cout << "Non-termianls: " << *alphabets->N << std::endl;
	std::cout << "Termianls: " << *alphabets->T << std::endl;

	for (auto& rule : grammar.rules) {
		std::cout << "The grammar has a rule from " << rule.from << " to "
		          << rule.to << std::endl;
	}

	Automata<LetterChar> automata{alphabets->N, alphabets->T, createMeatBalls()};

	std::cout << "Start: " << automata.start.human_name << std::endl;

	for (auto const& s : automata.meatBalls) {
		std::cout << "MeatBall: " << s.human_name << std::endl;
		s.printTransitions(std::cout);
	}

	while (!streamFinished(std::cin))
		std::cout << automata.readWord(parseString(std::cin, alphabets->T))
		          << std::endl;

	std::cout << "Bye!" << std::endl;

	return 0;
}
