#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "error.h"

// Garante que a condicao success seja verdadeira
// caso contrario o programa eh abortado
void ensure_success(int success, char *errorMsg, ...) {
    va_list args;

    if (!success) {
        va_start(args, errorMsg);
        vfprintf(stderr, errorMsg, args);
        exit(1);
    }
    va_end(args);
}
