#include "csapp.h"
#include <dirent.h>
#include "ftpecho.h"

int main(int argc, char **argv)
{
//INITIALISATION DE VARIABLE IMPORTANTES
    int listenfd;
    int port = 4000;
    socklen_t serverlen;
    pid_t pidFils;
    fd_set readset;

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
        infos_client clientInfos;
        close(pipeInfos[1]);

        while(1){
          FD_ZERO(&readset);
          for(int k=0;k<NB_SERVERS;k++){
              FD_SET(tabSlaves[j].connfd,&readset);
          }

          FD_SET(pipeInfos[0],&readset);
          if (select(FD_SETSIZE,&readset, NULL, NULL,NULL)>0){
              if(FD_ISSET(pipeInfos[0],&readset)){
                  read(pipeInfos[0],&(tabClients)[cptClient],sizeof(infos_client));
                  j =0;
                  int tmp = cptClient%NB_SERVERS;
                  /*while(j<NB_SERVERS && tabSlaves[j].status!=LIBRE){j++;}*/
                  if(j<NB_SERVERS){
                    Rio_writen(tabSlaves[tmp].connfd, &(tabClients)[cptClient], sizeof(infos_client));
                    //tabSlaves[j].status=OCCUPE;
                    cptClient++;
                    printf("Infos client transmis au server slaves... \n");
                  }else{
                    printf("Tout les servers sont occupés\n");
                  }
              }
              // INFORME LA FIN DE COMMUNICATION D'UN SERVER AVEC UN CLIENT
              for(int k=0;k<NB_SERVERS;k++){
                  if(FD_ISSET(tabSlaves[k].connfd,&readset)){
                      if(rio_readn(tabSlaves[k].connfd, &clientInfos,sizeof(infos_client))!= 0){
                          tabSlaves[j].status=LIBRE;
                          printf("Libération d'un des serveurs\n");
                      }
                  }
              }
          }
        }
    }
    exit(0);
}
