#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

// Garante que a condicao success seja verdadeira
// caso contrario o programa eh abortado
void ensure_success(int success, char *errorMsg, ...);

#endif