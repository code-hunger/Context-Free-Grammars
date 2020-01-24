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

template <typename C, typename CPtrBox>
inline AlphabetToupleDistinct<C, StackBottomChar>
extendAlphabet(const std::shared_ptr<AlphabetLike<C, CPtrBox>> alphabet)
{

	AlphabetToupleDistinct<C, StackBottomChar> stackAlphabet{
	    alphabet,
	    std::make_shared<
	        SingletonAlphabet<StackBottomChar, const StackBottomChar*>>(
	        bottom)};
	return stackAlphabet;
}

template <typename CN, typename CT, typename CPtrBox>
auto grammarToAutomata(CFGrammarTouple<CN, CT, CPtrBox> const& grammar)
{
	AlphabetToupleDistinct stackAlphabet = extendAlphabet<
	    typename decltype(grammar.alphabets)::element_type::char_type,
	    typename decltype(grammar.alphabets)::element_type::char_box_type>(
	    grammar.alphabets);

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
	    std::make_shared<Push<CStack, CStackPtrBox>>(CN{*grammar.start}),
	    wild);

	for (auto const& rule : grammar.rules) {
		auto replacor = rule.to;
		std::string properlyTypedCopy;

		rule.to.for_each([&properlyTypedCopy](CPtrBox c) {
			std::ostringstream s;
			//decltype(c)::dada;
			const auto &x = *c;
			x.print(s);
			properlyTypedCopy += s.str();
		});

		auto realReplacor = AlphaString<CStack, CStackPtrBox>::parseString(
		    stackAlphabetPtr, properlyTypedCopy);

		wild.addTransition(
		    CharUnion<CN,CT>{rule.from}, std::nullopt,
		    std::make_shared<Replace<CStack, CStackPtrBox>>(
		        std::make_shared<AlphaString<CStack, CStackPtrBox>>(
		            realReplacor)),
		    wild);
	}

	auto popCmd = std::make_shared<Pop<CStack, CStackPtrBox>>();

	grammar.alphabets->T->for_each([&wild, &popCmd](CT const& c) mutable {
		wild.addTransition(CharUnion<CN,CT>{c}, c, popCmd, wild);
	});

	wild.addTransition(CStack{bottom}, {}, popCmd, accept);

	return Automata<CStack, CT, CStackPtrBox>{
	    stackAlphabetPtr, grammar.alphabets->T, std::move(meatBalls), start};
}

template <typename A1, typename A2> auto automataUnion(A1 const&, A2 const&) {}

} // namespace context_free
