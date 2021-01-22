/*******************************************************************************
 * SERVIDOR no porto 9000, à escuta de novos clientes.  Quando surjem
 * novos clientes os dados por eles enviados são lidos e descarregados no ecran.
 *******************************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
// TCP SERVER Ex1

#define SERVER_PORT     9000
#define BUF_SIZE	1024

void process_client(int fd);
void erro(char *msg);
int n_clientes = 0; 	// *** MEU *** contador de clientes = 0

int main() {
  int fd, client;
  struct sockaddr_in addr, client_addr;
  int client_addr_size;
  char print_address[INET_ADDRSTRLEN];

  bzero((void *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(SERVER_PORT);


  if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	erro("na funcao socket");
  if ( bind(fd,(struct sockaddr*)&addr,sizeof(addr)) < 0)
	erro("na funcao bind");
  if( listen(fd, 5) < 0)
	erro("na funcao listen");
  client_addr_size = sizeof(client_addr);
  while (1) {
    //clean finished child processes, avoiding zombies
    //must use WNOHANG or would block whenever a child process was working
    while(waitpid(-1,NULL,WNOHANG)>0);
    //wait for new connection
    client = accept(fd,(struct sockaddr *)&client_addr,(socklen_t *)&client_addr_size);
    n_clientes++; 	// *** MEU *** contador de clientes +1
    if (client > 0) {
      if (fork() == 0) {
        close(fd);
	inet_ntop(AF_INET, &(client_addr.sin_addr.s_addr), print_address, INET_ADDRSTRLEN);	// *** MEU *** string address
  	printf("Endereco do Cliente: %s ; Porto: %d ; Numero de Clientes: %d \n", print_address, client_addr.sin_port, n_clientes);	// *** MEU *** print info ip:porta e contador
	process_client(client);
        exit(0);
      }
    close(client);
    }
  }
  return 0;
}

void process_client(int client_fd)
{
	int nread = 0;
	char buffer[BUF_SIZE];
	
	nread = read(client_fd, buffer, BUF_SIZE-1);
	buffer[nread] = '\0';
	
	printf("%s\n", buffer);
	fflush(stdout);
	close(client_fd);
}

void erro(char *msg)
{
	printf("Erro: %s\n", msg);
	exit(-1);
}
