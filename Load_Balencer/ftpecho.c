/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_NAME_LEN 256

void echo(int connfd)
{
    ssize_t n;
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
    int fdin,clientAlive;
    struct stat st;
    ssize_t sizeRead,size, n;
    fd_set readset;
    struct timeval timeout;
    timeout.tv_sec =20;
/*-----------------------------------------*/
/*-----------------------------------------*/

/*----RECUPERE LE FICHIER DEMANDE PAR LE CLIENT----*/
FD_ZERO(&readset);
FD_SET(connfd,&readset);

if (select(FD_SETSIZE,&readset, NULL, NULL,NULL)>0){
  if(FD_ISSET(connfd,&readset)){
        if ((n = read(connfd, bufName, MAXLINE)) != 0){
          printf("La taille %lu\n",n);
          char * file = malloc(n);
          if(file == NULL){ printf("Erreur d'alocation\n"); exit(0);};
          strncpy(file,bufName,n-1);
          printf("Le client demande le fichier : %s\n",file);
/*-------------------------------------------------*/
          if((fdin = open(file, O_RDONLY, NULL))>0){
                  /*Recupere la taille du fichier */
                  printf("pouet");
                  stat(file, &st);
                  size = st.st_size;
                  printf("Le fichier a envoyer à une taille de %lu\n",size);
                  write(connfd,&size, sizeof(size));
                  clientAlive = 1;

                  while ((sizeRead = read(fdin, bufFile, MAXBLOCK)) !=0  && clientAlive !=0) {
                      if(read(connfd, bufFile, sizeRead) == -1){
                          printf("La connexion a un client a été perdu");
                          clientAlive = 0;
                      }
                  }
                  close(fdin);
                  printf("Le fichier a été envoyé !");
          }else{
                  printf("Le fichier n'existe pas !");
                  size = -1;
                  write(connfd,&size, sizeof(size));
          }
          free(file);
        }

      }
    }
    free(bufName);
    free(bufFile);

}
