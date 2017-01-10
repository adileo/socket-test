#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

void print_bytes(const void *object, size_t size)
{
  const unsigned char * const bytes = object;
  size_t i;

  printf("[ ");
  for(i = 0; i < size; i++)
  {
    //printf("%02x ", bytes[i]);
    int z;
    for (z = 0; z < 8; z++) {
      printf("%d", !!((bytes[i] << z) & 0x80));
    }
    printf(" ");
  }
  printf("]\n");
}

//SERVER
int main(){

  int sock, connection;
  
  //Creazione del socket
  printf("Creo il socket!\n");
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == -1){
    //Errore
    printf("socket error\n");
    exit(-1);
  }
  //Bind: Colleghiamo il socket ad un indirizzo
  struct sockaddr_in localadress;
  memset(&localadress, 0, sizeof(localadress)); //Setto a 0 tutti i campi della struct localadress
  localadress.sin_family = AF_INET; 
  localadress.sin_addr.s_addr = htonl(INADDR_ANY); //Accetto tutti gli ip
  localadress.sin_port = htons(60000);
  int b;
  printf("Bindo!\n");
  b = bind(sock, (struct sockaddr*)&localadress, sizeof(localadress)); //Cast esplicito a puntatore a struttura
  if(b == -1){
    //Errore nel bind
  }
  printf("Ascolto!\n");
  //Listen: Mi metto in ascolto fino a un max di x connessioni
  listen(sock, 100);
  printf("In ascolto!\n");
  
  char buffer[255];
  time_t ticks;

  while(1){
    /*
       It extracts the first connection
       request on the queue of pending connections for the listening socket,
       sockfd, creates a new connected socket, and returns a new file
       descriptor referring to that socket.  The newly created socket is not
       in the listening state.  The original socket sockfd is unaffected by
       this call.
    */
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    connection = accept(sock, (struct sockaddr*) &client, (socklen_t*) &len); //Su stessa porta e indirizzo, lasciando inalterato sock
    
    printf("%s\n",inet_ntoa(client.sin_addr));
    int pid;
    if ((pid = fork()) == -1)
        {
            close(connection);
            continue;
        }
        else if(pid > 0)
        {
            //printf("got connection from %s", &client_address);
            close(connection);
            //counter++;
            //printf("here2\n");
            continue;
        }
        else if(pid == 0)
        {
          printf("%d\n",ntohs(client.sin_port));

          struct header_ip{
            //char = 1 byte, uint8_t
            //short = 2 byte, uint16_t
            unsigned char ip_version:4;   //!< The IP version.
            unsigned char ip_hdr_len:4;   //!< The IP header length.
            unsigned char ip_tos;
            unsigned short int ip_len;
            unsigned short int ip_id;
            unsigned char ip_flag:3;
            unsigned short int ip_offset:13; //Offset
            unsigned char ip_ttl;
            unsigned char ip_proto;
            unsigned short int ip_chk;
            struct in_addr ip_src;
            struct in_addr ip_dst;
          };
          struct header_ip iph;
          int n = read(connection, &iph, sizeof(iph));
          if(n > 0){
            printf("Header ricevuto\n");
            struct in_addr temp = iph.ip_src;
            iph.ip_src = iph.ip_dst;
            iph.ip_dst = temp;
            iph.ip_version = 0b0;
            iph.ip_hdr_len = 0b0;
            iph.ip_tos = 0b0;
            iph.ip_len = 0b0;
            iph.ip_id = 0b0;
            iph.ip_flag = 0b0;
            iph.ip_offset = 0b0;
            iph.ip_ttl = 0b0;
            iph.ip_proto = 0b0;
            iph.ip_chk = 0b0;
            print_bytes(&iph, sizeof(iph));

            write(connection, &iph, sizeof(iph));
            
            unsigned short int porta = ntohs(client.sin_port);
            write(connection, &porta, sizeof(porta));

            n = read(connection, NULL, NULL);
            if(n == 0){
              printf("Connessione chiusa dal client: %s\n", inet_ntoa(client.sin_addr));
              close(connection);
            }

          }else{
            printf("Errore: %s\n", strerror(errno));
          }
        }
    

  }
}
