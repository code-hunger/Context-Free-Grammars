#pragma once

#include "Automata.h"
#include "CFGrammar.h"

namespace context_free {

struct StackBottomChar : Char
{
	StackBottomChar() = default;

	StackBottomChar(char c)
	{
		if (c != '#')
			throw std::logic_error("A stack bottom char type must always be "
			                       "instantiated with a '#'");
	}

	void print(std::ostream& out) const override { out << "{#}"; }

	friend bool operator==(StackBottomChar, StackBottomChar) { return true; }
};

template <typename C>
inline static auto
extendAlphabet(const std::shared_ptr<AlphabetLike<C>> alphabet)
{
	StackBottomChar bottom;
	std::shared_ptr<AlphabetLike<StackBottomChar>> singletonAlphabet =
	    std::make_shared<Alphabet<StackBottomChar>>("#");

	AlphabetToupleDistinct<C, StackBottomChar> stackAlphabet{alphabet,
	                                                         singletonAlphabet};
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

	auto bottomPtr = stackAlphabet.T->findChar(StackBottomChar{'#'});
	auto bottom = StackBottomChar{'#'};

	std::forward_list<MeatBall> meatBalls;
	meatBalls.emplace_front("start");
	auto& start = meatBalls.front();

	meatBalls.emplace_front("wild");
	auto& wild = meatBalls.front();

	meatBalls.emplace_front("final");
	auto& accept = meatBalls.front();

	start.addTransition(
	    CStack{bottomPtr}, std::nullopt,
	    std::make_shared<Push<CStack, CStack>>(CStack{grammar.start}), wild);

	for (auto const& rule : grammar.rules) {
		wild.addTransition(CStack{stackAlphabet.findChar(rule.from)},
		                   std::nullopt,
		                   std::make_shared<Replace<CStack, CStackPtrBox>>(
		                       CStack{rule.to.string[0]}),
		                   wild);
	}

	auto popCmd = std::make_shared<Pop<CStack, CStackPtrBox>>();

	grammar.alphabets->T->for_each([&wild, &popCmd](CT const& c) mutable {
		wild.addTransition(CStack{&c}, c, popCmd, wild);
	});

	wild.addTransition(CStack{bottomPtr}, {}, popCmd, accept);

	return Automata<CStack, CT, CStackPtrBox>{
	    std::make_shared<decltype(stackAlphabet)>(std::move(stackAlphabet)),
	    grammar.alphabets->T, std::move(meatBalls), start};
}

template <typename A1, typename A2> auto automataUnion(A1 const&, A2 const&) {}

} // namespace context_free
