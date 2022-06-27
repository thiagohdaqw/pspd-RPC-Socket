#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "error.h"
#include "message.h"

// pid do processo.
int pid;

int init_server_socket(int argc, char* argv[]);
void handle_client(int cd, struct sockaddr_in endCli);
void close_connection(int cd, struct sockaddr_in endCli);
void minmax(Result *r, float number);

/*
    Funcao main que gerencia o servidor
    Cria um processo filho para atender cada nova conexao
*/
int main(int argc, char* argv[])
{
    int sd, clientSd;
    struct sockaddr_in endCli;
    int cliLength = sizeof(endCli);

    ensure_success(argc >= 3, "Uso: %s IP PORTA\n", argv[0]);

    sd = init_server_socket(argc, argv);

    pid = getpid();
    printf("\nINFO[%d]: esperando por dados no IP: %s, porta TCP numero: %s\n", pid, argv[1], argv[2]);
    while (1)
    {
        clientSd = accept(sd, (struct sockaddr *)&endCli, &cliLength);
        ensure_success(clientSd >= 0, "ERROR[%d]: falha na conexao\n", pid);
        
        pid = fork();
        ensure_success(pid >= 0, "ERROR[%d]: falha na criacao do processo filho\n", pid);

        if (pid == 0) {
            close(sd);
            pid = getpid();
            printf("\nINFO[%d]: [%s:%u] nova conexao\n", pid, inet_ntoa(endCli.sin_addr), ntohs(endCli.sin_port));
            handle_client(clientSd, endCli);
            return 0;
        }
    }
    close(sd);
    return 0;
}

// Inicia o socket tcp do servidor
int init_server_socket(int argc, char* argv[]) {
    struct sockaddr_in endServ;
    int sd, rc;

    memset((char*)&endServ, 0x0, sizeof(endServ));

    // Informacoes do servidor
    endServ.sin_family = AF_INET;
    endServ.sin_addr.s_addr = inet_addr(argv[1]);
    endServ.sin_port = htons(atoi(argv[2]));

    // Inicializacao do socket
    sd = socket(AF_INET, SOCK_STREAM, 0);
    ensure_success(sd >= 0, "ERROR[%s]: socket nao pode ser aberto\n", argv[0]);

    rc = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    ensure_success(rc >= 0, "ERROR[%s]: nao foi possivel reutilizar o endereco no socket\n", argv[0]);

    rc = bind(sd, (struct sockaddr *)&endServ, sizeof(endServ));
    ensure_success(rc >= 0, "ERROR[%s]: nao foi possivel realizar o bind na porta %s\n", argv[0], argv[2]);

    rc = listen(sd, 5);
    ensure_success(rc >= 0, "ERROR[%s]: nao foi possivel ouvir a porta %s\n", argv[0], argv[2]);

    return sd;
}

/*
    Funcao que gerencia cada nova conexao
    Le bytes enviados pelo client, processa, interpreta e retorna o resultado com o minimo e maximo
*/
void handle_client(int sd, struct sockaddr_in endCli) {
    Params p = DEFAULT_PARAMS;
    int MAX_MSG = 2048;                 // Tamanho maximo da mensagem lida
    int bufferMax = MAX_MSG+5;          // Tamanho maximo considerando o resto
    char buffer[bufferMax];
    int resto = 0;                      // O resto referencia a quantidade de bytes nao interpretados, pois o tamanho eh menor que o do float         
    int cursor = 0;                     // Cursor que indexa o buffer
    int buffLength = 0;

    while (1) {
        for (int i = 0; i < resto; i++) {
            buffer[i] = buffer[buffLength+i];                       // Adiciona o resto no inicio do buffer
        }
        
        memset(buffer + resto, 0x0, bufferMax);                     // Limpa o buffer

        buffLength = recv(sd, buffer + resto, MAX_MSG, 0) + resto;  // Le os dados

        resto = 0;                                                  
        cursor = 0;

        if (buffLength == 0) break;

        if (buffLength % MSG_MIN != 0) {                            // Como a quantiade de bytes lidos eh variavel
            resto = buffLength % MSG_MIN;                           // Os bytes que nao atendem o minomo sao tratados como resto
            buffLength -= resto;                                    // E serao processados na proxima iteracao
        }

        if (pequals(p, DEFAULT_PARAMS) && buffLength >= MSG_MIN) {  // Se for o inicio da leitura, inicializa os parametros
            cursor = init_params(&p, buffer);
            printf("\nINFO[%d]: %d dados a caminho...\n", pid, p.length);
        }

        if (read_data(&p, buffer, &cursor, buffLength, minmax)) {   // Interpreta os dados e executa o minmax para cada dado
            printf("\nINFO[%d]: leitura concluida.\n", pid);
            printf("INFO[%d]: enviando Resultado(min=%f, max=%f)\n", pid, p.result.min, p.result.max);
            send_result(p.result, sd);                              // Envia o resultado para o cliente
            break;
        }
    }
    close_connection(sd, endCli);
    return;
}

// Funcao que verifica que o valor lido eh menor e maior que o do resultado
void minmax(Result *r, float number) {
    if (number < r->min) {
        r->min = number;
    }
    if (number > r->max) {
        r->max = number;
    }
}


void close_connection(int cd, struct sockaddr_in endCli) {
    printf("\nINFO[%d]: [%s:%u] Encerrando conexao\n", pid, inet_ntoa(endCli.sin_addr), ntohs(endCli.sin_port));
    close(cd);
}