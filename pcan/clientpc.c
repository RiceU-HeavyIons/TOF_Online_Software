/* File name     : clientpc.c
 * Creation date : 11/18/05
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
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <libpcan.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_FILE       "/tmp/pcanfifo"
#define FIFO_RESPONSE "/tmp/pcanfifoRsp"

int main(int argc, char *argv[])
{
  FILE *fp;
  char txt[256];
  int fifofd;
  int numRead, i;
  TPCANMsg m;

  if ( argc != 2 ) {
    printf("USAGE: %s [string]\n", argv[0]);
    exit(1);
  }
  
  umask(0);
  printf("make FIFO\n");
  mkfifo(FIFO_RESPONSE, 0666);


  printf("open write FIFO\n");
  sprintf(txt, "%s-%d", FIFO_FILE, getuid()); // create FIFO filename
  if((fp = fopen(txt, "w")) == NULL) {
    perror("fopen");
    exit(1);
  }
  

  printf("sending w command\n");
  fputs("w", fp); fflush(fp);

  /* this blocks until the other side is open for Write */
  printf("open FIFO\n");
  fifofd = open(FIFO_RESPONSE, O_RDONLY);

  printf("sending command\n");
  fputs(argv[1], fp); fflush(fp);
  
  
  printf("reading response\n");
  numRead = read(fifofd, &m, sizeof(m));


  printf("number of bytes read %d\n", numRead);
  if (numRead > 0) {
    printf("testit: message received : %c %c 0x%08x %1d  ", 
	   (m.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
	   (m.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
	   m.ID, 
	   m.LEN); 
    
    for (i = 0; i < m.LEN; i++)
      printf("0x%02x ", m.DATA[i]);
    printf("\n");
  }

  printf("sending r command\n");
  fputs("r", fp); fflush(fp);
  fclose(fp);


  unlink(FIFO_RESPONSE);
  return(0);
}
