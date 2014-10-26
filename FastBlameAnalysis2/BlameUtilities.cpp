#include "BlameUtilities.h"

std::istream& operator>>(std::istream& input, DebugInfo& dbg) {
	std::string _;
	input >> _ >> dbg.file >> _ >> dbg.line >> _ >> dbg.column;
	return input;
}
