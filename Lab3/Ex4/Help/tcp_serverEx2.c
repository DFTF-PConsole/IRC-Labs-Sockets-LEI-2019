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
#include <ctype.h>

#define SERVER_PORT     9000
#define BUF_SIZE	1024

void process_client(int fd);
void erro(char *msg);

int main() {
  int fd, client;
  struct sockaddr_in addr, client_addr;
  int client_addr_size;
  char print_address[INET_ADDRSTRLEN];
  bzero((void *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(SERVER_PORT);
  unsigned long addr_ntohl;
  

  if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	erro("na funcao socket");
  if ( bind(fd,(struct sockaddr*)&addr,sizeof(addr)) < 0)
	erro("na funcao bind");
  if( listen(fd, 5) < 0)
	erro("na funcao listen");
  addr_ntohl =  ntohl(addr.sin_addr.s_addr);
  inet_ntop(AF_INET, &addr_ntohl, print_address, INET_ADDRSTRLEN);
  printf("Endereco do Servidor: %s ; Porto: %d \n", print_address, ntohs(addr.sin_port));
  client_addr_size = sizeof(client_addr);
  while (1) {
    //clean finished child processes, avoiding zombies
    //must use WNOHANG or would block whenever a child process was working
    while(waitpid(-1,NULL,WNOHANG)>0);
    //wait for new connection
    client = accept(fd,(struct sockaddr *)&client_addr,(socklen_t *)&client_addr_size);
    if (client > 0) {
      if (fork() == 0) {
        close(fd);
	inet_ntop(AF_INET, &(client_addr.sin_addr.s_addr), print_address, INET_ADDRSTRLEN);
  	printf("\n\n***** Endereco do Cliente: %s ; Porto: %d *****\n\n", print_address, client_addr.sin_port);
        process_client(client);
        exit(0);
      }
    close(client);
    }
  }
  return 0;
}

void process_client(int client_fd) {
	int nread = 0;
	int n_char;
	char my_msg[BUF_SIZE-2];
	char buffer[BUF_SIZE];
	int factor, special_factor = 0;
	int i, counter, acumula, meu_erro, omg;
	int array[11];
	char temp[11];
	double media;

	while (1) {
	bzero(my_msg, BUF_SIZE -2);
	nread = read(client_fd, my_msg, BUF_SIZE-2);
	my_msg[nread] = '\0';
	printf("[CLIENTE] %s \n", my_msg);
	factor = 0;
	if (strcmp("DADOS\n", my_msg) == 0) {
		factor = 1;
		special_factor = 1;
	}
	else {
		if (strcmp("SOMA\n", my_msg) == 0) {
			factor = 2;
			if (special_factor == 0) {
				bzero(my_msg, BUF_SIZE -2);
				n_char = sprintf(my_msg, "[SERVIDOR] Erro: Numeros ainda nao recebidos!\n");
				write(client_fd, my_msg, 1 + n_char);
				factor = 0;
			}
		}
		else {
			if (strcmp("MEDIA\n", my_msg) == 0) {
				factor = 3;
				if (special_factor == 0) {
					bzero(my_msg, BUF_SIZE -2);
					n_char = sprintf(my_msg, "[SERVIDOR] Erro: Numeros ainda nao recebidos!\n");
					write(client_fd, my_msg, 1 + n_char);
					factor = 0;
				}
			}
			else {
				bzero(my_msg, BUF_SIZE -2);
				n_char = sprintf(my_msg, "[SERVIDOR] Erro: Comando nao existe!\n");
				write(client_fd, my_msg, 1 + n_char);
				factor = 0;
			}
				
		}
	}
	
	if (factor == 1) {
		bzero(my_msg, BUF_SIZE -2);
		n_char = sprintf(my_msg, "[SERVIDOR] Escreva os 10 numeros separados por espacos simples:\n");
		write(client_fd, my_msg, 1 + n_char);
		bzero(my_msg, BUF_SIZE -2);
		nread = read(client_fd, my_msg, BUF_SIZE-1);
		my_msg[nread] = '\0';
		printf("[CLIENTE] %s \n", my_msg);
		omg = 0;
		meu_erro = 0;
		for (i = 0, counter = 0; my_msg[i] != '\0'; i++) {
			n_char = 0;
			
			while (isdigit(my_msg[i])) {
				
				temp[n_char] = my_msg[i];
				n_char++;
				i++;
				omg = 1;
			}
			if (counter >= 10 && omg == 1) {
				meu_erro = -1;
				break;
			}
			if (omg == 1) {
				array[counter] = atoi(temp);
				//printf(">>> %d\n", array[counter]);
				counter++;
				omg = 0;
			}
			bzero(temp, 11);
		}
		
		if (meu_erro == -1 || counter != 10) {
			bzero(my_msg, BUF_SIZE -2);
			n_char = sprintf(my_msg, "[SERVIDOR] Erro: Quantidade de Numeros Invalidos!\n");
			write(client_fd, my_msg, 1 + n_char);
			factor = 0;
			special_factor = 0;
			meu_erro = 0;
		}
		else {
			bzero(my_msg, BUF_SIZE -2);
			n_char = sprintf(my_msg, "[SERVIDOR] Numeros Recebidos e Validos!\n");
			write(client_fd, my_msg, 1 + n_char);
		}
	
	}
	if (factor == 2) {
		acumula = 0;
		for (i=0; i <10 ; i++) {
			acumula = acumula + array[i];
		}
		bzero(my_msg, BUF_SIZE -2);
		n_char = sprintf(my_msg, "[SERVIDOR] Soma = %d\n", acumula);
		write(client_fd, my_msg, 1 + n_char);
	}
	if (factor == 3) {
		acumula = 0;
		for (i=0; i <10 ; i++) {
			acumula = acumula + array[i];
		}
		media = acumula / 10;
		bzero(my_msg, BUF_SIZE -2);
		n_char = sprintf(my_msg, "[SERVIDOR] Media = %f\n", media);
		write(client_fd, my_msg, 1 + n_char);
	}
	}
	
	
	
	
	
	
	fflush(stdout);
	close(client_fd);
}

void erro(char *msg) {
	printf("Erro: %s\n", msg);
	exit(-1);
}
