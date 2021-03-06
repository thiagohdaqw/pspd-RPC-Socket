/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "minmax.h"

bool_t
xdr_params (XDR *xdrs, params *objp)
{
	register int32_t *buf;

	 if (!xdr_array (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, ~0,
		sizeof (float), (xdrproc_t) xdr_float))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_result (XDR *xdrs, result *objp)
{
	register int32_t *buf;

	 if (!xdr_float (xdrs, &objp->min))
		 return FALSE;
	 if (!xdr_float (xdrs, &objp->max))
		 return FALSE;
	return TRUE;
}
