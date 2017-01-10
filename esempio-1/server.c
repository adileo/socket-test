#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

  	connection = accept(sock, (struct sockaddr*)NULL, NULL); //Su stessa porta e indirizzo, lasciando inalterato sock

    //ticks = time(NULL);
    snprintf(buffer, sizeof(buffer), "abcdefghilm"); 
    printf("abcdefghilm\n");
    write(connection, buffer, strlen(buffer));
    close(connection);
  }
}
