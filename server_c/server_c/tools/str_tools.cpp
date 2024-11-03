#include "tools/h/str_tools.h"





std::vector<std::string> Str_tools::splitString(const std::string& str, char c){

	std::vector<std::string> result;
	size_t start = 0;
	size_t end = str.find(c);

	while (end != std::string::npos) {
		result.push_back(str.substr(start, end));
		start = end + 1;
		end = str.find(c, start);
	}
	result.push_back(str.substr(start));
	return result;
}