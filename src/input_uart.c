
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#include "input_uart.h"

extern int start_collect;

pthread_mutex_t  mutex_pm = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t  mutex_gps = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t  mutex_sensor = PTHREAD_MUTEX_INITIALIZER;
int set_com_config(int fd,int baud_rate,int data_bits, char parity, int stop_bits) 
{ 
	struct termios new_cfg,old_cfg; 
	int speed; 
	/*保存并测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/ 
	if (tcgetattr(fd, &old_cfg) != 0) 
	{ 
		perror("tcgetattr"); 
		return -1; 
	}
	/* 设置字符大小*/ 
	new_cfg = old_cfg; 
	cfmakeraw(&new_cfg); /* 配置为原始模式*/
	new_cfg.c_cflag &= ~CSIZE; 
	/*设置波特率*/ 
	switch (baud_rate) 
	{ 
		case 2400: 
		{
			speed = B2400; 
		}
		break;
		case 4800:
		{
			speed = B4800;
		}
		break;
		case 9600:
		{
			speed = B9600;
		}
		break;
		case 19200:
		{
			speed = B19200;
		}
		break;
		case 38400: 
		{
			speed = B38400; 
		}
		break;
		default:
		case 115200: 
		{
			speed = B115200; 
		}
		break; 
	} 
	cfsetispeed(&new_cfg, speed); 
	cfsetospeed(&new_cfg, speed);
	/*设置停止位*/ 
	switch (data_bits) 
	{ 
		case 7: 
		{ 
			new_cfg.c_cflag |= CS7; 
		} 
		break; 
		default: 
		case 8: 
		{ 
			new_cfg.c_cflag |= CS8; 
		} 
		break; 
	} 
	/*设置奇偶校验位*/ 
	switch (parity) 
	{ 
		default: 
		case 'n': 
		case 'N': 
		{ 
			new_cfg.c_cflag &= ~PARENB;
			new_cfg.c_iflag &= ~INPCK; 
		} 
		break; 
		case 'o': 
		case 'O': 
		{ 
			new_cfg.c_cflag |= (PARODD | PARENB); 
			new_cfg.c_iflag |= INPCK;
		} 
		break; 
		case 'e': 
		case 'E': 
		{ 
			new_cfg.c_cflag |= PARENB; 
			new_cfg.c_cflag &= ~PARODD; 
			new_cfg.c_iflag |= INPCK; 
		} 
		break; 
		case 's': /*as no parity*/ 
		case 'S': 
		{ 
			new_cfg.c_cflag &= ~PARENB; 
			new_cfg.c_cflag &= ~CSTOPB;
		} 
		break; 
	} 

		/*设置停止位*/ 
	switch (stop_bits) 
	{ 
		default: 
		case 1: 
		{ 
			new_cfg.c_cflag &= ~CSTOPB; 
		} 
		break; 
		case 2: 
		{ 
			new_cfg.c_cflag |= CSTOPB; 
		} 
	} 
	/*设置等待时间和最小接收字符*/ 
	new_cfg.c_cc[VTIME] = 0; 
	new_cfg.c_cc[VMIN] = 10;
	/*处理未接收字符*/ 
	tcflush(fd, TCIFLUSH); 
	/*激活新配置*/ 
	if ((tcsetattr(fd, TCSANOW, &new_cfg)) != 0)
	{ 
		perror("tcsetattr"); 
		return -1; 
	}
	return 0;
}

int set_com_config_gps(int fd,int baud_rate,int data_bits, char parity, int stop_bits) 
{ 
	struct termios new_cfg,old_cfg; 
	int speed; 
	/*保存并测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/ 
	if (tcgetattr(fd, &old_cfg) != 0) 
	{ 
		perror("tcgetattr"); 
		return -1; 
	}
	/* 设置字符大小*/ 
	new_cfg = old_cfg; 
	cfmakeraw(&new_cfg); /* 配置为原始模式*/
	new_cfg.c_cflag &= ~CSIZE; 
	/*设置波特率*/ 
	switch (baud_rate) 
	{ 
		case 2400: 
		{
			speed = B2400; 
		}
		break;
		case 4800:
		{
			speed = B4800;
		}
		break;
		case 9600:
		{
			speed = B9600;
		}
		break;
		case 19200:
		{
			speed = B19200;
		}
		break;
		case 38400: 
		{
			speed = B38400; 
		}
		break;
		default:
		case 115200: 
		{
			speed = B115200; 
		}
		break; 
	} 
	cfsetispeed(&new_cfg, speed); 
	cfsetospeed(&new_cfg, speed);
	/*设置停止位*/ 
	switch (data_bits) 
	{ 
		case 7: 
		{ 
			new_cfg.c_cflag |= CS7; 
		} 
		break; 
		default: 
		case 8: 
		{ 
			new_cfg.c_cflag |= CS8; 
		} 
		break; 
	} 
	/*设置奇偶校验位*/ 
	switch (parity) 
	{ 
		default: 
		case 'n': 
		case 'N': 
		{ 
			new_cfg.c_cflag &= ~PARENB;
			new_cfg.c_iflag &= ~INPCK; 
		} 
		break; 
		case 'o': 
		case 'O': 
		{ 
			new_cfg.c_cflag |= (PARODD | PARENB); 
			new_cfg.c_iflag |= INPCK;
		} 
		break; 
		case 'e': 
		case 'E': 
		{ 
			new_cfg.c_cflag |= PARENB; 
			new_cfg.c_cflag &= ~PARODD; 
			new_cfg.c_iflag |= INPCK; 
		} 
		break; 
		case 's': /*as no parity*/ 
		case 'S': 
		{ 
			new_cfg.c_cflag &= ~PARENB; 
			new_cfg.c_cflag &= ~CSTOPB;
		} 
		break; 
	} 

		/*设置停止位*/ 
	switch (stop_bits) 
	{ 
		default: 
		case 1: 
		{ 
			new_cfg.c_cflag &= ~CSTOPB; 
		} 
		break; 
		case 2: 
		{ 
			new_cfg.c_cflag |= CSTOPB; 
		} 
	} 
	/*设置等待时间和最小接收字符*/ 
	new_cfg.c_cc[VTIME] = 0; 
	new_cfg.c_cc[VMIN] = 1;
	/*处理未接收字符*/ 
	tcflush(fd, TCIFLUSH); 
	/*激活新配置*/ 
	if ((tcsetattr(fd, TCSANOW, &new_cfg)) != 0)
	{ 
		perror("tcsetattr"); 
		return -1; 
	}
	return 0;
}

int set_com_config_sensor(int fd,int baud_rate,int data_bits, char parity, int stop_bits) 
{ 
	struct termios new_cfg,old_cfg; 
	int speed; 
	/*保存并测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/ 
	if (tcgetattr(fd, &old_cfg) != 0) 
	{ 
		perror("tcgetattr"); 
		return -1; 
	}
	/* 设置字符大小*/ 
	new_cfg = old_cfg; 
	cfmakeraw(&new_cfg); /* 配置为原始模式*/
	new_cfg.c_cflag &= ~CSIZE; 
	/*设置波特率*/ 
	switch (baud_rate) 
	{ 
		case 2400: 
		{
			speed = B2400; 
		}
		break;
		case 4800:
		{
			speed = B4800;
		}
		break;
		case 9600:
		{
			speed = B9600;
		}
		break;
		case 19200:
		{
			speed = B19200;
		}
		break;
		case 38400: 
		{
			speed = B38400; 
		}
		break;
		default:
		case 115200: 
		{
			speed = B115200; 
		}
		break; 
	} 
	cfsetispeed(&new_cfg, speed); 
	cfsetospeed(&new_cfg, speed);
	/*设置停止位*/ 
	switch (data_bits) 
	{ 
		case 7: 
		{ 
			new_cfg.c_cflag |= CS7; 
		} 
		break; 
		default: 
		case 8: 
		{ 
			new_cfg.c_cflag |= CS8; 
		} 
		break; 
	} 
	/*设置奇偶校验位*/ 
	switch (parity) 
	{ 
		default: 
		case 'n': 
		case 'N': 
		{ 
			new_cfg.c_cflag &= ~PARENB;
			new_cfg.c_iflag &= ~INPCK; 
		} 
		break; 
		case 'o': 
		case 'O': 
		{ 
			new_cfg.c_cflag |= (PARODD | PARENB); 
			new_cfg.c_iflag |= INPCK;
		} 
		break; 
		case 'e': 
		case 'E': 
		{ 
			new_cfg.c_cflag |= PARENB; 
			new_cfg.c_cflag &= ~PARODD; 
			new_cfg.c_iflag |= INPCK; 
		} 
		break; 
		case 's': /*as no parity*/ 
		case 'S': 
		{ 
			new_cfg.c_cflag &= ~PARENB; 
			new_cfg.c_cflag &= ~CSTOPB;
		} 
		break; 
	} 

		/*设置停止位*/ 
	switch (stop_bits) 
	{ 
		default: 
		case 1: 
		{ 
			new_cfg.c_cflag &= ~CSTOPB; 
		} 
		break; 
		case 2: 
		{ 
			new_cfg.c_cflag |= CSTOPB; 
		} 
	} 
	/*设置等待时间和最小接收字符*/ 
	new_cfg.c_cc[VTIME] = 0; 
	new_cfg.c_cc[VMIN] = 1;
	/*处理未接收字符*/ 
	tcflush(fd, TCIFLUSH); 
	/*激活新配置*/ 
	if ((tcsetattr(fd, TCSANOW, &new_cfg)) != 0)
	{ 
		perror("tcsetattr"); 
		return -1; 
	}
	return 0;
}
void analysis(char *buffer_gps)
{
	int j = 0;
	int cnt;
	char comma_address[15];
	float c ,d;
	int degrees, minutes, seconds;
	for( j = 0;j < 15;j++)
		comma_address[j] = 0;
	if(!strncmp(buffer_gps,"$GPGGA",6))
	{
		printf("_+_+_+_+_+_+_+_+_+_+_\n|  %s  | \n",buffer_gps);
		j = 0;
		for(cnt = 0;cnt < 77;cnt++)
		{
			if(buffer_gps[cnt] == ',')
			{
				comma_address[j] = cnt;
				//printf("%d\n",comma_address[j]);
				j++;
			}
		}

		//memcpy(longitude,buffer_gps + comma_address[3] + 1,comma_address[4] - comma_address[3] - 1);
		//memcpy(latitude,buffer_gps + comma_address[1] + 1,comma_address[2] - comma_address[1] - 1);
		//memcpy(altitude,buffer_gps + comma_address[8] + 1,comma_address[9] - comma_address[8] - 1);
		// $GPGGA,005910.0002256.526647,N,11322.950368,E,0,00,127.000,49.182,M,0,M,,*49
		// $GPGGA,005908.000,2256.526647,N,11322.950368,E,0,00,127.000,49.182,M,0,M,,*40
		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@strlen(buffer_gps) = %d   \n",strlen(buffer_gps));
		if(j == 14 && strlen(buffer_gps) == 79)
		{
			pthread_mutex_lock( &mutex_gps );	
			memcpy(longitude,buffer_gps + comma_address[3] + 1,12);	
			c = atof(longitude);
			d = c / 100.0;
			degrees = (int)d;
			minutes = (int)(( d - degrees ) * 60);
			seconds = (int)((( d - degrees ) * 60 - minutes) * 60);
			// printf("\n****************%d,%d,%d****************\n",degrees,minutes,seconds);
			sprintf(longitude, "%d'%d'%d'", degrees,minutes,seconds);
			//gcvt(c,12,longitude);
			printf("************%s, %f ************\n",longitude,c);
			memcpy(latitude,buffer_gps + comma_address[1] + 1,11);
			c = atof(latitude);
			d = c / 100.0;
			degrees = (int)d;
			minutes = (int)(( d - degrees ) * 60);
			seconds = (int)((( d - degrees ) * 60 - minutes) * 60);
			// printf("\n****************%d,%d,%d****************\n",degrees,minutes,seconds);
			sprintf(latitude, "%d'%d'%d'", degrees,minutes,seconds);
			memcpy(altitude,buffer_gps + comma_address[8] + 1,6);
			printf("-------- longitude = %s ,latitude = %s ,altitude = %s \n",longitude,latitude,altitude);
			pthread_mutex_unlock( &mutex_gps );
		}
	}
}

void* thread_input_gps_function (void* arg)
{
	/* gps input线程要完成的工作 */
	int i = 0;

    int fd_gps = *((int*)arg);
	char gps_read[2];
	printf("thread gps input work!\n");
	memset(buffer_gps,0,BUFFER_SIZE_GPS);
	while(1)
	{
		memset(gps_read,0,BUFFER_SIZE_GPS);
		if(	read(fd_gps, gps_read, sizeof(gps_read)) > 0)
		{
			// printf("%c",gps_read[0]);			
			if(i > 0)
			{
				buffer_gps[i] = gps_read[0];
				i++;
			}
			/** 判断回车符 */
			if(0x0a == gps_read[0])
			{
				//printf("************ %s",buffer_gps);
				analysis(buffer_gps);
				i = 0;
				memset(buffer_gps,0,BUFFER_SIZE_GPS);
			}
			if('$' == gps_read[0])
			{
				buffer_gps[i] = gps_read[0];
				i++;
			}
		}
	}
	close(fd_gps);
	pthread_exit("Exiting from the thread _function!");
}

void *thread_input_pm25_function (void* arg)
{
	int fd_pm25 = *((int*)arg);

	printf("thread pm25 input work!\n");
	/* pm25 input线程要完成的工作 */
		// 打开串口，接收数据，解析数据，存储数据，发送数据
	do
	{
		memset(buffer_pm25,0,BUFFER_SIZE_PM25);
		if(	read(fd_pm25, buffer_pm25, 10) > 0)
		{
		
			pthread_mutex_lock( &mutex_pm );
			pm2_5 = (float)(buffer_pm25[3]*256 + buffer_pm25[2])/10.0;
			pm10  = (float)(buffer_pm25[5]*256 + buffer_pm25[4])/10.0;
			pthread_mutex_unlock( &mutex_pm );
			
			printf("PM2.5 = %.1f,PM10 = %.1f\n",pm2_5,pm10);
		}
		printf("\n");
	}
	while(strncmp(buffer_pm25,"quit",4));

	close(fd_pm25);

	pthread_exit("Exiting from the thread _function!");
}

void* thread_input_sensor_function (void* thread_arg)
{
	int fd_sensor = *((int*)thread_arg);
	printf("Thread sensor input work!\n");
	/* sensor input线程要完成的工作 */
	while(1)
	{
		// 打开串口，接收数据，解析数据，存储数据，发送数据
		
		memset(buffer_sensor,0,BUFFER_SIZE_SENSOR);
		if(	read(fd_sensor, buffer_sensor, sizeof(buffer_sensor)) > 0)
		{
			//34,32,203,  0@
			if(strlen(buffer_sensor) > 5)
			{
				printf("------------- %s",buffer_sensor);
				
				pthread_mutex_lock( &mutex_sensor );
				memcpy(tempeture_raw,buffer_sensor,2);
				memcpy(humidity_raw,buffer_sensor+3,2);
				memcpy(smog_raw,buffer_sensor+6,3);
				
				printf("\nThe tempeture is %s 'C,the humidity is %s rh,the smog is %s ppm. \n",tempeture_raw,humidity_raw,smog_raw);
				pthread_mutex_unlock( &mutex_sensor );
			}
			else
			{
			//	pthread_mutex_lock( &mutex_sensor );
			//	memset(tempeture_raw,0x30,2);
			//	memset(humidity_raw,0x30,2);
			//	pthread_mutex_unlock( &mutex_sensor );
			}
			
		}
		
		if(start_collect == 1)
		{
			start_collect = 0;
			write(fd_sensor,"#START@",strlen("#START@"));
		}
		else if(start_collect == 2)
		{
			start_collect = 0;
			write(fd_sensor,"#STOP@",strlen("#STOP@"));
		}

		sleep(1);
	}
	pthread_exit("Exiting from the thread _function!");
}

int input_uart(void)
{
	char i;
	int fd[3];
	char *ttyusbx[3] = {"/dev/ttyUSB0","/dev/ttyUSB1","/dev/ttyUSB2"};
	unsigned char buffer_public[BUFFER_SIZE_PUBLIC];
	unsigned char pm25_header = 0xAA;
	unsigned char pm25_header1 = 0xC0;
	unsigned char gps_header  = '$';
	unsigned char gps_header1  = 'G';
	unsigned char sensor_header = '#';
	
	/** 分别打开ttyUSB0~ttyUSB2设备 */
	for(i = 0;i < 3;i++)
	{
		fd[i] = open(ttyusbx[i], O_RDWR|O_NOCTTY|O_NDELAY);
		
		if ( fd[i] < 0 )
		{
			printf("Open %s error\n",ttyusbx[i]);
			continue;
			//return 1;
		}
		/* 恢复串口的状态为阻塞状态 */
		if(fcntl(fd[i], F_SETFL, 0) < 0)
		{
			perror("fcntl F_SETFL\n");
		}
		/* 测试打开文件描述 */
		if(isatty(STDIN_FILENO) == 0)
		{
			printf("standard input is not a terminal device!\n");
			return 1;
		}

		if(set_com_config(fd[i],9600,8,'N',1) < 0)
		{
			perror("set_com_config");
			return 1;
		}

TestAgain:	memset(buffer_public,0,BUFFER_SIZE_PUBLIC);
		if(	read(fd[i], buffer_public, 10) > 0)
		{
			/** 判断打开的是否为pm25传感器USB串口设备 */
			if(0 == strncmp(&buffer_public[0],&pm25_header,1) && 0 == strncmp(&buffer_public[1],&pm25_header1,1))
			{
				printf("Open %s device is the pm2.5 sensor!\n",ttyusbx[i]);
				pthread_t thread_input_pm25;
				pthread_create(&thread_input_pm25, NULL, &thread_input_pm25_function, (void*)&fd[i]);	
				pthread_detach(thread_input_pm25);
			}/** 判断打开的是否为GPS USB串口设备 */
			else if(0 == strncmp(&buffer_public[0],&gps_header,1) && 0 == strncmp(&buffer_public[1],&gps_header1,1)) 
			{
				if(set_com_config_gps(fd[i],9600,8,'N',1) < 0)
				{
					perror("set_com_config");
					//return 1;
				}
				printf("Open %s device is the gps device!\n",ttyusbx[i]);
				pthread_t thread_input_gps;
				pthread_create(&thread_input_gps, NULL, &thread_input_gps_function, (void*)&fd[i]);
				pthread_detach(thread_input_gps);
			}/** 判断打开的是否为传感器USB串口设备 */
			else if(0 == strncmp(&buffer_public[0],&sensor_header,1))
			{
				if(set_com_config_sensor(fd[i],9600,8,'N',1) < 0)
				{
					perror("set_com_config");
					//return 1;
				}
				printf("Open %s device is the sensor device!\n",ttyusbx[i]);
				pthread_t thread_input_sensor;
				pthread_create(&thread_input_sensor, NULL, &thread_input_sensor_function,(void*)&fd[i]);
				pthread_detach(thread_input_sensor);
			}
			else
			{
				goto TestAgain;
			}
		}
	}

	while(1)
	{
		sleep(3);
		printf("Input_uart is working\n");
	}

	return 0;
}