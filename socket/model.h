#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <float.h>
#include <stdlib.h>

// Estruturas que podem ser passadas por mensagem

typedef struct
{
    float min;
    float max;
} Result;

typedef struct
{
    float *data;
    int length;
    int readed;
    Result result;
} Params;


// Valores DEFAULT da estrutura
static const Params DEFAULT_PARAMS = {NULL, -1, 0, {FLT_MAX, FLT_MIN}};

#define pequals(a, b) (a.length==b.length && a.readed==b.readed)

#endif