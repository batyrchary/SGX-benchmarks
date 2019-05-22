#ifndef _ENCLAVE_H_
#define _ENCLAVE_H_

#include <stdlib.h>
#include <assert.h>

#if defined(__cplusplus)
extern "C" {
#endif

void printf(const char *fmt, ...);

void ecall_my_dtree( int *data, size_t len);

void selection_sort( int *data, size_t len);


#if defined(__cplusplus)
}
#endif

#endif /* !_ENCLAVE_H_ */
