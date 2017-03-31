/*
 * echoserveri.c - An iterative echo server
 */

#include "csapp.h"
#include <dirent.h>

#define MAX_NAME_LEN 256
#define NB_CLIENTS 2
void echo(int connfd);
void sendFile(int connfd);

typedef struct {
     unsigned int s_addr; 
     size_t octetsSend;
} infosClient;

int ls();

int main(int argc, char **argv)
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    pid_t pidPapa = getpid();

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
            /* determine the name of the client */
            Getnameinfo((SA *) &clientaddr, clientlen,
                        client_hostname, MAX_NAME_LEN, 0, 0, 0);

            /* determine the textual representation of the client's IP address */
            Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                      INET_ADDRSTRLEN);

            printf("server connected to %s (%s)\n", client_hostname,
                   client_ip_string);
			ls();
            sendFile(connfd);
            Close(connfd);
        }
    }
    exit(0);
}

int ls()
{
	DIR* currentdir = opendir(".");
	struct dirent *dir_st;
	int nbfile = 0;
	
	while ((dir_st = readdir(currentdir)))
	{
	    printf ("%s\n", dir_st->d_name);
	    nbfile += 1;
   
	}
	printf("%i files found.\n", nbfile); 
/*	else
	    {
	        if (errno == 0) 
			{
	            closedir(currentdir);
		    	printf("Error : File not found !");
	            return 1;
	        }
			while ((dir_st = readdir(currentdir)) != NULL) 
			{
		        printf ("[%s]\n", dir_st->d_name);
		    }

	        closedir(currentdir);
			printf("Read error");
			return 2;*/
	closedir(currentdir);
	return 0;
}
