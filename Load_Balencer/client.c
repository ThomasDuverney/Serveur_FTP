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
    int port_server = 4001;
    socklen_t serverlen;
    struct sockaddr_in serveraddr;
    char server_ip_string[INET_ADDRSTRLEN];
    char server_hostname[MAX_NAME_LEN];

    char *host, bufName[MAXLINE],bufFile[MAXBLOCK];
    rio_t rio;
    int fdin;
    ssize_t countFileSize = 0;
    size_t sizeRead,sizeFileSend;

    time_t start_t, end_t;
    double diff_t;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    masterfd = Open_clientfd(host, port_master);

    // CREATION SOCKET ECOUTE ET ATTENTE DU SERVER SLAVE
    serverlen = (socklen_t)sizeof(serveraddr);
    listenfd = Open_listenfd(port_server);
    while((serverfd = Accept(listenfd, (SA *)&serveraddr, &serverlen))<0);

    printf("Connecté à mon Serveur FTP\n");
    printf("ftp > get : ");
    Rio_readinitb(&rio, serverfd);

    while (Fgets(bufName, MAXLINE, stdin) != NULL) {

        Rio_writen(serverfd, bufName, strlen(bufName));

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
                exit(0);
            }else{
                printf("Problème ouverture fichier\n");
            }
        }else{
            printf("%lu\n",sizeFileSend);
            printf("Le fichier demandé n'existe pas sur le serveur\n");
            exit(0);
        }
        printf("ftp > get : ");
    }
    Close(serverfd);
    exit(0);
}
