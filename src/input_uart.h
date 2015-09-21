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
	char header;			// ֡ͷ
	char *tempeture;		// �¶�
	char *humidity;			// ʪ��
	char *smog;				// ����Ũ��
	float pm25;				// pm2.5��ֵ
	float pm10;				// pm10��ֵ
	char *longitude;		// ����
	char west_east;			// W,E
	char *latitude;			// γ��
	char south_north;		// S,N
	char *altitude;			// �߶�
	char *gamsample;		// ����ɼ�װ��״̬
	char tail;				// ֡β
}sst_c;

typedef struct client_send_to_server
{
	char header;			// ֡ͷ
	int gamsample;			// ����ɼ�װ��״̬
	char tail;				// ֡β
}cst_s;

/** �������ڷ��͵����� */
typedef struct server_send_to_sensor_board
{
	char header;			// ֡ͷ
	char cmd1;				// ����1
	char cmd2;				// ����2
	char tail;				// ֡β
}sst_sb;

typedef struct sensor_board_send_to_server
{
	char header;			// ֡ͷ
	int tempeture;			// �¶�
	int humidity;			// ʪ��
	int gamsample;			// ����ɼ�װ��״̬
	char tail;				// ֡β
}sbst_s;





int set_com_config(int fd,int baud_rate,int data_bits, char parity, int stop_bits);
int input_uart(void);

#endif