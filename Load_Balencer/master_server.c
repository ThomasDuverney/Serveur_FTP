#include "csapp.h"
#include <dirent.h>

#define MAX_NAME_LEN 256
#define NB_SERVERS 2
#define NB_CLIENTS 5
#define OCCUPE 1
#define LIBRE 0

int main(int argc, char **argv)
{
//INITIALISATION DE VARIABLE IMPORTANTES
    int listenfd;
    int port = 4000;
    socklen_t serverlen;
    //rio_t rio;
// INFORMATIONS POUR LES CLIENTS
    typedef struct {
      struct sockaddr_in clientaddr;
      char client_ip_string[INET_ADDRSTRLEN];
      char client_hostname[MAX_NAME_LEN];
      int nbOctetsSent;
    }infos_client;

    int connfd;
    infos_client tabClients[NB_CLIENTS];

// INFORMATIONS POUR LES SERVERS ESCLAVES
    typedef struct {
      struct sockaddr_in serveraddr;
      char server_ip_string[INET_ADDRSTRLEN];
      char server_hostname[MAX_NAME_LEN];
      int connfd;
      int status;
    }infos_slaves;

    infos_slaves tabSlaves[NB_SERVERS];

//OUVERTURE DE LA SOCKET D'ECOUTE
    serverlen = (socklen_t)sizeof(tabSlaves[0].serveraddr);
    listenfd = Open_listenfd(port);

// BOUCLE POUR ATTENDRE TOUS LES SLAVES
    printf("En attente des slaves...\n");
    int i =0;
    while(i <NB_SERVERS){

          while((tabSlaves[i].connfd = Accept(listenfd, (SA *)&(tabSlaves[i]).serveraddr, &serverlen))<0);
          /* determine the name of the server */
          Getnameinfo((SA *) &(tabSlaves[i]).serveraddr,serverlen,tabSlaves[i].server_hostname, MAX_NAME_LEN, 0, 0,0);

          /* determine the textual representation of the server's IP address */
          Inet_ntop(AF_INET, &(tabSlaves[i]).serveraddr.sin_addr,tabSlaves[i].server_ip_string,INET_ADDRSTRLEN);
          tabSlaves[i].status = LIBRE;
          printf("Master server connected to %s (%s)\n", tabSlaves[i].server_hostname,tabSlaves[i].server_ip_string);
          i++;
    }
    printf("Les servers slaves sont prêts \n");
    printf("En attente de clients... \n");
// BOUCLE POUR ATTENDRE TOUS LES CLIENTS
    while (1) {

          while((connfd = Accept(listenfd, (SA *)&(tabClients[i]).clientaddr, &serverlen))<0);
          /* determine the name of the server */
          Getnameinfo((SA *) &(tabClients[i]).clientaddr,serverlen,tabClients[i].client_hostname, MAX_NAME_LEN, 0, 0,0);

          /* determine the textual representation of the server's IP address */
          Inet_ntop(AF_INET, &(tabClients[i]).clientaddr.sin_addr,tabClients[i].client_ip_string,INET_ADDRSTRLEN);

          printf("Master server connected to client %s (%s)\n", tabClients[i].client_hostname,tabClients[i].client_ip_string);
          Close(connfd);

          int j =0;
          while(j<NB_SERVERS && tabSlaves[j].status!=LIBRE){j++;}
          if(j<NB_SERVERS){
            //Rio_readinitb(&rio, tabSlaves[j].connfd);
            Rio_writen(tabSlaves[j].connfd, &(tabSlaves)[j], sizeof(infos_client));
            tabSlaves[j].status=OCCUPE;
            printf("Infos client transmis au server slaves... \n");
          }

    }
    exit(0);
}
