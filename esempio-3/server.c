#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct header_ip{
  // con i due punti riduco la dimensione del type
  // char  = 1 byte, uint8_t
  // short = 2 byte, uint16_t
  unsigned char ip_version:4;   
  unsigned char ip_hdr_len:4;   
  unsigned char ip_tos;
  unsigned short int ip_len;
  unsigned short int ip_id;
  unsigned char ip_flag:3;
  unsigned short int ip_offset:13; 
  unsigned char ip_ttl;
  unsigned char ip_proto;
  unsigned short int ip_chk;
  struct in_addr ip_src;
  struct in_addr ip_dst;
};


void print_datagram(struct header_ip * datagram){
  printf("{\n");
  printf("  ip_version: %d\n", datagram->ip_version);
  printf("  ip_hdr_len: %d\n", datagram->ip_hdr_len);
  printf("  ip_tos: %d\n", datagram->ip_tos);
  printf("  ip_len: %d\n", datagram->ip_len);
  printf("  ip_id: %d\n", datagram->ip_id);
  printf("  ip_flag: %d\n", datagram->ip_flag);
  printf("  ip_offset: %d\n", datagram->ip_offset);
  printf("  ip_ttl: %d\n", datagram->ip_ttl);
  printf("  ip_proto: %d\n", datagram->ip_proto);
  printf("  ip_chk: %d\n", datagram->ip_chk);
  printf("  ip_src: %d\n", datagram->ip_src);
  printf("  ip_dst: %d\n", datagram->ip_dst);
  printf("}\n");
}

int main(){

  int sock, connection;
  
  // Creazione del socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == -1){
    printf("Errore nella creazione del socket: %s\n", strerror(errno));
    exit(1);
  }

  // Collego il socket ad una porta
  struct sockaddr_in localadress;
  // Resetto la struct
  memset(&localadress, 0, sizeof(localadress));

  localadress.sin_family = AF_INET; 
  localadress.sin_addr.s_addr = htonl(INADDR_ANY); // da tutti gli ip
  localadress.sin_port = htons(90000); // sulla porta 90000

  if(bind(sock, (struct sockaddr*)&localadress, sizeof(localadress)) == -1){
    printf("Errore nel bind: %s\n", strerror(errno));
    exit(1);
  }

  //Listen: Mi metto in ascolto fino a un max di x connessioni
  if (listen(sock, 100) == -1){
    printf("Errore nell'avvio del listen: %s\n", strerror(errno));
    exit(1);
  }

  printf("Server Avviato!\n");
  

  while(1){
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    connection = accept(sock, (struct sockaddr*) &client, (socklen_t*) &len); //Su stessa porta e indirizzo, lasciando inalterato sock
    printf("Ricevuta richiesta di connessione da indirizzo: %s \n", inet_ntoa(client.sin_addr));

    if (connection == -1){
      printf("Errore nell'accettare la connessione: %s\n", strerror(errno));
    }

    int pid;
    if ((pid = fork()) > 0){

      // Processo padre
      close(connection);
      continue;

    } else if (pid == 0){
      // Processo figlio
      printf("Porta assegnata dal kernel: %d\n",ntohs(client.sin_port));

      struct header_ip iphead;
      
      if(read(connection, &iphead, sizeof(iphead)) == -1){
        printf("Errore nell'ricevere il datagram: %s\n", strerror(errno));
        exit(1);
      }

      printf("Header ricevuto\n");
      
      // Switch src <-> dst
      struct in_addr temp = iphead.ip_src;
      iphead.ip_src = iphead.ip_dst;
      iphead.ip_dst = temp;

      iphead.ip_version  = 0b0;
      iphead.ip_hdr_len  = 0b0;
      iphead.ip_tos      = 0b0;
      iphead.ip_len      = 0b0;
      iphead.ip_id       = 0b0;
      iphead.ip_flag     = 0b0;
      iphead.ip_offset   = 0b0;
      iphead.ip_ttl      = 0b0;
      iphead.ip_proto    = 0b0;
      iphead.ip_chk      = 0b0;

      print_datagram(&iphead);

      // Invio il datagram invertito
      if(write(connection, &iphead, sizeof(iphead)) == -1){
        printf("Errore nell'inviare il datagram invertito: %s\n", strerror(errno));
        exit(1);
      }
            
      unsigned short int kernel_server_port = ntohs(client.sin_port);
        
      if(write(connection, &kernel_server_port, sizeof(kernel_server_port)) == -1){
        printf("Errore nell'inviare la porta del kernel: %s\n", strerror(errno));
        exit(1);
      }


      if(read(connection, NULL, NULL) == 0){
        printf("Connessione chiusa dal client: %s\n", inet_ntoa(client.sin_addr));
        close(connection);
      }

      }else{
            
            printf("Errore nel fork\n");
          
      }
    

  }
  close(connection);
}
