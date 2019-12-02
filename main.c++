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

auto createMyStates()
{
	using State = State<LetterChar, LetterChar>;
	using Command = StackCommand<LetterChar>;

	std::remove_const_t<decltype(Automata<LetterChar>::states)> states;

	states.emplace_front("p");
	auto& p = states.front();

	states.emplace_front("q");
	auto& q = states.front();

	states.emplace_front("f");

	auto sleep = std::make_shared<Sleep<LetterChar>>();

	p.transitions[std::make_pair('a', 'E')] =
	    std::vector<std::pair<std::shared_ptr<Command>, State*>>{
	        std::make_pair(sleep, &q), std::make_pair(sleep, &p)};

	return states;
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

	Automata<LetterChar> automata{alphabets, createMyStates()};

	std::cout << "Start: " << automata.start.human_name << std::endl;

	for (auto const& s : automata.states) {
		std::cout << "State: " << s.human_name << std::endl;
		s.printTransitions(std::cout);
	}

	std::cout << automata.readWord(parseString(std::cin, alphabets->T))
	          << std::endl;

	std::cout << "Bye!" << std::endl;

	return 0;
}
