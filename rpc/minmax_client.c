#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <float.h>
#include <math.h>

#include "minmax.h"
#include "error.h"

// Quantidade de dados
#ifndef MAXDATA
#define MAXDATA 500000
#endif

// Estrutura que sera passada para cada thread
// com os dados que serao passados para o servidor
typedef struct
{
	char *host;
	params op;
}Toperandos;

// Os dados que serao processados
static float data[MAXDATA];

// Resultado do processamento dos dados
// Como eh regiao critica, eh necessario de um semaforo
result r = {FLT_MAX, FLT_MIN};
sem_t mutex;


void init_data();
void sqrt_data();
CLIENT* create_client(char *host);
pthread_t* start_workers(int nWorkers, char *hosts[]);
void* worker_runner(void *arg);
void wait_workers(int nWorkers, pthread_t *tids);

/*
	A funcao main gerencia toda a inicializacao, conexao com os servidores e impressao do resultado
*/
int main(int argc, char *argv[])
{
	pthread_t *tids;
	int nWorkers = argc - 1;

	// Validacao dos argumentos
	ensure_success(argc >= 2, "Uso: %s IPSERVER...\n", argv[0]);

	// Inicializacao dos dados
	init_data();
	sqrt_data();

	// Divisao dos dados e inicializacao dos workers
	tids = start_workers(nWorkers, argv + 1);

	wait_workers(nWorkers, tids);

	// Impressao do resultado
	printf("\n===============RESULTADO===================\n");
    printf("menor=%f\n", r.min);
    printf("maior=%f\n", r.max);
    printf("===========================================\n");

	return 0;
}

void init_data() {
    float random, number;

    srand((unsigned int)time(NULL));

    printf("\nINFO: Inicializando %d dados...\n", MAXDATA);

    for (int i = 0; i < MAXDATA; i++)
    {
        random = (float)rand() / (float)(RAND_MAX / MAXDATA);
        number = (1 - random / 2);
        data[i] = number * number;
    }
}

void sqrt_data() {
    printf("INFO: Realizando sqrt dos dados...\n");
    for (int i = 0; i < MAXDATA; i++){
        data[i] = sqrtf(data[i]);
		data[i] = (float)i;
    }
}

// Cria um cliente tcp com o servidor rpc
CLIENT* create_client(char *host) {
	CLIENT *clnt = clnt_create(host, PROG, VERSAO, "tcp");
	ensure_success(clnt != NULL, "ERROR: falha de conexao com o host %s\n", host);
	return clnt;
}

// Realiza a divisao dos dados e inicializacao das threads dos workers
pthread_t* start_workers(int nWorkers, char *hosts[]) {
	pthread_t *tids = malloc(nWorkers * sizeof(pthread_t));
	Toperandos *tops = malloc(nWorkers * sizeof(Toperandos));
	int offset;
	int section = (MAXDATA)/nWorkers;		// Quantidade de dados que serao passados a cada worker

	sem_init(&mutex, 0, 1);					// Inicializacao do semaoro

	printf("INFO: enviando dados para os n=%d workers\n", nWorkers);
	for (int i = 0; i < nWorkers; i++) {
		// Endereco do servidor
		tops[i].host = hosts[i];

		// Divisao dos dados
		offset = i*section;
		tops[i].op.data.data_val = data + offset;
		tops[i].op.data.data_len = (i+1 == nWorkers) ? MAXDATA - offset : section;

		// Inicializacao da thread do worker
		pthread_create(&tids[i], NULL, &worker_runner, (void *)&tops[i]);
	}

	return tids;
}

// O worker cria conexao com o servidor rpc, envia os dados e espera o resultado
void* worker_runner(void *arg) {
	Toperandos *top = (Toperandos*) arg;
	result *tmp;
	CLIENT *clnt = create_client(top->host);
	
	tmp = minmax_100(&top->op, clnt);
	ensure_success(tmp != NULL, "ERROR: falha na chamada remota minmax\n");

	sem_wait(&mutex);

	if (tmp->min < r.min) {
		r.min = tmp->min;
	}

	if (tmp->max > r.max) {
		r.max = tmp->max;
	}

	sem_post(&mutex);
}

// Aguarda todos as threads finalizarem
void wait_workers(int nWorkers, pthread_t *tids) {
	printf("INFO: aguardando respostas...\n");
	for (int i = 0; i < nWorkers; i++) {
		pthread_join(tids[i], NULL);
	}
}
