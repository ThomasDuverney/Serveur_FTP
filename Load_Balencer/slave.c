/*
 * echoclient.c - An echo client
 */
#include "csapp.h"
#include <sys/time.h>
#define MAX_NAME_LEN 256
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

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    masterfd = Open_clientfd(host, port_master);

    printf("Connecté au Master server\n");
    printf("En attente de clients...\n");
    Rio_readinitb(&rio,masterfd);
    while(1){
          while((sizeClient = Rio_readnb(&rio, &clientInfos,sizeof(infos_client)) != sizeof(infos_client)));
          clientfd = Open_clientfd(clientInfos.client_hostname, port_client);
          printf("Server connected to client %s (%s)\n", clientInfos.client_hostname,clientInfos.client_ip_string);
          sendFile(clientfd);
    }
   exit(0);
}
