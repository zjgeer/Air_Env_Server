#ifndef __OUTPUT_TCP_H__
#define __OUTPUT_TCP_H__

/** 服务器套接字文件描述符 */
int s_s;
/** 客户端套接字文件描述符 */
int s_c;
int fp_database;

#define COUNTER_CLIENT 5


int output_tcp(int argc, char *argv[]);


#endif