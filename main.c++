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

int main()
{
	std::string terminals = "abcdefghijklmnopqrstuvwxyz",
	            variables = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	CFGrammarTouple grammar =
	    parseGrammar(std::cin, std::make_shared<AlphabetTouple<LetterChar>>(
	                               toSharedAlphabet(variables),
	                               toSharedAlphabet(terminals)));

	auto& alphabets = grammar.alphabets;

	std::cout << "Non-termianls: " << *alphabets->N << std::endl;
	std::cout << "Termianls: " << *alphabets->T << std::endl;

	for (auto& rule : grammar.rules) {
		std::cout << "The grammar has a rule from " << rule.from << " to "
		          << rule.to << std::endl;
	}

	using State = State<LetterChar, LetterChar>;
	using Stack = Stack<LetterChar>;
	using Command = Stack::Command;
	State p{"p"};
	State q{"q"};
	State f{"f"};

	p.transitions[std::make_pair('a', 'E')] =
	    std::vector<std::pair<Command, State*>>{
	        std::make_pair<Command, State*>(Stack::Sleep{}, &q),
	        std::make_pair<Command, State*>(Stack::Sleep{}, &p)};

	Automata<LetterChar> automata{{p, q, f}};

	automata.states[0].printTransitions(std::cout);

	std::cout << "Bye!" << std::endl;

	return 0;
}
