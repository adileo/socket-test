#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv){
	int sock;
  char buffer[5+1];
  
  if(argc!=2){printf("Stronzo devi fornirmi un IP\n"); exit(-1);}
  
  //Creazione del Socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == -1){
  	//Errore
    printf("socket error\n");
    exit(-1);
  }
  
  //Connect: connettiamoci al server
  struct sockaddr_in serveraddress;
  memset(&serveraddress, 0, sizeof(serveraddress));
  serveraddress.sin_family = AF_INET;
  serveraddress.sin_port = htons(60000);
  
  	//Trasformo l'indirizzo IP passato come argomento al programma e lo trasforma in indirizzo salvandolo in serveraddress.sin_addr
  if( inet_pton(AF_INET, argv[1], &serveraddress.sin_addr) <= 0 ){
  	printf("Il tuo indirizzo IP che mi hai passato puzza\n");
    exit(-3);
  }
  
  if( connect(sock, (struct sockaddr *) &serveraddress, sizeof(serveraddress)) < 0 ){
  	printf("Errore di connessione\n");
    printf("Qualcosa è andato a troie! %s\n", strerror(errno));
    exit(-4);
  }
 
  int n;
  while( (n = read(sock, buffer, 5)) > 0){
  	buffer[n] = 0; //Metto il terminatore nel buffer, ad esempio se leggo 100 caratteri dopo il 100esimo ho spazzatura in memoria
    if (fputs(buffer, stdout) == EOF){
     	printf("fputs error\n");
      exit(-5);
    }
    const char *b = "\r\n";
    fputs(b, stdout);
  }
  if(n < 0){
    printf("Read Error\n");
    exit(-6);
  }
  exit(0);
  
}
