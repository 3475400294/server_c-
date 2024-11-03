
#include "server/h/Webserver.h"
#include <type_traits> // conditional, is_same
#include <signal.h>


#define PROCESS_COUNT 4

int main() {
	
	// ���� SIGCHLD �źţ������ӽ��̳�Ϊ��ʬ����
	signal(SIGCHLD, SIG_IGN);
	Webserver webserver = Webserver();
	webserver.init_server();



	// ��������ӽ��̣�ÿ���ӽ��̵ȴ��ͻ�������
	for (int i = 0; i < PROCESS_COUNT; ++i)
	{
		pid_t pid = fork();
		if (pid == 0)
		{
			// �ӽ��̽��빤��ѭ��
			webserver.cycle();
			return 0;
		}
		else if (pid < 0)
		{
			std::cerr << "Failed to fork\n";
		}

	}
	
	// �����̹رռ����׽��ֲ��ȴ��ӽ�������
	webserver.close_();
	while (true)
	{
		pause(); // �����̱�������״̬
	}

	return 0;
	
};