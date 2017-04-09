/*
 * echoclient.c - An echo client
 */
#include "csapp.h"
#include <sys/time.h>
#define MAX_NAME_LEN 256

void sendFile(int connfd);

// INFORMATIONS POUR LES CLIENTS
    typedef struct {
      struct sockaddr_in clientaddr;
      char client_ip_string[INET_ADDRSTRLEN];
      char client_hostname[MAX_NAME_LEN];
      int nbOctetsSent;
    }infos_client;

int main(int argc, char **argv)
{
    int masterfd,clientfd;
    int port_master = 4000; int port_client = 4001;
    char *host;
    int sizeClient;
    infos_client clientInfos;
    rio_t rio;
    fd_set readset;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    masterfd = Open_clientfd(host, port_master);

    printf("Connecté au Master server\n");
    printf("En attente de clients...\n");
   FD_ZERO(&readset);
   FD_SET(masterfd, &readset);
   if (select (FD_SETSIZE, &readset, NULL, NULL, NULL) < 0){
       perror ("select");
       exit (EXIT_FAILURE);
   }

    Rio_readinitb(&rio,masterfd);
    if((sizeClient = Rio_readnb(&rio, &clientInfos,sizeof(infos_client)) != 0)){
          printf("Client %s reçu, atttente connexion...\n",clientInfos.client_hostname);
          clientfd = Open_clientfd(clientInfos.client_hostname, port_client);

          FD_ZERO(&readset);
          FD_SET(clientfd,&readset);

          if (select(FD_SETSIZE,&readset, NULL, NULL,NULL)>0){
            if(FD_ISSET(clientfd,&readset)){
              printf("Server connected to client %s (%s)\n", clientInfos.client_hostname,clientInfos.client_ip_string);
              int connected;
              read(clientfd,&connected,sizeof(connected));
              while(1){
                sendFile(clientfd);
              }

            }
        }else{
          exit(0);
        }
}
   exit(0);
}
