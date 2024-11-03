
#include "server/h/Webserver.h"
#include <type_traits> // conditional, is_same
#include <signal.h>


#define PROCESS_COUNT 4

int main() {
	
	// 忽略 SIGCHLD 信号，避免子进程成为僵尸进程
	signal(SIGCHLD, SIG_IGN);
	Webserver webserver = Webserver();
	webserver.init_server();



	// 创建多个子进程，每个子进程等待客户端连接
	for (int i = 0; i < PROCESS_COUNT; ++i)
	{
		pid_t pid = fork();
		if (pid == 0)
		{
			// 子进程进入工作循环
			webserver.cycle();
			return 0;
		}
		else if (pid < 0)
		{
			std::cerr << "Failed to fork\n";
		}

	}
	
	// 父进程关闭监听套接字并等待子进程运行
	webserver.close_();
	while (true)
	{
		pause(); // 父进程保持运行状态
	}

	return 0;
	
};