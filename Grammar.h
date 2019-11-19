#include <memory>
#include <string>

struct Grammar
{
	virtual bool contains(std::string) const = 0;

	virtual std::unique_ptr<Grammar> union_with(Grammar const&) const = 0;
	virtual std::unique_ptr<Grammar> concat_with(Grammar const&) const = 0;
	virtual std::unique_ptr<Grammar> star() const = 0;

	virtual bool empty() const = 0;
	virtual bool infinite() const = 0;

	virtual ~Grammar() = default;
};
