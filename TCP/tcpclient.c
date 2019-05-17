/* 
 * tcpclient.c - A simple TCP client
 * usage: tcpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include<sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <fcntl.h>
#include <openssl/md5.h>
#define BUFSIZE 1024

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

void md5(unsigned char *c,char *buf)
{
    int i;
    FILE *fr = fopen (buf, "rb");
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];
    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, fr)) != 0)
        MD5_Update (&mdContext, data, bytes);
    MD5_Final (c,&mdContext);
}


int main(int argc, char **argv) {
    int sockfd, portno, n,i;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE+1];
    unsigned char c[50];
	//MD5_CTX mdContext;
    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* connect: create a connection with the server */
    if (connect(sockfd, &serveraddr, sizeof(serveraddr)) < 0) 
      error("ERROR connecting");

    /* get file from user */
    bzero(buf, BUFSIZE);
    fgets(buf, BUFSIZE, stdin);
    for(i=0;buf[i]!='\n' && buf[i]!='\0';++i);
    buf[i]='\0';

    /* finding size of file  */
	FILE *fp=fopen(buf,"rb");
	fseek(fp, 0L, SEEK_END);
	int x= ftell(fp);
	fclose(fp);

    /* opening file for transfer */
	char buf2[20];
	strcpy(buf2,buf);
	int fr=open(buf,O_RDONLY);
	buf[i]=' ';
	sprintf(buf+i+1,"%d",x);
    n = write(sockfd, buf, strlen(buf));

	n = read(sockfd, buf, BUFSIZE);
    if (n < 0) 
      error("ERROR writing to socket");
	printf("server says : %s\ntransfering file...\n",buf);
	bzero(buf,BUFSIZE);

    /* transfering file */

	while((n=read(fr,buf,BUFSIZE))>0)
	{
		write(sockfd,buf,n);
		bzero(buf,BUFSIZE);
	}
	md5(c,buf2);
	read(sockfd,buf,MD5_DIGEST_LENGTH+1);
    /* checking md5 */
	if(!strncmp(buf,(char *)c,MD5_DIGEST_LENGTH))printf("transfer complete md5 matched\n");
	else printf("transfer error md5 not matched\n");

	
    close(sockfd);
    return 0;
}
