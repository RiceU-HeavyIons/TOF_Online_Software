/* $Id$ */
// JS: added extra data for the DDL banks
//
#ifndef _TOF_READER_H_
#define _TOF_READER_H_

#include <sys/types.h>


struct tof {
  int mode ;
  int channels ;
  int max_channels ;
  int ddl_words[4];

  u_short adc[180] ;	// was 48 in FY02
  u_short tdc[184];	// was 48 in FY02
  float   a2d[32];
  u_int   sca[12];
  u_int * ddl_word_p[4];
} ;

extern struct tof tof ;

extern int tofReader(char *mem) ;

#endif
