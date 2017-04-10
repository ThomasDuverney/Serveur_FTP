#include "ftpecho.h"
#include "csapp.h"
#include <sys/time.h>
#define MAX_NAME_LEN 256
#define CONNECTED 1
#define DECONNECTED 0
#define NOCONNEXION 2

int main(int argc, char **argv)
{
    int masterfd,clientfd;
    int port_master = 4000; int port_client = 5001;
    char *host;
    int sizeClient;
    infos_client clientInfos;
    rio_t rio;

    int etat = NOCONNEXION;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    masterfd = Open_clientfd(host, port_master);
    Rio_readinitb(&rio,masterfd);
    printf("Connecté au Master server\n");
    int i=0;
    // On créer les fils 
    while(i <NB_CLIENTS && fork() != 0){
        i++;
    }
    while(1){
        // SI IL N'Y A AUCUNE CONNEXION SUR LE SERVER
        if(etat == NOCONNEXION){
            printf("En attente de clients...\n");
            if((sizeClient = rio_readnb(&rio, &clientInfos,sizeof(infos_client))) != 0){

                  printf("Client %s reçu, attente connexion...\n",clientInfos.client_hostname);
                  clientfd = Open_clientfd(clientInfos.client_hostname, port_client);
                  printf("Server connected to client %s (%s)\n", clientInfos.client_hostname,clientInfos.client_ip_string);
                  etat = sendFile(clientfd);
            }
        // SI UN CLIENT SE DECONNECT ON ENVOI UN MESSAGE AU MASTER
        }else if(etat == DECONNECTED){
                printf("Deconnexion du client...\n");
                close(clientfd);
                rio_writen(masterfd,&clientInfos,sizeof(infos_client));
                etat = NOCONNEXION;
        // SI LE CLIENT EST TOUJOURS CONNECTÉ ET QU'IL REDEMANDE UN FICHIER
        }else{
            etat = sendFile(clientfd);

        }

    }
   exit(0);
}
