#pragma once
#include <string>
#include <include/nlohmann/json.hpp>

using namespace std;

class Json_tools {
public:

	using json = nlohmann::json;


	string map_to_json(std::map<std::string, std::string> data_map);
	nlohmann::json string_to_json(string jsonString);
};