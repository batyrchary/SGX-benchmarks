#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */


#include "Enclave.h"
#include "Enclave_t.h"  /* print_string */

/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
void printf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
}



void ecall_my_dtree( int *data, size_t len)
{
	//printf("%d",len);	
}



void selection_sort( int *data, size_t len)
{
	
	int *ar=data;
	int length=len/4;

	for (int i = 0; i < length-1; i++)
   	{
      		int min = i;
      		for (int j = i+1; j < length; j++)
            		if (ar[j] < ar[min]) min = j;
      		int temp = ar[i];
     		ar[i] = ar[min];
      		ar[min] = temp;
	}		
}

