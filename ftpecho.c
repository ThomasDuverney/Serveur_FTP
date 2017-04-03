/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"
#include <sys/stat.h>
#include <unistd.h>

void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        printf("server received %u bytes\n", (unsigned int)n);
        printf("Le client a écrit \n: %s",buf);
        Rio_writen(connfd, buf, n);
    }
}

void sendFile(int connfd)
{
/*------INITIALISATION DES VARIABLES--------*/
/*-----------------------------------------*/
    char * bufName = malloc(MAXLINE);
    char * bufFile = malloc(MAXLINE);
    rio_t rios, riof;
    int fdin;
    fd_set exceptfds;
    struct stat st;
    size_t sizeRead,size,n;
/*-----------------------------------------*/
/*-----------------------------------------*/
    /* */
    Rio_readinitb(&rios, connfd);

/*----RECUPERE LE FICHIER DEMANDE PAR LE CLIENT----*/

    n = Rio_readlineb(&rios, bufName, MAXLINE);
    printf("Le client demande le fichier \n: %s",bufName);
    char * file = malloc(n);
    if(file == NULL){ printf("Erreur d'alocation"); exit(0);};
    strncpy(file,bufName,n-1);
/*-------------------------------------------------*/

    if((fdin = open(file, O_RDONLY, NULL))>0){
            /*Recupere la taille du fichier */
            stat(file, &st);
            size = st.st_size;
            printf("Le fichier a envoyer à une taille de %lu\n",size);
            Rio_writen(connfd,&size, sizeof(size));

            Rio_readinitb(&riof, fdin);
            FD_ZERO(&exceptfds);
            while ((sizeRead = Rio_readnb(&riof, bufFile, MAXBLOCK)) != 0) {
                select(2,&exceptfds,NULL,NULL,NULL);
                if(FD_ISSET(connfd,&exceptfds)==0){
                  rio_writen(connfd, bufFile, sizeRead);
                }else{
                  printf("pouet");
                }
            }

            close(fdin);
    }else{
            size = 0;
            Rio_writen(connfd,&size, sizeof(size));
    }
    free(bufName);
    free(bufFile);

}
