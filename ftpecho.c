/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"

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
    size_t n;
    char * bufName = malloc(MAXLINE);
    char * bufFile = malloc(MAXLINE);
    rio_t rios, riof;
    int fdin;

    Rio_readinitb(&rios, connfd);
    n = Rio_readlineb(&rios, bufName, MAXLINE);

    printf("Le client demande le fichier \n: %s",bufName);
    char * file = malloc(n);
    if(file == NULL){ printf("Erreur d'alocation"); exit(0);};
    strncpy(file,bufName,n-1);
    printf("file '%lu' '%s' '%s'\n", n,file, bufName);

    if((fdin = open(file, O_RDONLY, 0))>0){

            Rio_readinitb(&riof, fdin);
            while ((n = Rio_readlineb(&riof, bufFile, MAXLINE)) != 0) {
                Rio_writen(connfd, bufFile, n);
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
