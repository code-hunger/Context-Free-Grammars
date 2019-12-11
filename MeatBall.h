#pragma once

#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

#include "StackCommands.h"

namespace context_free {

template <typename CTerminal, typename CStack> struct MeatBall
{
	const std::string human_name;

	MeatBall(std::string name) : human_name(name) {}
	MeatBall(MeatBall&&) =
	    delete; //{ throw std::runtime_error("Should never happen"); }; //
	            // Needed if the automata uses std::vector for storing MeatBalls
	MeatBall(MeatBall const&) = delete;

	std::map<
	    std::pair<std::optional<CStack>, CTerminal>,
	    std::vector<std::pair<std::shared_ptr<StackCommand<CStack>>, MeatBall*>>>
	    transitions{};

	auto& next(Stack<CStack> const& stack, CTerminal charToRead) const
	{
		static typename decltype(transitions)::mapped_type emptyTransition = {};
		auto x = transitions.find(std::make_pair(stack.top(), charToRead));

		return x != transitions.end() ? x->second : emptyTransition;
	}

	static void printOrMissing(std::ostream& out,
	                           std::optional<CStack> const& c)
	{
		out << (c.has_value() ? *c : CStack{'-'});
	}

	void printTransitions(std::ostream& out) const
	{
		for (auto const& [from, to] : transitions) {
			for (auto const& [command, targetMeatBall] : to) {
				out << " | ";
				printOrMissing(out, from.first);
				out << ", " << from.second << " --> ";
				command->print(out);
				out << " -> " << targetMeatBall->human_name << std::endl;
			}
		}
	}
};

} // namespace context_free