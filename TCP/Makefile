all : tcpserver tcpclient

tcpserver : tcpserver.c
	gcc tcpserver.c -lssl -lcrypto -o tcpserver

tcpclient : tcpclient.c
	gcc tcpclient.c -lssl -lcrypto -o tcpclient


clean: 
	rm tcpserver tcpclient 
