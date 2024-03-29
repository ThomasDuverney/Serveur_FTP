/*
 * echoclient.c - An echo client
 */
#include "csapp.h"
#include "ftpecho.h"
#include <sys/time.h>

int main(int argc, char **argv)
{
    int listenfd, masterfd,serverfd;
    int port_master=4000;
    int port_server = 5001;
    socklen_t serverlen;
    struct sockaddr_in serveraddr;
    char server_ip_string[INET_ADDRSTRLEN];
    char server_hostname[MAX_NAME_LEN];

    char *host;
    char bufName[MAXLINE];
    char bufFile[MAXBLOCK];
    int fdin,decremente;
    size_t maxBlock;
    ssize_t countFileSize = 0;
    ssize_t sizeRead,sizeFileSend;
    //struct stat st;

    struct timespec start, stop;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    //port_server = atoi(argv[2]);
    masterfd = Open_clientfd(host, port_master);
    close(masterfd);
    // CREATION SOCKET ECOUTE ET ATTENTE DU SERVER SLAVE
    serverlen = (socklen_t)sizeof(serveraddr);
    listenfd = Open_listenfd(port_server);
    while((serverfd = Accept(listenfd, (SA *)&serveraddr, &serverlen))<0);

    // RECUPERATION DES INFORMATIONS DU SERVEUR
    Getnameinfo((SA *) &serveraddr, serverlen,server_hostname, MAX_NAME_LEN, 0, 0, 0);
    Inet_ntop(AF_INET, &serveraddr.sin_addr, server_ip_string,INET_ADDRSTRLEN);

    printf("Connecté à mon Serveur FTP\n");
    printf("ftp > get : ");

    while (Fgets(bufName, MAXLINE, stdin) != NULL) {
        // Enleve le caractère de retour chariot
        char * file = malloc(strlen(bufName));
        if(file == NULL){ printf("Erreur d'alocation\n"); exit(0);};
        strncpy(file,bufName,strlen(bufName)-1);

        // Vérifie si la saisie ne correspont pas à la commande bye
        if(strcmp(file,"bye") != 0){
              // Envoi du nom de fichier
              rio_writen(serverfd,file, strlen(bufName)-1);
              // Recupère la taille du nom de fichier
              if((sizeRead = rio_readn(serverfd, &sizeFileSend,sizeof(sizeFileSend))) != 0 && sizeFileSend !=-1){
                if((fdin = open("Fichier_recu.jpg",O_WRONLY | O_CREAT | O_TRUNC,0644))>0){

                    clock_gettime( CLOCK_REALTIME, &start);
                    countFileSize = 0;
                    maxBlock = (sizeFileSend >= MAXBLOCK) ? MAXBLOCK : sizeFileSend;
                    decremente =sizeFileSend;
                    // On envoi le fichier par block jusqu'à ce que la taille
                    while(countFileSize <sizeFileSend && (sizeRead = rio_readn(serverfd,bufFile,maxBlock)) >0) {
                        countFileSize += rio_writen(fdin,bufFile,sizeRead);
                        decremente -=MAXBLOCK;
                        if(decremente < MAXBLOCK){
                            maxBlock = decremente;
                        }
                    }
                   clock_gettime( CLOCK_REALTIME, &stop);
                   close(fdin);
                   double val = (stop.tv_nsec/1000000.0) - (start.tv_nsec/1000000.0);

                   printf("Le fichier a été envoyé avec succes\n");
                   printf("%lu bytes reçus en %f ms\n",countFileSize,val);

                }else{ printf("Problème ouverture fichier\n");}

              }else{printf("Le fichier demandé n'existe pas sur le serveur\n");}

        }else{printf("Deconnexion \n"); exit(0);}
        free(file);
        printf("ftp > get : ");
    }
    Close(serverfd);
    exit(0);
}
