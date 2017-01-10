#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

//CLIENT
void print_bytes(const void *object, size_t size)
{
  const unsigned char * const bytes = object;
  size_t i;

  printf("[ ");
  for(i = 0; i < size; i++)
  {
    //Stampo esadecimale
    //printf("%02x ", bytes[i]);
    int z;
    for (z = 0; z < 8; z++) {
      printf("%d", !!((bytes[i] << z) & 0x80));
    }
    printf(" ");
  }
  printf("]\n");
}

int main(int arcg, char** argv){
	int sock;
	struct sockaddr_in serveraddress;
	
	//creazione socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1){
	    printf("socket error\n");
	    exit(-1);
  	}
  	
  	memset(&serveraddress, 0, sizeof(serveraddress));
  	serveraddress.sin_family = AF_INET;
  	//serveraddress.sin_addr.s_addr = inet_addr("127.0.0.1"); //indirizzo IP dell'host a cui vogliamo connetterci
  	serveraddress.sin_port = htons(60000);
  	
  	if( inet_pton(AF_INET, argv[1], &serveraddress.sin_addr) <= 0 ){//l'idirizzo IP del server ci è stato dato in input
	  	printf("indirizzo IP error\n");
	    exit(-3);
  	}
  	
  	//connect(mysocket, (struct sockaddr *)&serveraddress, sizeof(serveraddress));
	
	if( connect(sock, (struct sockaddr *) &serveraddress, sizeof(serveraddress)) < 0 ){
	  	printf("Errore di connessione %s\n", strerror(errno));
      
	    exit(-4);
  	}
  	
	//funzione

  //Costruiamo DATAGRAM
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
  //__attribute__((packed))
  
  struct header_ip iph;
  
  iph.ip_version = 0b0111;
  iph.ip_hdr_len = 0b0111;
  iph.ip_tos = 0b0111;
  iph.ip_len = 0b0111;
  iph.ip_id = 0b0111;
  iph.ip_flag = 0b0111;
  iph.ip_offset = 0b0111;
  iph.ip_ttl = 0b0111;
  iph.ip_proto = 0b0111;
  iph.ip_chk = 0b0111;
  
  //o inet_aton per fare il controllo dell'indirizzo
  inet_pton(AF_INET, "130.131.100.1", &iph.ip_src);
	inet_pton(AF_INET, "132.133.200.2", &iph.ip_dst);
  
  //printf("%ld\n", iph);
  print_bytes(&iph, sizeof(iph));

  write(sock, &iph, sizeof(iph));

  unsigned short int porta;

  int n = read(sock, &iph, sizeof(iph));
  int m = read(sock, &porta, sizeof(porta));
  if((n && m) > 0){
    printf("Header ricevuto\n");
    print_bytes(&iph, sizeof(iph));
    printf("IP SERVER: %s\n", argv[1]);
    printf("PORTA: %d\n", porta);
    
  }

	close(sock);
	
}
