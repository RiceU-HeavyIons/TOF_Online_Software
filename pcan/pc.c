/* File name     : pc.c
 * Creation date : 9/10/03
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id$";
#endif /* lint */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define FIFO_FILE       "/tmp/pcanfifo"

int main(int argc, char *argv[])
{
  FILE *fp;
  char txt[256];

  if ( argc != 2 ) {
    printf("USAGE: %s [string]\n", argv[0]);
    exit(1);
  }
  
  sprintf(txt, "%s-%d", FIFO_FILE, getuid()); // create FIFO filename
  if((fp = fopen(FIFO_FILE, "w")) == NULL) {
    perror("fopen");
    exit(1);
  }
  
  fputs(argv[1], fp);
  
  fclose(fp);
  return(0);
}
