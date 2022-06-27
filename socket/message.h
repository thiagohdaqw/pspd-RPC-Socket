#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include "model.h"

#define MSG_MIN sizeof(float)                   // Tamanho minimo de uma mensagem em bytes.
#define RESULT_LENGTH 2*sizeof(float)           // Tamanho do resultado (min e max)

// Envia os parametros pelo socket
// Formato da mensagem: LENGTH(float-bytes) + data(LENGTH-bytes)
void send_params(Params p, int sd);

// Envia o resultado pelo socket
// Formato da mensagem: MIN(float-bytes) + MAX(float-bytes)
void send_result(Result r, int sd);
    
// Inicia os parametros lendo o buffer do socket
// Retorna a quantidade de bytes lidos do buffer
int init_params(Params *p, char *buffer);

// Le os dados passados pelo socket e pra cada float lido o callback eh chamado
// Retorna verdadeiro se ja leu todos os dados
int read_data(Params *p, char *buffer, int *bufferCursor, int bufferMax, void (*callback)(Result*, float));

// Interpreta os bytes do resultado recebido pelo socket 
Result read_result(char *buffer);

// recebe o resultado pelo socket
Result receive_result(int sd);

#endif