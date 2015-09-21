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
	/* input线程要完成的工作 */
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
	/* input线程要完成的工作 */
	while(1)
	{
		// input_uvc(2,argv_input_uvc); 2015年5月19日21:44:02
		break;
	}
	pthread_exit("Exiting from the thread _function!");
}

void* thread_output_function (void* thread_arg)
{
	/* output线程要完成的工作 */
	while(1)
	{
		output_tcp(3,argv_output);
		printf("thread output work!\n");
		break;
	}
	pthread_exit("Exiting from the thread _function!");
}
/******************************************************************************
Description.: 相应按键Ctrl+C按键终端信号，关闭socket，关闭创建的相关线程
Input Value.: signal
Return Value: null
******************************************************************************/
void signal_handler(int sig)
{
	// 关闭socket
	close(s_s);
	// 关闭客户端连接的
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
		/* 主线程做的工作 */
		printf("Main thread work!\n");
		sleep(3);
	}
	return 0;
}