#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: daqctl.cxx,v 1.1.1.1 2003-07-03 18:34:51 jschamba Exp $";
#endif /* lint */

/*****************************************************************************
 Excerpt from "Linux Programmer's Guide - Chapter 6"
 (C)opyright 1994-1995, Scott Burkett
 ***************************************************************************** 
 MODULE: fifoclient.c
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#define FIFO_FILE       "/tmp/daqfifo"

int main(int argc, char *argv[])
{
  FILE *fp;
  
  if ( argc != 2 ) {
    printf("USAGE: daqctl [string]\n");
    exit(1);
  }
  
  if((fp = fopen(FIFO_FILE, "w")) == NULL) {
    perror("fopen");
    exit(1);
  }
  
  fputs(argv[1], fp);
  
  fclose(fp);
  return(0);
}
