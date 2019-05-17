/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>
#include <fcntl.h>
#include <openssl/md5.h>
#define BUFSIZE 1024
# define HEAD 2*sizeof(int)
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
    int sockfd, portno, n;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE],buf1[BUFSIZE];
    struct timeval tv;
    int i,m;
    unsigned char c[50];
    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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

    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));


    /* get file from user */
	    serverlen = sizeof(serveraddr);
    bzero(buf, BUFSIZE);
    fgets(buf, BUFSIZE, stdin);
	//scanf("%s",buf);
	printf("%s\n",buf);
    for(i=0;buf[i]!='\n' && buf[i]!='\0';++i);
    buf[i]='\0';

    /* finding size of file  */
	FILE *fp=fopen(buf,"rb");
	fseek(fp, 0L, SEEK_END);
	int x= ftell(fp);
	fclose(fp);

	int y=(x%(BUFSIZE-HEAD)?x/(BUFSIZE-HEAD)+1:x/(BUFSIZE-HEAD));

    /* opening file for transfer */
	char buf2[200];
	strcpy(buf2,buf);
	int fr=open(buf,O_RDONLY);
	buf[i]=' ';
	sprintf(buf+i+1,"%d",x);
	for(;buf[i]!='\n' && buf[i]!='\0';++i);
	buf[i]=' ';++i;
	sprintf(buf+i,"%d",y);
		printf("%s\n",buf);
	while(1){
	n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);
	printf("%d\n",n);
    	n = recvfrom(sockfd, buf, BUFSIZE, 0, &serveraddr, &serverlen);
	if(n>0)break;
	}

	printf("server says : %s\ntransfering file...\n",buf);
	bzero(buf,BUFSIZE);

    /* transfering file */
	for(i=0;(n=read(fr,buf+HEAD,BUFSIZE-HEAD))>0;++i)
	{
		((int*)buf)[0]=i;
		((int*)buf)[1]=n;

		while(1){
		sendto(sockfd, buf, n+HEAD, 0, &serveraddr, serverlen);
		m = recvfrom(sockfd, buf1, BUFSIZE, 0, &serveraddr, &serverlen);
		if(m>0 && ((int*)buf1)[0]==i)break;
		
		}
		bzero(buf,BUFSIZE);
	}
	//printf("transfer complete\n");
	
	md5(c,buf2);
	while(1){
	m = recvfrom(sockfd, buf, BUFSIZE, 0, &serveraddr, &serverlen);
	if(m>0 && ((int*)buf)[0]==i)
	{
    /* checking md5 */
	if(!strncmp(buf+HEAD,(char *)c,MD5_DIGEST_LENGTH))
	{
		printf("transfer complete md5 matched\n");
		break;
	}
	else {
		printf("transfer error md5 not matched\n");
		break;
	}
	}
}


    /* send the message to the server 
    serverlen = sizeof(serveraddr);
    n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);
    if (n < 0) 
      error("ERROR in sendto");
    
    /* print the server's reply 
    n = recvfrom(sockfd, buf, strlen(buf), 0, &serveraddr, &serverlen);
    if (n < 0) 
      error("ERROR in recvfrom");
    printf("Echo from server: %s", buf);*/
    return 0;
}
