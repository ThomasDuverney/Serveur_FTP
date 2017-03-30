/*
 * echoserveri.c - An iterative echo server
 */

#include "csapp.h"
#include <dirent.h>

#define MAX_NAME_LEN 256
#define NB_CLIENTS 2
void echo(int connfd);
void sendFile(int connfd);
/*
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */

int ls();

int main(int argc, char **argv)
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    pid_t pidPapa = getpid();
    //DIR* dir;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);

    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);
    int i=0;
    while(i <NB_CLIENTS && fork() != 0){
        i++;
    }

    while (1) {

        if(getpid()!= pidPapa){

            while((connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen))<0);
            //Close(listenfd);
            /* determine the name of the client */
            Getnameinfo((SA *) &clientaddr, clientlen,
                        client_hostname, MAX_NAME_LEN, 0, 0, 0);

            /* determine the textual representation of the client's IP address */
            Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                      INET_ADDRSTRLEN);

            printf("server connected to %s (%s)\n", client_hostname,
                   client_ip_string);

            sendFile(connfd);
            Close(connfd);
        }
    }
    exit(0);
}

int ls()
{
	currentdir = opendir(".");

	while (currentdir)
	{
	    errno = 0;
	    if ((rd = readdir(currentdir)) != NULL)
	    {
	        if (strcmp(rd->d_name, name) == 0)
		{
	            closedir(currentdir);
	        }
	    }
	    else
	    {
	        if (errno == 0) 
		{
	            closedir(dirp);
		    printf("Error : File not found !");
	            return 1;
	        }
	        closedir(dirp);
		printf("Read error");
		return 2;
   	     }
	}
	return 0;
}
