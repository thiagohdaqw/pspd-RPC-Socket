#include "minmax.h"
#include <float.h>
#include <unistd.h>

result *
minmax_100_svc(params *argp, struct svc_req *rqstp)
{
	static result  result;

	// Inicializacao do resultado
	result.min = FLT_MAX;
	result.max = FLT_MIN;

	printf("INFO[%d]: processando %d dados...\n", getpid(), argp->data.data_len);
	
	for (int i = 0; i < argp->data.data_len; i++) {
		if (argp->data.data_val[i] < result.min) {
			result.min = argp->data.data_val[i];
		}
		if (argp->data.data_val[i] > result.max) {
			result.max = argp->data.data_val[i];
		}
	}
	
	printf("INFO[%d]: enviando Resultado(min=%f, max=%f)\n", getpid(), result.min, result.max);

	return &result;
}
