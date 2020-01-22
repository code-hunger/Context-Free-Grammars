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

	std::map<TransitionFrom, TransitionTo> transitions{};

	void
	addTransition(std::optional<CStack> stackC, std::optional<CTerminal> wordC,
	              std::shared_ptr<StackCommand<CStack, CStackPtrBox>> command,
	              MeatBall& target)
	{
		transitions[make_pair(stackC, wordC)].push_back(
		    make_pair(command, &target));
	}

	auto& next(Stack<CStack, CStackPtrBox> const& stack,
	           CTerminal charToRead) const
	{
		static typename decltype(transitions)::mapped_type emptyTransition = {};

		auto search_for = std::make_pair(
		    std::make_optional<CStack>(
		        **stack.top()), // top() returns CPtrBox, not just C
		    std::make_optional<CTerminal>(charToRead));

		auto x = transitions.find(TransitionFrom{search_for});

		return x != transitions.end() ? x->second : emptyTransition;
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
