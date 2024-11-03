#include "tools/h/json_tools.h"




string Json_tools::map_to_json(std::map<std::string, std::string> data_map)
{

	json json_data = data_map;


	std::string json_str = json_data.dump();
	return json_str;
}



nlohmann::json Json_tools::string_to_json(string jsonString)
{

	nlohmann::json jsonObj = nlohmann::json::parse(jsonString);

	return jsonObj;
}


