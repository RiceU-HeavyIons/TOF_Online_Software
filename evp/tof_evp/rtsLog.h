#ifndef _RTS_LOG_H_
#define _RTS_LOG_H_

/* 
	Version 2.1	Added __ROOT__ to stderr. 
	Version 2.0	Revamped to use funky variadic GNU cpp stuff...
	Version 1.2	Added RTS_PROJECT so that the defaults work
			even with PP2PP
	Version 1.11	Replaced "extern __inline__" with just "inline"
			so that Sun's CC can use it...
	Version 1.1	Should be useful by	vxWorks: I960, PPC
						unix


*/

#ifdef __cplusplus
extern "C" {
#endif





/* this is only used in the UNIX version */


#define RTS_LOG_PORT_RTS	8000
#define RTS_LOG_PORT_TEST	8001
#define RTS_LOG_PORT_DAQ	8002
#define RTS_LOG_PORT_TRG	8003
#define RTS_LOG_PORT_EVP	8004
#define RTS_LOG_PORT_READER	8005
#define RTS_LOG_PORT_DUMMY	8006
#define RTS_LOG_PORT_DB		8007
#define RTS_LOG_PORT_SL3	8008
#define RTS_LOG_PORT_NONE	8009
#define RTS_LOG_PORT_DET	8010

#define RTS_LOG_PORT	RTS_LOG_PORT_RTS	/* default, rts.log, port */

/* the hostname _must_ be in the number notation! */
#define RTS_LOG_HOST	"130.199.60.86"	/* daqman.starp */

/* Bit pattern: log over network and/or stderr. Default is both. */
#define RTS_LOG_NET	1
#define RTS_LOG_STDERR	2


/* strings used */
#define CRIT	"CRITICAL"	/* unmasked (5) */
#define OPER	"OPERATOR"	/* 4 */
#define ERR	"ERROR"		/* 3 */
#define WARN	"WARNING"	/* 2 */
#define NOTE	"NOTICE"	/* 1 */
#define DBG	"DEBUG"		/* 0 */

#define INFO	"INFO"		/* unmasked */

#define CAUTION	"CAUTION"	/* unmasked - for the operator */

#define TERR	"Tonko"		/* this will go away! */
#define SHIFT	"SHIFTLOG"	/* this will go away! */


#ifdef __ROOT__			/* Special (mis)handling for STAR Offline Code */

/* To make this work at all, one needs to fix the severity level to ERR */
	#define LOG(SEV,STRING,ARGS...) \
        do { \
                const char *const yada = SEV ; \
                if((*yada == 'D')) ; \
                else if((*yada == 'N')) ; \
                else if((*yada == 'W')) ; \
                else { \
                        fprintf(stderr,"RTSlog="SEV": "__FILE__" [line %d]: "STRING"\n" , __LINE__ , ##ARGS) ;\
		} \
	} while(0) \


#else	/* __ROOT__ */


#ifdef __GNUC__
	#define INLINE_HACK extern __inline__
#else
	#define INLINE_HACK inline
#endif



/* exists in all flavors */
extern volatile int tonkoLogLevel ;	/* the unfortunte name is due to historic reasons... */

extern int rtsLogAddDest(char *server, int port) ;


INLINE_HACK void rtsLogLevelInt(int level)
{
  tonkoLogLevel = level;
  return;
}

/* let's have a function too... */
INLINE_HACK void rtsLogLevel(char *level) 
{
	switch((int) *level) {
	case 'D' :
		tonkoLogLevel = 0 ;
		break ;
	case 'N' :
		tonkoLogLevel = 1 ;
		break ;
	case 'W' :
		tonkoLogLevel = 2 ;
		break ;
	case 'E' :
		tonkoLogLevel = 3 ;
		break ;
	case 'O' :
		tonkoLogLevel = 4 ;
		break ;
	case 'C' :
	default :
		tonkoLogLevel = 5 ;
		break ;
	}

	return ;
}





#ifdef __vxworks	
/* Following is vxWorks specific */
	#include <vxWorks.h>
	#include <logLib.h>

	/* Only in MVME vxworks kernels! */
	#ifdef _ARCH_PPC	
		extern int sbLoggerStart(void) ;
		extern int sbLOG(char *str, unsigned int a1, unsigned int a2, 
				 unsigned int a3, unsigned int a4, unsigned int a5, unsigned int a6) ;
		extern int sbLoggerRemoveDesc(int desc) ;
		extern int sbLoggerAddDesc(int desc) ;
	#else
		#define sbLOG(args...) 
	#endif


	#define LOG(SEV,STRING,A1,A2,A3,A4,A5) \
        do { \
                const char *const yada = SEV ; \
                if((tonkoLogLevel>0) && (*yada == 'D')) ; \
                else if((tonkoLogLevel>1) && (*yada == 'N')) ; \
                else if((tonkoLogLevel>2) && (*yada == 'W')) ; \
                else if((tonkoLogLevel>3) && (*yada == 'E')) ; \
                else if((tonkoLogLevel>4) && (*yada == 'O')) ; \
                else { \
			logMsg(""SEV": "__FILE__" [line %d]: "STRING"\n",__LINE__,(unsigned int)A1,(unsigned int)A2,(unsigned int)A3,(unsigned int)A4,(unsigned int)A5) ;\
			sbLOG(""SEV": "__FILE__" [line %d]: "STRING"\n",__LINE__,(unsigned int)A1,(unsigned int)A2,(unsigned int)A3,(unsigned int)A4,(unsigned int)A5) ;\
		} \
	} while(0) \


#else /* unix */

	extern int rtsLogUnix_v(const char *str, ...) ;

	extern int rtsLogOutput(int flag) ;

	extern void rtsLogAddCmd(const char *cmd) ;

	#define LOG(SEV,STRING,ARGS...) \
        do { \
                const char *const yada = SEV ; \
                if((tonkoLogLevel>0) && (*yada == 'D')) ; \
                else if((tonkoLogLevel>1) && (*yada == 'N')) ; \
                else if((tonkoLogLevel>2) && (*yada == 'W')) ; \
                else if((tonkoLogLevel>3) && (*yada == 'E')) ; \
                else if((tonkoLogLevel>4) && (*yada == 'O')) ; \
                else { \
                        rtsLogUnix_v(""SEV": "__FILE__" [line %d]: "STRING"\n" , __LINE__ , ##ARGS) ;\
		} \
	} while(0) \


#endif


#endif	/* __ROOT__ */

#ifdef __cplusplus
}
#endif

#endif	/* _RTS_LOG_H */
