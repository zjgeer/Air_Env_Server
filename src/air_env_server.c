#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "input_uart.h"
#include "input_uvc.h"
#include "output_tcp.h"


char *argv_output[] = {"output","192.168.8.2","7777"};
char *argv_input_uvc[] = {"input_uvc","m"};

void* thread_input_uart_function (void* thread_arg)
{
	printf("thread input uart work!\n");
	/* input�߳�Ҫ��ɵĹ��� */
	while(1)
	{
		input_uart();
		break;
	}
	pthread_exit("Exiting from the thread _function!");
}

void* thread_input_uvc_function (void* thread_arg)
{
	printf("thread input uvc work!\n");
	/* input�߳�Ҫ��ɵĹ��� */
	while(1)
	{
		// input_uvc(2,argv_input_uvc); 2015��5��19��21:44:02
		break;
	}
	pthread_exit("Exiting from the thread _function!");
}

void* thread_output_function (void* thread_arg)
{
	/* output�߳�Ҫ��ɵĹ��� */
	while(1)
	{
		output_tcp(3,argv_output);
		printf("thread output work!\n");
		break;
	}
	pthread_exit("Exiting from the thread _function!");
}
/******************************************************************************
Description.: ��Ӧ����Ctrl+C�����ն��źţ��ر�socket���رմ���������߳�
Input Value.: signal
Return Value: null
******************************************************************************/
void signal_handler(int sig)
{
	// �ر�socket
	close(s_s);
	// �رտͻ������ӵ�
	close(s_c);
	close(fp_database);
	printf("\n Exit the process! \n");
	exit(0);
}

int main(int argc, char *argv[])
{
	pthread_t thread_input_uart;
	pthread_t thread_input_uvc;
	pthread_t thread_output;

	pthread_create(&thread_input_uart, NULL, &thread_input_uart_function, NULL);
	pthread_create(&thread_input_uvc, NULL, &thread_input_uvc_function, NULL);
	pthread_create(&thread_output, NULL, &thread_output_function, NULL);
	
	pthread_detach(thread_input_uart);
	pthread_detach(thread_input_uvc);
	pthread_detach(thread_output);

	if (signal(SIGINT, signal_handler) == SIG_ERR)
	{
		printf("Exit with the Ctrl + C failuure!\n");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		/* ���߳����Ĺ��� */
		printf("Main thread work!\n");
		sleep(3);
	}
	return 0;
}