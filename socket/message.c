#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "message.h"
#include "error.h"

// Converte os bytes para float
float deserialize(char *buffer) {
    float *target = (float *)buffer;
    return *target; 
}

// Envia os parametros pelo socket
// Formato da mensagem: LENGTH(float-bytes) + data(LENGTH-bytes)
void send_params(Params p, int sd) {
    int sended;

    sended =  send(sd, &p.length, sizeof(p.length), 0);             // Envia primeiro o tamanho do array de dados
    sended += send(sd, p.data, p.length*sizeof(float), 0);          // Envia os dados

    ensure_success(sended > 0, "ERROR: falha ao enviar os dados\n");
}

// Envia o resultado pelo socket
// Formato da mensagem: MIN(float-bytes) + MAX(float-bytes)
void send_result(Result r, int sd) {
    int sended;

    sended =  send(sd, &r.min, sizeof(r.min), 0);               // Envia o minimo
    sended += send(sd, &r.max, sizeof(r.max), 0);               // Envia o maximo

    ensure_success(sended > 0, "ERROR: falha ao enviar os dados\n");
}

// Recebe o resultado pelo socketz
Result receive_result(int sd) {
    char buffer[RESULT_LENGTH] = {[0 ... RESULT_LENGTH-1] = 0};
    int readed = 0;

    readed = recv(sd, buffer, RESULT_LENGTH, MSG_WAITALL);
    ensure_success(readed == RESULT_LENGTH, "ERROR: Erro ao ler o resultado! n=%d\n", readed);
    
    return read_result(buffer);
}

// Interpreta o resultado pro meio dos bytes recebidos
// retorna a quantidade de bytes lidos do buffer
int init_params(Params *p, char *buffer) {
    p->length = *((int *)buffer);
    p->readed = 0;
    return sizeof(p->length);
}

// Le os dados passados pelo socket e pra cada float lido o callback eh chamado
//     evitando que todos os dados sejam armazenados na memoria
// Retorna verdadeiro se ja leu todos os dados
int read_data(Params *p, char *buffer, int *bufferCursor, int bufferMax, void (*callback)(Result*, float)) {
    float num;
    int sizeofFloat = sizeof(float);

    while (p->readed < p->length && *bufferCursor + MSG_MIN <= bufferMax) {
        num = deserialize(buffer + *bufferCursor);

        p->readed++;
        *bufferCursor += sizeofFloat;

        (*callback)(&p->result, num);
    }

    return p->readed >= p->length;
}

// Interpreta os bytes do resultado recebido pelo socket 
Result read_result(char *buffer) {
    Result r;

    r.min = deserialize(buffer);
    r.max = deserialize(buffer + sizeof(float));

    return r;
}
