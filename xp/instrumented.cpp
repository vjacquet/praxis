#include <algorithm>

#include "instrumented.h"

int xp::instrumented_base::counts[];
const char* xp::instrumented_base::names[] = {"default_construct", "construct", "copy_construct", "move_construct", "copy_assign", "move_assign", "equal", "compare", "destruct"};

void xp::instrumented_base::reset() {
	std::fill(counts, counts + n, 0);
}