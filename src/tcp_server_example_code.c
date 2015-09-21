
/** �ͻ��˷���TIME,���������պ���ʱ�䣬�رտͻ��� */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

#include "output_tcp.h"

#define BUFFLEN 1024
#define SERVER_PORT 7777
#define BACKLOG 5

void *run(void *arg)
{
	return NULL;
}


/* argc�����в������������argv�洢�����б������� */
int output_tcp(int argc, char *argv[])
{
	/** �������׽����ļ������� */
	int s_s,s_c;
	/** ���ص�ַ */
	struct sockaddr_in local,from;
	time_t now;
	char buff[BUFFLEN];
	int n = 0;
	int len =sizeof(from);
	
	/** ����TCP�׽��� */
	s_s = socket( AF_INET, SOCK_STREAM, 0);
	/** ��ʼ����ַ */
	memset(&local, 0,sizeof(local));  // ����
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(SERVER_PORT);
	
	/** ���׽����ļ��������󶨵����ص�ַ�Ͷ˿� */
	int err = bind(s_s, (struct sockaddr *)&local,sizeof(local));
	err = listen(s_s, BACKLOG);
	
	while(1)
	{
		/** ���տͻ������� */
		s_c = accept(s_s,(struct sockaddr *)&from,&len);
		memset(buff, 0, BUFFLEN);
		n = recv(s_c, buff, BUFFLEN, 0);
		if(n > 0 && !strncmp(buff,"TIME",4))
		{
			memset(buff, 0,BUFFLEN);
			now = time(NULL);
			sprintf(buff,"%24s\r\n",ctime(&now));
			printf("Send data:%s",buff);
			send(s_c,buff,strlen(buff),0);
		}
		close(s_c);
	}
	close(s_s);
    return 0;
}


/** ʵ�ֶ���ͻ��˵����� */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

#include "output_tcp.h"

#define BUFFLEN 1024
#define SERVER_PORT 7777
#define BACKLOG 5

void *handle_request(void *arg)
{
	int s_c = *((int*)arg);
	time_t now;
	char buff[BUFFLEN];
	int n = 0;
	
	memset(buff,0,BUFFLEN);
	n = recv(s_c, buff,BUFFLEN,0);
	if(n > 0 && !strncmp(buff,"TIME",4))
	{
		memset(buff,0,BUFFLEN);
		now = time(NULL);
		sprintf(buff,"%24s\r\n",ctime(&now));
		printf("Send data:%s",buff);
		send(s_c,buff,strlen(buff),0);
	}
	close(s_c);
	return NULL;
}

static void handle_connect(int s_s)
{
	int s_c;
	struct sockaddr_in from;
	int len = sizeof(from);
	pthread_t thread_do;
	
	while(1)
	{
		s_c = accept(s_s,(struct sockaddr*)&from,&len);
		if(s_c > 0)
		{
			int err = pthread_create(&thread_do,NULL,handle_request,(void*)&s_c);
			pthread_detach(thread_do);
		}
	}
}


/* argc�����в������������argv�洢�����б������� */
int output_tcp(int argc, char *argv[])
{
	/** �������׽����ļ������� */
	int s_s,s_c;
	/** ���ص�ַ */
	struct sockaddr_in local,from;
	time_t now;
	char buff[BUFFLEN];
	int n = 0;
	int len =sizeof(from);
	
	/** ����TCP�׽��� */
	s_s = socket( AF_INET, SOCK_STREAM, 0);
	/** ��ʼ����ַ */
	memset(&local, 0,sizeof(local));  // ����
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(SERVER_PORT);
	
	/** ���׽����ļ��������󶨵����ص�ַ�Ͷ˿� */
	int err = bind(s_s, (struct sockaddr *)&local,sizeof(local));
	err = listen(s_s, BACKLOG);
	
	// while(1)
	// {
		// /** ���տͻ������� */
		// s_c = accept(s_s,(struct sockaddr *)&from,&len);
		// memset(buff, 0, BUFFLEN);
		// n = recv(s_c, buff, BUFFLEN, 0);
		// if(n > 0 && !strncmp(buff,"TIME",4))
		// {
			// memset(buff, 0,BUFFLEN);
			// now = time(NULL);
			// sprintf(buff,"%24s\r\n",ctime(&now));
			// printf("Send data:%s",buff);
			// send(s_c,buff,strlen(buff),0);
		// }
		// close(s_c);
	// }
	handle_connect(s_s);
	close(s_s);
    return 0;
}





#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

#include "output_tcp.h"

#define BUFFLEN 1024
#define SERVER_PORT 7777
#define BACKLOG 5
#define CLIENTNUM 2

/** ������ */
pthread_mutex_t ALOCK = PTHREAD_MUTEX_INITIALIZER;

static void *handle_request(void *arg)
{
	int s_s = *((int*)arg);
	int s_c;
	struct sockaddr_in from;
	int len = sizeof(from);
	
	for(;;)
	{
		time_t now;
		char buff[BUFFLEN];
		int n = 0;
		pthread_mutex_lock(&ALOCK);
		
		s_c = accept(s_s,(struct sockaddr*)&from,&len);
		
		memset(buff,0,BUFFLEN);
		n = recv(s_c,buff,BUFFLEN,0);
		if(n > 0 && !strncmp(buff,"TIME",4))
		{
			memset(buff,0,BUFFLEN);
			now = time(NULL);
			sprintf(buff,"%24s\r\n",ctime(&now));
			printf("Send data:%s",buff);
			send(s_c,buff,strlen(buff),0);
		}

	}
	close(s_c);
	return NULL;
}

static void handle_connect(int s)
{
	int s_s = s;
	pthread_t thread_do[CLIENTNUM];
	int i = 0;
	
	for(i = 0;i < CLIENTNUM;i++)
	{
		pthread_create(&thread_do[i],
		NULL,
		handle_request,
		(void*)&s_s);
	}
	for(i = 0;i < CLIENTNUM;i++)
		pthread_join(thread_do[i],NULL);
}


/* argc�����в������������argv�洢�����б������� */
int output_tcp(int argc, char *argv[])
{
	/** �������׽����ļ������� */
	int s_s;
	
	/** ���ص�ַ */
	struct sockaddr_in local;
	
	/** ����TCP�׽��� */
	s_s = socket( AF_INET, SOCK_STREAM, 0);
	/** ��ʼ����ַ */
	memset(&local, 0,sizeof(local));  // ����
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(SERVER_PORT);
	
	/** ���׽����ļ��������󶨵����ص�ַ�Ͷ˿� */
	int err = bind(s_s, (struct sockaddr *)&local,sizeof(local));
	err = listen(s_s, BACKLOG);
	
	handle_connect(s_s);
	
	close(s_s);
	
    return 0;
}




/** ʵ�ֶ���ͻ��˵�����,����ͻ���ͬһ���߳� */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

#include "output_tcp.h"

#define BUFFLEN 1024
#define SERVER_PORT 7777
#define BACKLOG 5

void *handle_request(void *arg)
{
	int s_c = *((int*)arg);
	time_t now;
	char buff[BUFFLEN];
	int n = 0;
	
	while(1)
	{
		memset(buff,0,BUFFLEN);
		n = recv(s_c, buff,BUFFLEN,0);
		if(n > 0 && !strncmp(buff,"TIME",4))
		{
			memset(buff,0,BUFFLEN);
			now = time(NULL);
			sprintf(buff,"%24s\r\n",ctime(&now));
			printf("Send data:%s",buff);
			send(s_c,buff,strlen(buff),0);
		}
		else
		{
			close(s_c);
			return NULL;
		}
		printf("One client one thread,thread ID %d\n", pthread_self());
	}
}

static void handle_connect(int s_s)
{
	int s_c;
	struct sockaddr_in from;
	int len = sizeof(from);
	pthread_t thread_do;
	
	while(1)
	{
		s_c = accept(s_s,(struct sockaddr*)&from,&len);
		if(s_c > 0)
		{
			int err = pthread_create(&thread_do,NULL,handle_request,(void*)&s_c);
			pthread_detach(thread_do);
		}
	}
}


/* argc�����в������������argv�洢�����б������� */
int output_tcp(int argc, char *argv[])
{
	/** �������׽����ļ������� */
	int s_s,s_c;
	/** ���ص�ַ */
	struct sockaddr_in local,from;
	time_t now;
	char buff[BUFFLEN];
	int n = 0;
	int len =sizeof(from);
	
	/** ����TCP�׽��� */
	s_s = socket( AF_INET, SOCK_STREAM, 0);
	/** ��ʼ����ַ */
	memset(&local, 0,sizeof(local));  // ����
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(SERVER_PORT);
	
	/** ���׽����ļ��������󶨵����ص�ַ�Ͷ˿� */
	int err = bind(s_s, (struct sockaddr *)&local,sizeof(local));
	err = listen(s_s, BACKLOG);
	
	// while(1)
	// {
		// /** ���տͻ������� */
		// s_c = accept(s_s,(struct sockaddr *)&from,&len);
		// memset(buff, 0, BUFFLEN);
		// n = recv(s_c, buff, BUFFLEN, 0);
		// if(n > 0 && !strncmp(buff,"TIME",4))
		// {
			// memset(buff, 0,BUFFLEN);
			// now = time(NULL);
			// sprintf(buff,"%24s\r\n",ctime(&now));
			// printf("Send data:%s",buff);
			// send(s_c,buff,strlen(buff),0);
		// }
		// close(s_c);
	// }
	handle_connect(s_s);
	close(s_s);
    return 0;
}










#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

#include "output_tcp.h"

#define BUFFLEN 1024
#define SERVER_PORT 7777
#define BACKLOG 5
#define CLIENTNUM 2

/** ������ */
pthread_mutex_t ALOCK = PTHREAD_MUTEX_INITIALIZER;

static void *handle_request(void *arg)
{
	int s_s = *((int*)arg);
	int s_c;
	struct sockaddr_in from;
	int len = sizeof(from);
	
	time_t now;
	char buff[BUFFLEN];
	int n = 0;	
	
	for(;;)
	{

		pthread_mutex_lock(&ALOCK);
		
		s_c = accept(s_s,(struct sockaddr*)&from,&len);
		
		pthread_mutex_unlock(&ALOCK);
		while(s_c > 0)
		{
			memset(buff,0,BUFFLEN);
			n = recv(s_c,buff,BUFFLEN,0);
			if(n > 0 && !strncmp(buff,"TIME",4))
			{
				memset(buff,0,BUFFLEN);
				now = time(NULL);
				sprintf(buff,"%24s\r\n",ctime(&now));
				printf("Send data:%s",buff);
				send(s_c,buff,strlen(buff),0);
				
			}
			else
			{
				close(s_c);
				return NULL;
			}
			printf("One client one thread,thread ID %d\n", pthread_self());
		}
	}

}

static void handle_connect(int s)
{
	int s_s = s;
	pthread_t thread_do[CLIENTNUM];
	int i = 0;
	
	for(i = 0;i < CLIENTNUM;i++)
	{
		pthread_create(&thread_do[i],
		NULL,
		handle_request,
		(void*)&s_s);
	}
	for(i = 0;i < CLIENTNUM;i++)
		pthread_join(thread_do[i],NULL);
}


/* argc�����в������������argv�洢�����б������� */
int output_tcp(int argc, char *argv[])
{
	/** �������׽����ļ������� */
	int s_s;
	
	/** ���ص�ַ */
	struct sockaddr_in local;
	
	/** ����TCP�׽��� */
	s_s = socket( AF_INET, SOCK_STREAM, 0);
	/** ��ʼ����ַ */
	memset(&local, 0,sizeof(local));  // ����
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(SERVER_PORT);
	
	/** ���׽����ļ��������󶨵����ص�ַ�Ͷ˿� */
	int err = bind(s_s, (struct sockaddr *)&local,sizeof(local));
	err = listen(s_s, BACKLOG);
	
	handle_connect(s_s);
	
	close(s_s);
	
    return 0;
}





