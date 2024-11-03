#include "tools/h/epoll_tools.h"




void EPoll_tools::cycle()
{

	while (!stop_server) {


        int event_count = epoll_wait(epoll_fd, events, MAX_EVENT_NUMBER, -1);
        if (event_count == -1) {
            printf("epoll_wait Error！\n");
            break;
        }
        for (int i = 0; i < event_count; i++)
        {   
             // 如果第一次连接过来
            if (events[i].data.fd == web_server.server_fd)
            {


                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);
            }


        }
	}
}
