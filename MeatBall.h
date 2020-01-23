#pragma once

#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

#include "StackCommands.h"

namespace context_free {

template <typename CStack, typename CTerminal, typename CStackPtrBox>
struct MeatBall
{
	const std::string human_name;

	MeatBall(std::string name) : human_name(name) {}
	MeatBall(MeatBall&&) =
	    delete; //{ throw std::runtime_error("Should never happen"); }; //
	            // Needed if the automata uses std::vector for storing MeatBalls
	MeatBall(MeatBall const&) = delete;

	using TransitionFrom =
	    std::pair<std::optional<CStack>, std::optional<CTerminal>>;
	using TransitionTo = std::vector<std::pair<
	    std::shared_ptr<StackCommand<CStack, CStackPtrBox>>, MeatBall*>>;

	// The imperative logic for constructing MeatBalls requires me to make it
	// non-const. I don't think it'd be beautiful otherwise.
	std::map<TransitionFrom, TransitionTo> transitions{};

	void
	addTransition(std::optional<CStack> stackC, std::optional<CTerminal> wordC,
	              std::shared_ptr<StackCommand<CStack, CStackPtrBox>> command,
	              MeatBall& target)
	{
		transitions[make_pair(stackC, wordC)].push_back(
		    make_pair(command, &target));
	}

	auto next(Stack<CStack, CStackPtrBox> const& stack,
	          std::optional<CTerminal> charToRead) const
	{
		static typename decltype(transitions)::mapped_type emptyTransition = {};

		auto search = std::make_pair(stack.top(), charToRead);

		auto possibleMoves = transitions.find(TransitionFrom{search});

		return possibleMoves == transitions.end() ? emptyTransition
		                                          : possibleMoves->second;
	}

	template <typename C>
	static void printOr(std::ostream& out, std::optional<C> const& c,
	                    std::string if_empty)
	{
		if (c.has_value())
			c->print(out);
		else
			out << if_empty;
	}

	void printTransitions(std::ostream& out) const
	{
		for (auto const& [from, to] : transitions) {
			for (auto const& [command, targetMeatBall] : to) {
				out << " | ";
				printOr(out, from.first, "-");
				out << ", ";
				printOr(out, from.second, "{eps}");
				out << " --> ";
				command->print(out);
				out << " -> " << targetMeatBall->human_name << std::endl;
			}
		}
	}
};

} // namespace context_free
