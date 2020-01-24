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

template <typename CN, typename CT>
auto readGrammarFromInput(std::shared_ptr<AlphabetLike<CN>> variables,
                          std::shared_ptr<AlphabetLike<CT>> terminals)
{
	CFGrammarTouple grammar = parseGrammar(
	    std::cin,
	    std::make_shared<AlphabetToupleDistinct<LetterChar, LetterChar>>(
	        variables, terminals));

	std::cout << "Non-termianls: " << *grammar.alphabets->N << std::endl;
	std::cout << "Termianls: " << *grammar.alphabets->T << std::endl;

	for (auto& rule : grammar.rules) {
		std::cout << "The grammar has a rule from " << rule.from << " to "
		          << rule.to << std::endl;
	}

	return grammar;
}

int main()
{
	const std::string variables1 = "SABCD";
	const std::string variables2 = "EFGH";

	auto termAlph = toSharedAlphabet("abcde");

	const auto a1 =
	    grammarToAutomata(readGrammarFromInput<LetterChar, LetterChar>(
	        toSharedAlphabet(variables1), termAlph));
	/*const auto a2 =
	    grammarToAutomata(readGrammarFromInput<LetterChar, LetterChar>(
			toSharedAlphabet(variables2), termAlph));*/

	auto& automata = a1;
	// auto joined = automataUnion(a1, a2);

	std::cout << "Start: " << automata.start.human_name << std::endl;

	for (auto const& s : automata.meatBalls) {
		std::cout << "MeatBall: " << s.human_name << std::endl;
		s.printTransitions(std::cout);
	}

	std::cout << "I'll try to read words now!" << std::endl;

	int n;
	if (!(std::cin >> n)) {
		throw std::runtime_error("fack");
	}

	while (!streamFinished(std::cin)) {
		auto word = parseString(std::cin, termAlph);

		std::cout << "From input I got '";
		word.print(std::cout);
		std::cout << "'" << std::endl;

		auto reader = automata.createReader(
		    word, decltype(automata.stackAlphabet)::element_type::char_type{
		              context_free::bottom});

		bool found = false;

		for (int i = 0; i < n; ++i) {
			std::cout << "Advancing…" << i << '\r';

			const auto result = reader.advance();

			if (result.finished) {
				found = result.acceptingState.has_value();
				break;
			}
		}

		std::cout << "\n\"" << word << "\" : " << std::boolalpha << found
		          << std::endl;
	}

	std::cout << "Bye!" << std::endl;

	return 0;
}
