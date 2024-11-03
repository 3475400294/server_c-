#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <typeinfo>
//#include <unordered_map>

#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <tools/h/json_tools.h>
#include <tools/h/str_tools.h>


#define MAX_EVENTS 10

using namespace std;

#pragma warning(disable:4996)



class Webserver {

public:
	int port = 8222;
	int server_fd;
	

	Json_tools json_tools;
	Str_tools str_tools;

	std::unordered_map<int, string> client_id_map;         // 保存post方法里面的data
	std::unordered_map<int, int> client_id_len_map;        // 保存post方法里面的data的长度
	std::unordered_map<int, string> client_id_method_map;  // 保存请求里面的方法
	std::unordered_map<int, string> client_id_url_map;     // 保存请求里面的url
	std::unordered_map<int, string> client_id_params_map;  // 保存请求里面的参数
	std::unordered_map<int, std::map<string, string>> client_id_params_map_map; // 保存请求里面的参数解析后
	std::unordered_map<int, bool> client_id_HTTP_map;       // 保存请求url后面的参数是否拿完，拿完的话，会出现HTTP/
	
	string get_client_id_map(int);
	string get_client_id_method_map(int);

	void cycle();

	void init_server();
	int wait_request();
	void disconnect(int epoll_fd, int client_fd);
	void send_data_pending(int client_fd);
	void send_data_get(int client_fd);
	void send_data_post(int client_fd);
	void close_();
};