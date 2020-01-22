#pragma once

#include <functional>
#include <memory>
#include <sstream>
#include <variant>

#include "Alphabet.h"

namespace context_free {

template <typename C1, typename C2> struct CharUnion
{
	const std::variant<C1, C2> value;

	CharUnion(std::variant<C1, C2> const& arg) : value(arg) {}

	CharUnion(C1 const& arg) : value(arg) {}

	void print(std::ostream& out) const
	{
		auto CharPrinter = [&out](const auto t) { t.print(out); };

		std::visit(CharPrinter, value);
	};

	bool operator==(CharUnion<C1, C2> const& other) const
	{
		return value == other.value;
	}

	operator bool() const
	{
		bool isEmpty = false;
		std::visit([&isEmpty](auto) { isEmpty = true; }, value);
		return isEmpty;
	}

	template <typename... Dummy> void customDelete()
	{
		throw std::runtime_error(
		    "Custom delete unsupported on this instantiation.");
	}

	template <typename... Dummy, typename U = C1, typename F = C2>
	std::enable_if_t<std::conjunction_v<std::is_pointer<U>, std::is_pointer<F>>,
	                 void>
	customDelete()
	{
		std::visit([](auto arg) { delete arg; }, value);
	}

	template <typename... Dummy, typename U = C1, typename F = C2>
	std::enable_if_t<std::conjunction_v<std::is_pointer<U>, std::is_pointer<F>>,
	                 CharUnion<std::remove_const_t<std::remove_pointer_t<U>>,
	                           std::remove_const_t<std::remove_pointer_t<F>>>>
	operator*() const
	{
		return std::visit(
		    [](auto arg) {
			    return std::variant<
			        std::remove_const_t<std::remove_pointer_t<U>>,
			        std::remove_const_t<std::remove_pointer_t<F>>>{*arg};
		    },
		    value);
	}

	template <typename P> auto visit(P predicate) const
	{
		return std::visit(predicate, value);
	}
};

template <typename C1, typename C2>
inline bool operator<(CharUnion<C1, C2> const& a, CharUnion<C1, C2> const& b)
{
	return a.value < b.value;
}

/*
 * A CharUnion makes no sense when C1 == C2.
 * Its only intended usage is as an argument ot AlphabetTouple<C1, C2> when C1
 * and C2 are different, otherwise AlphabetTouple uses a single C directly,
 * instead of `CharUnion<C, C>`.
 */
template <typename C> struct CharUnion<C, C>;

template <typename CN, typename CT = CN,
          typename CPtrBox = CharUnion<const CN*, const CT*>>
struct AlphabetTouple : public AlphabetLike<CN>,
                        public AlphabetLike<CT>,
                        public AlphabetLike<CharUnion<CN, CT>, CPtrBox>
{
	using char_type = CharUnion<CN, CT>;
	using char_box_type = CPtrBox;

	const std::shared_ptr<AlphabetLike<CN>> N;
	const std::shared_ptr<AlphabetLike<CT>> T;

	const CN* findChar(CN const& c) const override { return N->findChar(c); };

	const CT* findChar(CT const& c) const override { return T->findChar(c); };

	bool all_of(std::function<bool(CN const&)> const& f) const override
	{
		return N->all_of(f);
	};

	void for_each(std::function<void(CN const&)> const& f) const override
	{
		N->for_each(f);
	};

	bool all_of(std::function<bool(CT const&)> const& f) const override
	{
		return T->all_of(f);
	};

	void for_each(std::function<void(CT const&)> const& f) const override
	{
		T->for_each(f);
	};

	bool all_of(std::function<bool(CharUnion<CN, CT> const&)> const& predicate)
	    const override
	{
		auto forN = [&predicate](CN const& arg) {
			return predicate(char_type{arg});
		};
		auto forT = [&predicate](CT const& arg) {
			return predicate(char_type{arg});
		};
		return N->all_of(forN) && T->all_of(forT);
	}

	void for_each(std::function<void(CharUnion<CN, CT> const&)> const&
	                  predicate) const override
	{
		auto forN = [&predicate](CN const& arg) {
			return predicate(char_type{arg});
		};
		auto forT = [&predicate](CT const& arg) {
			return predicate(char_type{arg});
		};
		N->for_each(forN);
		T->for_each(forT);
	}

	CPtrBox findChar(CharUnion<CN, CT> const& c) const override
	{
		return c.visit([this](auto c) -> CPtrBox {
			return CPtrBox{this->findChar(c)};
		});
	}

	AlphabetTouple(decltype(N) N, decltype(T) T) : N(N), T(T) {}
};

template <typename C, typename CPtrBox>
struct AlphabetTouple<C, C, CPtrBox> : public AlphabetLike<C, CPtrBox>
{
	const std::shared_ptr<AlphabetLike<C>> N, T;

	CPtrBox findChar(C const& c) const override
	{
		// c is guaranteed to be at most in one of N and T.
		if (auto a = N->findChar(c)) return a;
		return T->findChar(c);
	}

	bool all_of(std::function<bool(C const&)> const& predicate) const override
	{
		return N->all_of(predicate) && T->all_of(predicate);
	}

	void for_each(std::function<void(C const&)> const& predicate) const override
	{
		N->for_each(predicate);
		T->for_each(predicate);
	}

	AlphabetTouple(decltype(N) N, decltype(T) T) : N(N), T(T) {}
};

template <typename CN, typename CT>
struct AlphabetToupleDistinct : public AlphabetTouple<CN, CT>
{
	/*
	 * A touple of 2 alphabets of different Char types is trivially distinct,
	 * no need for any checks.
	 */
	using parent = AlphabetTouple<CN, CT, CharUnion<const CN*, const CT*>>;

	AlphabetToupleDistinct(decltype(parent::N) N, decltype(parent::T) T)
	    : parent(N, T)
	{
	}
};

template <typename C, typename CPtrBox>
bool pairwiseDistinct(AlphabetLike<C, CPtrBox> const& A,
                      AlphabetLike<C, CPtrBox> const& B)
{
	return A.all_of([&B](C const& c) { return !B.findChar(c); });
}

template <typename C>
struct AlphabetToupleDistinct<C, C> : public AlphabetTouple<C, C, const C*>
{
	/*
	 * A touple of 2 alphabets of the same Char type must be manually checked
	 * for uniqueness of the elemnets of the 2 alphabets.
	 */
	using parent = AlphabetTouple<C, C, const C*>;

	AlphabetToupleDistinct(decltype(parent::N) N, decltype(parent::T) T)
	    : parent(N, T)
	{
		if (!pairwiseDistinct(*N, *T))
			throw std::invalid_argument(
			    "The two alphabets must be pairwise distinct.");
	}
};

} // namespace context_free
