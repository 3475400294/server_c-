#pragma once

#include <sys/epoll.h>
#include <cstdio>
#include <server/h/Webserver.h>


const int MAX_EVENT_NUMBER = 10000; //最大事件数

class EPoll_tools
{
public:
	bool stop_server = false;
	int epoll_fd = epoll_create(5);
	epoll_event events[MAX_EVENT_NUMBER];


	Webserver web_server = Webserver();
	void cycle();
};