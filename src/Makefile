#bulid a Makefile for air_env_server.c
#http://blog.csdn.net/gubenpeiyuan/article/details/8437640 参考此博客可以编译通过
air_env_server: air_env_server.o input_uart.o output_tcp.o input_uvc.o
	$(CC) $(LDFLAGS) air_env_server.o input_uart.o output_tcp.o input_uvc.o -o air_env_server -lpthread

air_env_server.o: air_env_server.c
	$(CC) $(CFLAGS) -c air_env_server.c
	
input_uart.o: input_uart.c
	$(CC) $(CFLAGS) -c input_uart.c

input_uvc.o: input_uvc.c
	$(CC) $(CFLAGS) -c input_uvc.c
	
output_tcp.o: output_tcp.c
	$(CC) $(CFLAGS) -c output_tcp.c
	
clean:
	rm *.o air_env_server