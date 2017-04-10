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
    pid_t pidFils;
    fd_set readset,readServer;
    //rio_t rio;
// INFORMATIONS POUR LES CLIENTS
    typedef struct {
      struct sockaddr_in clientaddr;
      char client_ip_string[INET_ADDRSTRLEN];
      char client_hostname[MAX_NAME_LEN];
      int nbOctetsSent;
    }infos_client;

    int connfd;

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

    int pipeInfos[2];
    pipe(pipeInfos);

    if ((pidFils = fork()) == 0){
        close(pipeInfos[0]);
        // BOUCLE POUR ATTENDRE LES CLIENTS
        while (1) {
          infos_client client;
          while((connfd = Accept(listenfd, (SA *) &client.clientaddr, &serverlen))<0);
          /* determine the name of the server */
          Getnameinfo((SA *) &client.clientaddr,serverlen,client.client_hostname, MAX_NAME_LEN, 0, 0,0);
          /* determine the textual representation of the server's IP address */
          Inet_ntop(AF_INET, &client.clientaddr.sin_addr,client.client_ip_string,INET_ADDRSTRLEN);

          printf("Master server connected to client %s (%s)\n", client.client_hostname,client.client_ip_string);
          Close(connfd);
          write(pipeInfos[1],&client,sizeof(infos_client));
        }
    }else{
        infos_client tabClients[NB_CLIENTS];
        int cptClient,j = 0;
        struct timeval tv;
        rio_t rio;
        infos_client clientInfos;
        close(pipeInfos[1]);

        while(1){
            tv.tv_sec = 1;
            tv.tv_usec = 0;
          FD_ZERO(&readset);
          FD_SET(pipeInfos[0],&readset);
          if (select(FD_SETSIZE,&readset, NULL, NULL,&tv)>0){

                read(pipeInfos[0],&(tabClients)[cptClient],sizeof(infos_client));
                cptClient++;
                j =0;
                while(j<NB_SERVERS && tabSlaves[j].status!=LIBRE){j++;}
                if(j<NB_SERVERS){
                  Rio_writen(tabSlaves[j].connfd, &(tabClients)[j], sizeof(infos_client));
                  tabSlaves[j].status=OCCUPE;
                  printf("Infos client transmis au server slaves... \n");
                }else{
                  printf("Tout les servers sont occupés\n");
                }
          }
          for(int k=0;k<NB_SERVERS;k++){
              FD_ZERO(&readServer);
              FD_SET(tabSlaves[j].connfd,&readServer);
          }
          tv.tv_sec = 1;
          tv.tv_usec = 0;
          printf(" LOL\n");
          if (select(FD_SETSIZE,&readset, NULL, NULL,&tv)>0){
              for(int k=0;k<NB_SERVERS;k++){
                  printf("Youpi\n");
                  Rio_readinitb(&rio,tabSlaves[j].connfd);
                  if(rio_readnb(&rio, &clientInfos,sizeof(infos_client)) != 0){
                      tabSlaves[j].status=LIBRE;
                      printf("Libération d'un des serveurs\n");
                  }
              }
          }
        }
    }
    exit(0);
}
