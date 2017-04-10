#include "ftpecho.h"
void echo(int connfd)
{
    ssize_t n;
    char buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        printf("server received %u bytes\n", (unsigned int)n);
        printf("Le client a écrit \n: %s",buf);
        write(connfd, buf, n);
    }
}

int sendFile(int connfd)
{
/*------INITIALISATION DES VARIABLES--------*/
/*-----------------------------------------*/
    char bufName[MAXLINE];
    char bufFile[MAXLINE];
    int fdin,clientAlive;
    struct stat st;
    ssize_t sizeRead,size, n, maxBlock;
    size_t sizewrite;
/*-----------------------------------------*/
/*-----------------------------------------*/
    
    if ((n = read(connfd, bufName, MAXLINE)) > 0){
      if(strcmp(bufName,"bye") != 0){

          printf("Le client demande le fichier : %s\n",bufName);

          if((fdin = open(bufName, O_RDONLY, NULL))>0){
                  /*Recupere la taille du fichier */
                  stat(bufName, &st);
                  size = st.st_size;
                  printf("Le fichier a envoyer à une taille de %lu\n",size);
                  Rio_writen(connfd,&size, sizeof(size));

                  clientAlive = 1;
                  maxBlock = (size >= MAXBLOCK) ? MAXBLOCK : size;

                  int decremente =size;
                  while((sizeRead = rio_readn(fdin, bufFile, maxBlock)) !=0  && decremente > 0 && clientAlive !=0) {

                      if((sizewrite = rio_writen(connfd, bufFile, (size_t)sizeRead)) == -1){
                          printf("La connexion a un client a été perdu\n");
                          clientAlive = 0;
                          return 0;
                      }
                      decremente -=MAXBLOCK;
                      if(decremente < MAXBLOCK){
                          maxBlock = decremente;
                      }
                  }
                  close(fdin);
                  printf("Le fichier a été envoyé !\n");
                  return 1;
          }else{
                  printf("Le fichier n'existe pas !\n");
                  size = -1;
                  Rio_writen(connfd,&size, sizeof(size));
                  return 1;
          }

      }else{ return 0; }

  }else{ return 0; }
}
