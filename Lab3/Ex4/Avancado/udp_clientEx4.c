/**********************************************************************
 * CLIENTE liga ao servidor (definido em argv[1]) no porto especificado  
 * (em argv[2]), escrevendo a palavra predefinida (em argv[3]).
 * USO: >cliente <enderecoServidor>  <porto>  <Palavra>  ###  UDP  ###
 **********************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h> 


//UDP cliente Ex4


#define BUF_SIZE	1024


int main(int argc, char *argv[]) {
  char endServer[100];
  char buffer[BUF_SIZE];
  int fd;
  struct sockaddr_in addr;
  struct hostent *hostPtr;
  int n; 

  if (argc != 4) {
    	printf("cliente <host> <port> <string>\n");
    	exit(-1);
  }

  strcpy(endServer, argv[1]);
  if ((hostPtr = gethostbyname(endServer)) == 0) {
    	puts("Nao consegui obter endereço");
	exit(1);
  }

  bzero((void *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
  addr.sin_port = htons((short) atoi(argv[2]));

  if((fd = socket(AF_INET,SOCK_DGRAM,0)) == -1) {
	printf("socket");
	exit(1);
  }
      
    sendto(fd, argv[3], 1 + strlen(argv[3]), MSG_CONFIRM, (const struct sockaddr *) &addr, (socklen_t) sizeof(addr)); 

    printf("[CLIENTE] Mensagem Enviada.\n"); 
          
    n = recvfrom(fd, (char *)buffer, BUF_SIZE, MSG_WAITALL, (struct sockaddr *) &addr, (socklen_t *) sizeof(addr)); 
    buffer[n] = '\0'; 
    printf("[SERVIDOR] %s\n", buffer); 
  
    close(fd); 
    return 0; 

}

