#ifndef __INPUT_UART_H__
#define __INPUT_UART_H__

#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE_PUBLIC	64
#define BUFFER_SIZE_PM25	64
#define BUFFER_SIZE_GPS		1024
#define BUFFER_SIZE_SENSOR	64

float pm2_5;
float pm10;

unsigned char buffer_pm25[BUFFER_SIZE_PM25];
char buffer_gps[BUFFER_SIZE_GPS];
char longitude[20];
char latitude[20];
char altitude[20];
unsigned char buffer_sensor[BUFFER_SIZE_SENSOR];
char tempeture_raw[3];
char humidity_raw[3];
char smog_raw[4];

typedef struct server_send_to_client
{
	char header;			// 帧头
	char *tempeture;		// 温度
	char *humidity;			// 湿度
	char *smog;				// 烟雾浓度
	float pm25;				// pm2.5数值
	float pm10;				// pm10数值
	char *longitude;		// 经度
	char west_east;			// W,E
	char *latitude;			// 纬度
	char south_north;		// S,N
	char *altitude;			// 高度
	char *gamsample;		// 气体采集装置状态
	char tail;				// 帧尾
}sst_c;

typedef struct client_send_to_server
{
	char header;			// 帧头
	int gamsample;			// 气体采集装置状态
	char tail;				// 帧尾
}cst_s;

/** 经过串口发送的数据 */
typedef struct server_send_to_sensor_board
{
	char header;			// 帧头
	char cmd1;				// 命令1
	char cmd2;				// 命令2
	char tail;				// 帧尾
}sst_sb;

typedef struct sensor_board_send_to_server
{
	char header;			// 帧头
	int tempeture;			// 温度
	int humidity;			// 湿度
	int gamsample;			// 气体采集装置状态
	char tail;				// 帧尾
}sbst_s;





int set_com_config(int fd,int baud_rate,int data_bits, char parity, int stop_bits);
int input_uart(void);

#endif