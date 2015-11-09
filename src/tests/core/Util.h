#pragma once

#include <list>
#include <vector>
#include <boost/variant.hpp>

template<typename... T>
using ComparisonList = std::vector<boost::variant<T...>>;

template<typename Iterator>
struct ComparisonOperator {
	typedef void result_type;
	Iterator it1, it2;
	void set(const Iterator& i1, const Iterator& i2) {
		it1 = i1;
		it2 = i2;
	}
	template<typename T1, typename T2>
	void operator()(const T1& t1, const T2& t2) {
		EXPECT_EQ(it1 < it2, t1 < t2) << t1 << " < " << t2;
		EXPECT_EQ(it1 > it2, t1 > t2) << t1 << " > " << t2;
		EXPECT_EQ(it1 <= it2, t1 <= t2) << t1 << " <= " << t2;
		EXPECT_EQ(it1 >= it2, t1 >= t2) << t1 << " >= " << t2;
		EXPECT_EQ(it1 == it2, t1 == t2) << t1 << " == " << t2;
		EXPECT_EQ(it1 != it2, t1 != t2) << t1 << " != " << t2;
	}
};

template<typename List>
void expectRightOrder(const List& list) {
	ComparisonOperator<typename List::const_iterator> c;
	for (auto it1 = list.begin(); it1 != list.end(); it1++) {
		for (auto it2 = list.begin(); it2 != list.end(); it2++) {
			c.set(it1, it2);
			boost::apply_visitor(c, *it1, *it2);
		}
	}
}