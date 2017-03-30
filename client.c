/*
 * echoclient.c - An echo client
 */
#include "csapp.h"
#include <sys/time.h>

int main(int argc, char **argv)
{
    int clientfd, port;
    char *host, bufName[MAXLINE],bufFile[MAXLINE];
    rio_t rio;
    int fdin;
    ssize_t sizeFile = 0;
    struct timeval stop, start;
    double resTemps;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = atoi(argv[2]);
    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    clientfd = Open_clientfd(host, port);

    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client connected to server OS\n");
    printf("Rentrer un nom de fichier :\n");
    Rio_readinitb(&rio, clientfd);

    while (Fgets(bufName, MAXLINE, stdin) != NULL) {

        Rio_writen(clientfd, bufName, strlen(bufName));
        if((fdin = open("Fichier_recu.jpg",O_WRONLY | O_CREAT,644))>0){
            gettimeofday(&start, NULL);
            while (Rio_readlineb(&rio, bufFile, MAXLINE) != 0) {
                Fputs(bufFile, stdout);
                sizeFile += rio_writen(fdin, bufFile, strlen(bufFile));
            }
            close(fdin);
            gettimeofday(&stop, NULL);
            printf("Le fichier a été envoyé avec succes\n");
            if(stop.tv_usec => start.tv_usec){
                resTemps = stop.tv_usec - start.tv_usec;
            }else{

            }
            printf(" %lu bytes reçus en %li ms\n",sizeFile,(stop.tv_usec - start.tv_usec)/1000000);
        }else{
            printf("Problème ouverture fichier");
        }
        printf("Rentrer un nom de fichier :\n");
    }
    Close(clientfd);
    exit(0);
}
