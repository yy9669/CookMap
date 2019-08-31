#include "Load.hpp"

#include <array>
#include <list>
#include <cassert>

namespace {
	std::array< std::list< std::function< void() > >, MaxLoadTag > &get_load_lists() {
		static std::array< std::list< std::function< void() > >, MaxLoadTag > load_lists;
		return load_lists;
	}
}

void add_load_function(LoadTag tag, std::function< void() > const &fn) {
	auto &load_lists = get_load_lists();
	assert(tag < load_lists.size());
	load_lists[tag].emplace_back(fn);
}

void call_load_functions() {
	static bool has_been_called = false;
	assert(!has_been_called && "call_load_functions should only be called *once*");
	has_been_called = true;

	auto &load_lists = get_load_lists();
	for (auto &fn_list : load_lists) {
		while (!fn_list.empty()) {
			(*fn_list.begin())(); //call first function in the list
			fn_list.pop_front(); //remove from list
		}
	}
}
