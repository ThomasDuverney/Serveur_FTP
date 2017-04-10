/*
 * echoclient.c - An echo client
 */
#include "csapp.h"
#include <sys/time.h>

int main(int argc, char **argv)
{
    int clientfd, port;
    char *host, bufName[MAXLINE],bufFile[MAXBLOCK];
    rio_t rio;
    int fdin;
    ssize_t countFileSize = 0;
    size_t sizeRead,sizeFileSend;

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
    printf("Connecté à mon Serveur FTP\n");
    printf("Rentrer un nom de fichier :\n");
    Rio_readinitb(&rio, clientfd);

   time_t start_t, end_t;
   double diff_t;

    while (Fgets(bufName, MAXLINE, stdin) != NULL) {

        Rio_writen(clientfd, bufName, strlen(bufName));

        if((sizeRead = Rio_readnb(&rio, &sizeFileSend,sizeof(sizeFileSend)) != 0)  && sizeFileSend !=0){

            if((fdin = open("Fichier_recu.jpg",O_WRONLY | O_CREAT | O_TRUNC,0644))>0){
                time(&start_t);
                while ((sizeRead = Rio_readnb(&rio, bufFile, MAXBLOCK)) != 0 && countFileSize !=sizeFileSend) {
                    countFileSize += rio_writen(fdin, bufFile,sizeRead);
                }
                time(&end_t);
                close(fdin);

                diff_t = difftime(end_t, start_t);

                printf("Le fichier a été envoyé avec succes\n");
                printf("%lu bytes reçus en %f ms\n",countFileSize,diff_t);
                //exit(0);
            }else{
                printf("Problème ouverture fichier\n");
            }
        }else{
            printf("%lu\n",sizeFileSend);
            printf("Le fichier demandé n'existe pas sur le serveur\n");
            exit(0);
        }
        printf("Rentrer un nom de fichier :\n");
    }
    Close(clientfd);
    exit(0);
}
