/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _MINMAX_H_RPCGEN
#define _MINMAX_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct params {
	struct {
		u_int data_len;
		float *data_val;
	} data;
};
typedef struct params params;

struct result {
	float min;
	float max;
};
typedef struct result result;

#define PROG 55555555
#define VERSAO 100

#if defined(__STDC__) || defined(__cplusplus)
#define MINMAX 1
extern  result * minmax_100(params *, CLIENT *);
extern  result * minmax_100_svc(params *, struct svc_req *);
extern int prog_100_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define MINMAX 1
extern  result * minmax_100();
extern  result * minmax_100_svc();
extern int prog_100_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_params (XDR *, params*);
extern  bool_t xdr_result (XDR *, result*);

#else /* K&R C */
extern bool_t xdr_params ();
extern bool_t xdr_result ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_MINMAX_H_RPCGEN */