#pragma once

#include "Automata.h"
#include "CFGrammar.h"
#include <iostream>

namespace context_free {

struct StackBottomChar : Char
{
	const char value = '#';
	StackBottomChar() = default;

	StackBottomChar(char c)
	{
		if (c != '#')
			throw std::logic_error("A stack bottom char type must always be "
			                       "instantiated with a '#'");
	}

	void print(std::ostream& out) const override { out << "{#}"; }

	friend bool operator==(StackBottomChar, StackBottomChar) { return true; }
	virtual ~StackBottomChar(){};
};

bool operator<(StackBottomChar const&, StackBottomChar const&) { return false; }

const StackBottomChar bottom;

template <typename C>
inline AlphabetToupleDistinct<C, StackBottomChar>
extendAlphabet(const std::shared_ptr<AlphabetLike<C>> alphabet)
{

	AlphabetToupleDistinct<C, StackBottomChar> stackAlphabet{
	    alphabet,
	    std::make_shared<
	        SingletonAlphabet<StackBottomChar, const StackBottomChar*>>(
	        bottom)};
	return stackAlphabet;
}

template <typename CN, typename CT>
auto grammarToAutomata(CFGrammarTouple<CT, CN> const& grammar)
{
	AlphabetToupleDistinct stackAlphabet = extendAlphabet<typename decltype(
	    grammar.alphabets)::element_type::char_type>(grammar.alphabets);

	using CStack = typename decltype(stackAlphabet)::char_type;
	using CStackPtrBox = typename decltype(stackAlphabet)::char_box_type;
	using MeatBall = MeatBall<CStack, CT, CStackPtrBox>;

	shared_ptr stackAlphabetPtr =
	    std::make_shared<decltype(stackAlphabet)>(std::move(stackAlphabet));

	auto bottomPtr = stackAlphabet.T->findChar(StackBottomChar{'#'});
	auto& bottom = *bottomPtr;

	std::forward_list<MeatBall> meatBalls;
	meatBalls.emplace_front("start");
	auto& start = meatBalls.front();

	meatBalls.emplace_front("wild");
	auto& wild = meatBalls.front();

	meatBalls.emplace_front("final");
	auto& accept = meatBalls.front();

	start.addTransition(
	    CStack{bottom}, std::nullopt,
	    std::make_shared<Push<CStack, CStackPtrBox>>(CStack{*grammar.start}),
	    wild);

	for (auto const& rule : grammar.rules) {
		auto replacor = rule.to;
		std::string properlyTypedCopy;
		rule.to.for_each([&properlyTypedCopy](LetterChar const& c) {
			properlyTypedCopy.push_back(c.value);
		});

		auto realReplacor = AlphaString<CStack, CStackPtrBox>::parseString(
		    stackAlphabetPtr, properlyTypedCopy);

		wild.addTransition(
		    CStack{rule.from}, std::nullopt,
		    std::make_shared<Replace<CStack, CStackPtrBox>>(
		        std::make_shared<AlphaString<CStack, CStackPtrBox>>(
		            realReplacor)),
		    wild);
	}

	auto popCmd = std::make_shared<Pop<CStack, CStackPtrBox>>();

	grammar.alphabets->T->for_each([&wild, &popCmd](CT const& c) mutable {
		wild.addTransition(CStack{c}, c, popCmd, wild);
	});

	wild.addTransition(CStack{bottom}, {}, popCmd, accept);

	return Automata<CStack, CT, CStackPtrBox>{
	    stackAlphabetPtr, grammar.alphabets->T, std::move(meatBalls), start};
}

template <typename T> auto joinTwoContainers(T const& a, T const& b)
{
	T joined;
	for (auto& x : a) {
		joined.emplace_front(x);
	}
	for (auto& x : b) {
		joined.emplace_front(x);
	}
	return joined;
}

template <typename CStackA, typename CStackAPtrBox, typename CStackB,
          typename CStackBPtrBox, typename CTerminal>
auto automataUnion(Automata<CStackA, CTerminal, CStackAPtrBox> const& a,
                   Automata<CStackB, CTerminal, CStackBPtrBox> const& b)
{
	auto alphabet = AlphabetToupleDistinct<CStackA, CStackB>{*a.stackAlphabet,
	                                                         *b.stackAlphabet};

	using CStack = typename decltype(alphabet)::char_type;
	using CStackPtrBox = typename decltype(alphabet)::char_box_type;

	auto meatBalls = joinTwoContainers(a.meatBalls, b.meatBalls);

	meatBalls.emplace_front("Joined start");
	auto& newStart = meatBalls.front();

	newStart.addTransition(CStack{bottom}, std::nullopt,
	                       std::make_shared<Sleep<CStack, CStackPtrBox>>(),
	                       a.start);

	newStart.addTransition(CStack{bottom}, std::nullopt,
	                       std::make_shared<Sleep<CStack, CStackPtrBox>>(),
	                       b.start);

	return Automata{alphabet, a.wordAlphabet, meatBalls, &newStart};
}

template <typename CStackA, typename CStackAPtrBox, typename CStackB,
          typename CStackBPtrBox, typename CTerminal>
auto automataConcat(Automata<CStackA, CTerminal, CStackAPtrBox> const& a,
                    Automata<CStackB, CTerminal, CStackBPtrBox> const& b)
{
	auto alphabet = AlphabetToupleDistinct<CStackA, CStackB>{*a.stackAlphabet,
	                                                         *b.stackAlphabet};

	using CStack = typename decltype(alphabet)::char_type;
	using CStackPtrBox = typename decltype(alphabet)::char_box_type;

	auto meatBalls = joinTwoContainers(a.meatBalls, b.meatBalls);

	for (auto& s : a.finals) {
		s.addTransition(CStack{bottom}, std::nullopt,
		                std::make_shared<Sleep<CStack, CStackPtrBox>>(),
		                b.start);
	}

	return Automata{alphabet, a.wordAlphabet, meatBalls, &meatBalls.front()};
}

template <typename CStackA, typename CStackAPtrBox, typename CStackB,
          typename CStackBPtrBox, typename CTerminal>
auto automataIterate(Automata<CStackA, CTerminal, CStackAPtrBox> const& a)
{
	auto alphabet = a.stackAlphabet;
	using CStack = typename decltype(alphabet)::char_type;
	using CStackPtrBox = typename decltype(alphabet)::char_box_type;

	auto meatBalls{a.meatBalls};

	meatBalls.front().addTransition(CStack{bottom}, std::nullopt,
	                       std::make_shared<Sleep<CStack, CStackPtrBox>>(),
	                       meatBalls.front());


	for (auto& s : meatBalls) {
		if (s.isFinal)
			s.addTransition(CStack{bottom}, std::nullopt,
			                std::make_shared<Pop<CStack, CStackPtrBox>>(),
			                a.start);
	}

	return Automata{alphabet, a.wordAlphabet, meatBalls, &meatBalls.front()};
}

} // namespace context_free
