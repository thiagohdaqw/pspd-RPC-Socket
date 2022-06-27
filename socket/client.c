#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>

#include "message.h"
#include "error.h"

// Constante que define a quantidade de numeros que serao gerados
// Esse programa aguenta no máximo o valor de 2**31-1 = INT_MAX - 1
#ifndef MAX
#define MAX 500000
#endif

// Array que guarda os dados gerados
static float data[MAX];
// variavel que guarda o resultado, min e max, do processamento
// eh uma regiao critica, por isso a necessidade de um semaforo
Result result = {FLT_MAX, FLT_MIN};
sem_t mutex;

// estrutura com os dados necessarios para que as threads consigam
// realizar a conexao e processamento com o servidor
typedef struct {
    int id;                     // id da thread para fins de log
    char *host;                 // endereco do servidor
    int   port;                 // porta do servidor
    Params p;                   // parametros com os dados (tamanho e so floats)
} WorkerParams;                 //     que serao enviados para o servidor


pthread_t* start_workers(int nWorkers, char *addr[]);
void* worker_runner(void *args);
int start_connection(char *host, int port);
void wait_workers(int nWorkers, pthread_t *tids);
void set_result(Result r);
void init_data();
void sqrt_data();


/*
    A main é onde tudo acontece.
    Recebe os enderecos e portas do servidor como parametro
    inicializa os dados
    cria as threads que se conectam com o servidor
    e imprime o resultado
*/
int main(int argc, char *argv[]) {
    int nWorkers;
    pthread_t *tids;

    // Validacao dos parametros
    ensure_success(argc >= 4, "Uso: %s N_WORKERS (IP PORT)... \n", argv[0]);
    ensure_success(argc >= atoi(argv[1])*2+2, "ERROR: Informe a quantidade de workers e seus enderecos e portas\n", argv[0]);

    // Inicializacao dos dados
    init_data();
    sqrt_data();

    // Conexao com os servidores e processamento dos dados
    nWorkers = atoi(argv[1]);

    tids = start_workers(nWorkers, argv + 2);
    
    wait_workers(nWorkers, tids);

    // Impressao do resultado
    printf("\n===============RESULTADO===================\n");
    printf("menor=%f\n", result.min);
    printf("maior=%f\n", result.max);
    printf("===========================================\n");

    return 0;
}

/*
    Aqui acontece a divisao dos dados para que cada worker processe uma parte do todo
    e inicializacao das threads que se conectam com os servidores
*/
pthread_t* start_workers(int nWorkers, char *addr[]) {
    pthread_t *tids  = malloc(nWorkers * sizeof(pthread_t));
    WorkerParams *ws = malloc(nWorkers * sizeof(WorkerParams));
    int offset;
    int section = MAX/nWorkers;                                 // Quantidade de dados por worker

    sem_init(&mutex, 0, 1);
    for (int i = 0; i < nWorkers; i++) {
        // Informacoes do servidor
        ws[i].id = i+1;
        ws[i].host = addr[i*2];
        ws[i].port = atoi(addr[i*2 + 1]);

        // Divisao dos dados
        offset         = i*section;
        ws[i].p.data   = data + offset;
        ws[i].p.length = (i+1 == nWorkers) ? MAX - offset : section;

        // Inicializacao das threads
        pthread_create(&tids[i], NULL, &worker_runner, (void *)&ws[i]);
    }
    return tids;   
}


/*
    Thread que se conecta com o servidor e aguarda a resposta
*/
void* worker_runner(void *args) {
    WorkerParams *ws = (WorkerParams *)args;
    Result r;
    int sd = start_connection(ws->host, ws->port);

    printf("INFO[%d]: Enviando %d dados...\n", ws->id, ws->p.length);
    send_params(ws->p, sd);

    printf("INFO[%d]: Aguardando resultado...\n", ws->id);
    r = receive_result(sd);

    set_result(r);
    
    close(sd);
}

/*
    Atribui o resultado do worker na variavel global de resultado
    Como ha varias threads e essa eh uma regiao critica, eh utilizado semaforos
*/
void set_result(Result r) { 
    sem_wait(&mutex);

    if(r.min < result.min) {
        result.min = r.min;
    }
    if(r.max > result.max) {
        result.max = r.max;
    }

    sem_post(&mutex);
}


// Inicia a conexao com o servidor por TCP
int start_connection(char *host, int port) {
    struct sockaddr_in serverAddr;
    int n, sd;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    ensure_success(sd > 0, "ERROR: nao pode abrir o socket \n");

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(host);
    serverAddr.sin_port = htons(port);

    n = connect(sd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    ensure_success(n == 0, "ERROR: falha ao tentar se conecar com servidor %s:%d\n", host, port);

    return sd;
}


// Aguarda as threads finalizarem para continuar o programa
void wait_workers(int nWorkers, pthread_t *tids) {
    for (int i = 0; i < nWorkers; i++) {
        pthread_join(tids[i], NULL);
    }
}

// Inicializacao dos dados
void init_data()
{
    float random, number;

    srand((unsigned int)time(NULL));

    printf("\nINFO: Inicializando %d dados...\n", MAX);

    for (int i = 0; i < MAX; i++)
    {
        random = (float)rand() / (float)(RAND_MAX / MAX);
        number = (1 - random / 2);
        data[i] = number * number;
    }
}

void sqrt_data()
{
    printf("INFO: Realizando sqrt dos dados...\n");
    for (int i = 0; i < MAX; i++){
        data[i] = sqrtf(data[i]);
    }
}
