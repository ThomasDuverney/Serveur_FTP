/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"
#include <sys/stat.h>

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
    struct stat st;
    size_t sizeRead,n;
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

    if((fdin = open(file, O_RDONLY, 0))>0){
            /*Recupere la taille du fichier */
            stat(file, &st);
            size_t size = st.st_size;
            printf("Le fichier a envoyer à une taille de %lu\n",size );
            Rio_writen(connfd,&size, sizeof(size));

            Rio_readinitb(&riof, fdin);
            while ((sizeRead = Rio_readnb(&riof, bufFile, MAXBLOCK)) != 0) {
                Rio_writen(connfd, bufFile, sizeRead);
            }
            //char * buf="Le fichier a été reçu entièrement";
            //Rio_writen(connfd, buf, strlen(buf));
            close(fdin);
    }else{
        char * buf="Le fichier demandé n'existe pas";
        Rio_writen(connfd, buf, strlen(buf));
    }
    free(bufName);
    free(bufFile);

}
