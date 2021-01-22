#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFLEN 1024	// Tamanho do buffer
#define PORT 9876	// Porto para recepção das mensagens

void erro(char *s)
	{
	perror(s);
	exit(1);
	}

int main(void)
{
struct sockaddr_in si_minha, si_outra;

	int s,recv_len, count, len;
	socklen_t slen = sizeof(si_outra);
	char buf[BUFLEN];
	char my_str[BUFLEN];

	// Cria um socket para recepção de pacotes UDP
	if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		erro("Erro na criação do socket");
	}

	si_minha.sin_family = AF_INET;
	si_minha.sin_port = htons(PORT);
	si_minha.sin_addr.s_addr = htonl(INADDR_ANY);

	// Associa o socket à informação de endereço
	if(bind(s,(struct sockaddr*)&si_minha, sizeof(si_minha)) == -1)
	{
		erro("Erro no bind");
	}

	// Espera recepção de mensagem (a chamada é bloqueante)
	if((recv_len = recvfrom(s, buf, BUFLEN, MSG_WAITALL, (struct sockaddr *) &si_outra, (socklen_t *)&slen)) == -1)
	{
	erro("Erro no recvfrom");
	}
	// Para ignorar o restante conteúdo (anterior do buffer)
	buf[recv_len]='\0';

	printf("[CLIENTE] %s\n", buf);
	count = strlen(buf);
	len = sprintf (my_str, "N. de Caracteres = %d", count);
	my_str[len] = '\0';
	
	sendto(s, my_str, len+1, MSG_CONFIRM, (const struct sockaddr *) &si_outra, (socklen_t) sizeof(si_outra)); 


	// Fecha socket e termina programa
	close(s);
	return 0;
}
