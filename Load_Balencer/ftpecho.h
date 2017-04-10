
#include "csapp.h"
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_NAME_LEN 256
#define SENTGOOD 1
#define SENTBAD 1
#define MAX_NAME_LEN 256
#define NB_SERVERS 2
#define NB_CLIENTS 5
#define OCCUPE 1
#define LIBRE 0

// INFORMATIONS POUR LES CLIENTS
    typedef struct {
      struct sockaddr_in clientaddr;
      char client_ip_string[INET_ADDRSTRLEN];
      char client_hostname[MAX_NAME_LEN];
      int nbOctetsSent;
      int verif;
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


int sendFile(int connfd);
void echo(int connfd);
