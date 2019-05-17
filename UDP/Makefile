all : udps udpc

udps : udpserver.c
	gcc udpserver.c -lssl -lcrypto -o udps

udpc : udpclient.c
	gcc udpclient.c -lssl -lcrypto -o udpc

clean :
	rm udpc udps
