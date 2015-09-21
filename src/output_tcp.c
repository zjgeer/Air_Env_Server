/** 实现多个客户端的请求,多个客户端同一个线程 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "output_tcp.h"
#include "input_uart.h"

#define BUFFLEN 1024
#define SERVER_PORT 7777
#define BACKLOG 5


int picture_capture = 0;
int start_collect = 0;
sst_c server_sendto_c;



char *data_file = "/usr/database.txt";

extern pthread_mutex_t  mutex_pm;
extern pthread_mutex_t  mutex_gps;
extern pthread_mutex_t  mutex_sensor;

void *handle_request(void *arg)
{
	int s_c = *((int*)arg);
	time_t now;
	char buff[BUFFLEN];
	char send_buffer[64];
	int n = 0;
	server_sendto_c.header = '$';
	server_sendto_c.tempeture = "25";
	server_sendto_c.humidity = "45";
	server_sendto_c.smog = "120";
	server_sendto_c.longitude = "longitude";
	server_sendto_c.west_east = 'E';			// W,E
	server_sendto_c.latitude = "latitude";
	server_sendto_c.south_north = 'N';
	server_sendto_c.altitude = "altitude";
	server_sendto_c.gamsample = "  0";
	server_sendto_c.tail = '@';
	char send_toc[64] = "Server received the cmd\n";

	while(1)
	{
		pthread_mutex_lock( &mutex_sensor );
		server_sendto_c.tempeture = tempeture_raw;
		server_sendto_c.humidity = humidity_raw;
		server_sendto_c.smog = smog_raw;
		pthread_mutex_unlock( &mutex_sensor );
	
		pthread_mutex_lock( &mutex_pm );
		server_sendto_c.pm25 = pm2_5;
		server_sendto_c.pm10 = pm10;
		pthread_mutex_unlock( &mutex_pm );

		pthread_mutex_lock( &mutex_gps );	
		server_sendto_c.longitude = longitude;
		server_sendto_c.latitude = latitude;
		server_sendto_c.altitude = altitude;
		pthread_mutex_unlock( &mutex_gps );	

		memset(buff,0,BUFFLEN);
		n = recv(s_c, buff,BUFFLEN,0);
		if(n > 0)
		{
			if(!strncmp(buff,"GET_DATA",8))
			{
				memset(buff,0,BUFFLEN);
				now = time(NULL);
				//sprintf(buff,"%24s\n",ctime(&now));
				//printf("Send data:%s",buff);
				//send(s_c,buff,strlen(buff),0);
				sprintf(send_buffer,"%c,%s,%s,%.2f,%.2f,%s,%c,%s,%c,%s,%s,%s,%c\n",
				server_sendto_c.header,
				server_sendto_c.tempeture,
				server_sendto_c.humidity,
				server_sendto_c.pm25,
				server_sendto_c.pm10,
				server_sendto_c.longitude,
				server_sendto_c.west_east,
				server_sendto_c.latitude,
				server_sendto_c.south_north,
				server_sendto_c.altitude,
				server_sendto_c.gamsample,
				server_sendto_c.smog,
				server_sendto_c.tail
				);
	
				printf("sizeof(send_buffer) = %d.\n",strlen(send_buffer));
				printf("send_buffer = %s.\n",send_buffer);
				write(fp_database,send_buffer,strlen(send_buffer));
				send(s_c,&send_buffer,strlen(send_buffer),0);
			}
			else if(!strncmp(buff,"SEND_CMD",8))
			{
				send(s_c,send_toc,strlen(send_toc),0);
			}
			else if(!strncmp(buff,"GET_PICTURE",11))
			{
				// 调用抓拍函数
				//pthread_mutex_lock( &mutex_uvc );
				picture_capture = 1;
				//pthread_mutex_unlock( &mutex_uvc );
			}
			else if(!strncmp(buff,"START_COLLECT",13))
			{
				start_collect = 1;
				printf("----  start  ----%s  ----\n",buff);
			}
			else if(!strncmp(buff,"STOP_COLLECT",12))
			{
				start_collect = 2;
				printf("----  stop  ----%s  ----\n",buff);
			}
			else
			{
				printf("Receive command error!\n");
			}
		}
		else // 判断断开连接
		{
			if(errno != EINTR)
			{
				printf("One client one thread,thread ID %d device disconnect.\n", pthread_self());
				close(s_c);
				return NULL;
			}
			else
			{
				// 连接正常
			}
		}
		printf("One client one thread,thread ID %d\n", pthread_self());
	}
}

static void handle_connect(int s_s)
{
	char client_ip[20];
	struct sockaddr_in from;
	int counter_client = 0;
	int len = sizeof(from);
	pthread_t thread_do;
	
	fp_database = open(data_file,O_RDWR | O_CREAT ); // | O_APPEND暂时不用追加
	while(1)
	{
		s_c = accept(s_s,(struct sockaddr*)&from,&len);
		if(s_c > 0)
		{
			if(counter_client <= COUNTER_CLIENT)
			{
				memcpy(client_ip,inet_ntoa(from.sin_addr),len);
				printf("Connect client device ip is: %s ,port number is %d.\n",client_ip,from.sin_port);
				int err = pthread_create(&thread_do,NULL,handle_request,(void*)&s_c);
				pthread_detach(thread_do);
			}
		}
	}
}

/* argc命令行参数输入个数，argv存储了所有变量参数 */
int output_tcp(int argc, char *argv[])
{
	/** 本地地址 */
	struct sockaddr_in local;
	time_t now;
	char buff[BUFFLEN];
	int err;
	
	/** 建立TCP套接字 */
	s_s = socket( AF_INET, SOCK_STREAM, 0);
	
	/** 初始化地址 */
	memset(&local, 0,sizeof(local));  // 清零
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(SERVER_PORT);

	/** 将套接字文件描述符绑定到本地地址和端口 */
BIND: err = bind(s_s, (struct sockaddr *)&local,sizeof(local));

	if(0 == err)
	{
		printf("+++++++++++++++++ TCP server start! +++++++++++++++++\n");
	}
	else
	{
		printf("bind error: %s\n", strerror(errno));
		printf("bind fail\n");
		sleep(30);
		goto BIND;
	}

	err = listen(s_s, BACKLOG);

	handle_connect(s_s);
	
	close(s_s);
    return 0;
}