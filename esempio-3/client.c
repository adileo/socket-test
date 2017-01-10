#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char** argv){
  char* ipv4;
  int port = 0;
	int sock;
	struct sockaddr_in serveraddress;

  if (argc == 3){
    ipv4 = argv[1];
    port = atoi(argv[2]);
  }else{
    char tmps[100];
    printf("Indirizzo del server <IPv4, Porta>:\n");
    scanf("%s %d", tmps, &port);
    ipv4 = &tmps;
  }
  
  struct header_ip iphead;
  
  iphead.ip_version = 0b0111;
  iphead.ip_hdr_len = 0b0111;
  iphead.ip_tos     = 0b0111;
  iphead.ip_len     = 0b0111;
  iphead.ip_id      = 0b0111;
  iphead.ip_flag    = 0b0111;
  iphead.ip_offset  = 0b0111;
  iphead.ip_ttl     = 0b0111;
  iphead.ip_proto   = 0b0111;
  iphead.ip_chk     = 0b0111;
  
  // Trasformo l'indirizzo IPv4 puntato da stringa a formato network
  inet_pton(AF_INET, "130.131.100.1", &iphead.ip_src);
  inet_pton(AF_INET, "132.133.200.2", &iphead.ip_dst);
  
  
  // Stampo il datagram costruito
  print_datagram(&iphead);


	// Creazione del socket TCP
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1){
	    printf("Errore nella creazione del socket: %s\n", strerror(errno));
	    exit(1);
  }
  	
	memset(&serveraddress, 0, sizeof(serveraddress));
	serveraddress.sin_family = AF_INET;
	serveraddress.sin_port = htons(port); //Trasformo la porta in formato network
	
  // Trasformo l'IP puntato fornito in input in formato network
	if( inet_pton(AF_INET, ipv4, &serveraddress.sin_addr) <= 0 ){
  	printf("Indirizzo IP errato\n");
    exit(1);
	}
	
  // Effettuo la connessione
	if( connect(sock, (struct sockaddr *) &serveraddress, sizeof(serveraddress)) < 0 ){
  	printf("Errore di connessione %s\n", strerror(errno));
    exit(1);
  }

  // Invio il datagram
  write(sock, &iphead, sizeof(iphead));

  // Aspetto il datagram invertito
  if (read(sock, &iphead, sizeof(iphead)) > 0){
    printf("Header invertito ricevuto\n");
    print_datagram(&iphead);
  }

  printf("Indirizzo IP del server è: %s\n", ipv4);

  // Aspetto il numero di porta
  unsigned short int kernel_server_port;
  if (read(sock, &kernel_server_port, sizeof(kernel_server_port)) > 0){
    printf("Numero di porta ricevuto: %d\n", kernel_server_port);
  }

  

	close(sock);

	return 0;
}
