/*
 * echoclient.c - An echo client
 */
#include "csapp.h"
#include <sys/time.h>
#define MAX_NAME_LEN 256

int main(int argc, char **argv)
{
    int listenfd, masterfd,serverfd;
    int port_master=4000;
    int port_server = 5001;
    socklen_t serverlen;
    struct sockaddr_in serveraddr;
    char server_ip_string[INET_ADDRSTRLEN];
    char server_hostname[MAX_NAME_LEN];

    char *host, bufName[MAXLINE],bufFile[MAXBLOCK];
    int fdin;
    fd_set readset;
    ssize_t countFileSize = 0;
    ssize_t sizeRead,sizeFileSend;

    time_t start_t, end_t;
    double diff_t;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    masterfd = Open_clientfd(host, port_master);
    close(masterfd);
    // CREATION SOCKET ECOUTE ET ATTENTE DU SERVER SLAVE
    serverlen = (socklen_t)sizeof(serveraddr);
    listenfd = Open_listenfd(port_server);
    while((serverfd = Accept(listenfd, (SA *)&serveraddr, &serverlen))<0);

    // RECPERATION DES INFORMATIONS SUR LE SERVEUR
    Getnameinfo((SA *) &serveraddr, serverlen,server_hostname, MAX_NAME_LEN, 0, 0, 0);
    Inet_ntop(AF_INET, &serveraddr.sin_addr, server_ip_string,INET_ADDRSTRLEN);

    //ENVOI D'UN OCTET POUR CONFIRMER LA CONNEXION
    int connected = 1;
    write(serverfd,&connected, sizeof(connected));

    printf("Connecté à mon Serveur FTP\n");
    printf("ftp > get : ");

    while (Fgets(bufName, MAXLINE, stdin) != NULL) {

        write(serverfd, bufName, strlen(bufName));
        FD_ZERO(&readset);
        FD_SET(serverfd,&readset);

        if (select(FD_SETSIZE,&readset, NULL, NULL,NULL)>0){
          if(FD_ISSET(serverfd,&readset)){
            sizeRead = read(serverfd, &sizeFileSend,sizeof(sizeFileSend));
            printf("%lu\n",sizeFileSend);
            printf("%lu\n",sizeRead);
            if(sizeRead!= 0 && sizeFileSend !=-1){
              if((fdin = open("Fichier_recu.jpg",O_WRONLY | O_CREAT | O_TRUNC,0644))>0){
                  time(&start_t);

                  while ((sizeRead = read(serverfd, bufFile, MAXBLOCK)) <= 0 && countFileSize !=sizeFileSend) {
                      countFileSize += write(fdin, bufFile,sizeRead);
                  }
                  time(&end_t);
                  close(fdin);
                  diff_t = difftime(end_t, start_t);

                  printf("Le fichier a été envoyé avec succes\n");
                  printf("%lu bytes reçus en %f ms\n",countFileSize,diff_t);

              }else{
                  printf("Problème ouverture fichier\n");
              }
            }else{
                printf("Le fichier demandé n'existe pas sur le serveur\n");
            }
          }
        }
        printf("ftp > get : ");
    }
    Close(serverfd);
    exit(0);
}
