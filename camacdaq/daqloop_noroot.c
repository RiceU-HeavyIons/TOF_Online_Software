#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id$";
#endif /* lint */

/* File name     : daqloop.c
 * Creation date : 4/15/02
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

/* System headers */
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>

/* Local headers */
#include "camlib.h"

/* Macros */
#define  ADC_2249 2
#define  MASK_2249 (1<<(ADC_2249-1))
#define  MEMBUF 4
#define  MAXLOOP 1000
#define  MAXLEN  1000
/* File scope variable */
unsigned short writebuffer[MAXLEN], readbuffer[MAXLEN];

/* External variables */

/* External functions */

/* Structure and unions */
union swaps {
    short shorts[2];
    long longs;
} swaps;

/* Signal catching functions */

/* Functions */
error_exit(status)
int status; {
    perror(strerror(status));
    exit(status);
}

/* Main */
main() {
  int status, i, ii, q, x;
  int length, error, return_length;
  int loop;
  short j;
  unsigned long seed=123456789;
  
  printf("daqloop : test starts\n");
  /* Initialization part */
  if( status = CAM_Open( ) ) 	error_exit(status);
  if( status = CSETBR(0) ) 	error_exit(status);
  if( status = CSETCR(0) ) 	error_exit(status);
  if( status = CGENZ() ) 	error_exit(status);
  if( status = CGENC() ) 	error_exit(status);
  if( status = CSETI() ) 	error_exit(status);
  
  /* Initialize CAMAC modules */
  /* 2249: Clear module and LAM */
  if( status = CAMAC(NAF(ADC_2249, 0, 9), &i, &q, &x) ) error_exit(status);
  printf("ADC F9: q=%d, x=%d\n", q, x);  
  /* 2249: Enable LAM */
  if( status = CAMAC(NAF(ADC_2249, 0, 26), &i, &q, &x) ) error_exit(status);
  printf("ADC F26: q=%d, x=%d\n", q, x);  
  

  /* Remove Inhibit */
  if( status = CREMI() ) 	error_exit(status);

  if( status = CENLAM (MASK_2249) ) error_exit(status);
  
  printf("enabled lam, will now wait for LAM...\n");
  status = CWTLAM(1000);

  printf("CWTLAM returned with status = %d\n", status);

  CCLOSE();
}

