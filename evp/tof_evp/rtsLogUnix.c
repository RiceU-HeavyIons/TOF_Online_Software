#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <string.h>
#include <syslog.h>
#include <stdlib.h>

#ifdef __linux__	
#else	/* Solaris */
#include <procfs.h>	
#endif


#include <rtsLog.h>


#ifdef __cplusplus
extern "C" {
#endif

volatile int tonkoLogLevel = 2 ;	


static char *getCmd(void) ;
static int  odesc = -1 ;
static int handchange ;

/* defaults */
#ifdef RTS_PROJECT_PP
static char servER[80] = "130.199.20.88" ;
#else
static char servER[80] = RTS_LOG_HOST ;
#endif

static int port = RTS_LOG_PORT ;
static int output_flag = RTS_LOG_NET ;
static char cmd[1024] ;

/*
	Thread issues:

	The routine itself should be thread safe however no guarantees
	are made to the operating system calls:
		sendto
		sprintf
		strlen
		perror
	The current (2.6) Solaris mentions that they should be.

	Additionally, the destination  node:port as well as the log severity
	level are COMMON (by design) to all threads.
*/

int rtsLogOutput(int flag)
{
	output_flag = flag ;
	return flag ;
}

void rtsLogAddCmd(const char *cmd_in)
{
	strncpy(cmd,cmd_in,sizeof(cmd)-1) ;
	return ;
}

int rtsLogAddDest(char *host, int newport)
{
	/* mark as changed by hand */
	handchange = 1 ;

	/* set the statics */
	strncpy(servER,host,sizeof(servER)-1) ;
	port = newport ;
	/* mark as not connected so that next send reconnects
	with the new values */
	if(odesc >= 0) close(odesc) ;	
	odesc = -1 ;


	return 0 ;
}



int rtsLogUnix_v(const char *str, ...) 
{
	/* common to all threads */
	static int sockAddrSize ;
	static struct sockaddr_in serverAddr ;
	static char *cmd_l ;
	/* thread dependant */
	int ret ;
	char buffer[1024] ;
	char *string = buffer ;
	unsigned int *cou = (unsigned int *)buffer ;
	int len ;
	int err = 0 ;
	va_list ap ;

	retry_connect: ;

	if((output_flag & RTS_LOG_NET) && (odesc<0)) {	/* set the socket up */
		char *rts_host ;

		int bufsize = 30*1500 ;

//		if(port==0) port = RTS_LOG_PORT ;

		if(!handchange && (rts_host=getenv("RTS_LOG_HOST"))) {
			strncpy(servER,rts_host,sizeof(servER)-1) ;
		}

	
		cmd_l = NULL ;

		sockAddrSize = sizeof(struct sockaddr_in) ; 
		memset((char *)&serverAddr,0,sockAddrSize) ;
		serverAddr.sin_family = AF_INET ;
		serverAddr.sin_port = htons(port) ;

		/* hostname */
		if((serverAddr.sin_addr.s_addr = inet_addr(servER)) == (unsigned int)-1) {
			perror(servER);
			return -1 ;
		}

		odesc = socket(AF_INET, SOCK_DGRAM,0) ;
		if(odesc < 0) {
			perror("socket") ;
			return -1 ;
		}

		bufsize = 128*1024 ;
		for(;;) {
			ret = setsockopt(odesc,SOL_SOCKET,SO_SNDBUF,(char *)&bufsize,sizeof(bufsize)) ;
			if(ret == 0) break ;
			bufsize /= 2 ;
		}
		//printf("2: set sockbuff to %d bytes\n",bufsize) ;

		cmd_l = getCmd() ;	// get the name of the running executable
	}

	len = 4 ;	/* to account for the counter */

	if(cmd[0]) {
		sprintf(string+len,"(%s): ",cmd) ;
		len += strlen(string+len) ;
	}
	else if(cmd_l) {
		sprintf(string+len,"(%s): ",cmd_l) ;
		len += strlen(string+len) ;
	}


	va_start(ap, str) ;
	vsprintf(string+len,str,ap) ;
	len += strlen(string+len) ;

	*cou = 0xFFFFFFFF ;	/* special handling by server! */

	/* send it to the right place via UDP */

	if(output_flag & RTS_LOG_NET) {
		ret = sendto(odesc,(char *)buffer,len,0,
			     (struct sockaddr *)&serverAddr,sockAddrSize) ;
		if(ret < 0) {
			syslog(LOG_USER|LOG_ERR,"LOG sendto returned %d [%m]\n",ret) ;
			close(odesc) ;
			odesc = -1 ;
			if(err == 0) {	// reconnect only once!
				err = 1 ;
				goto retry_connect ;
			}
		}
	}

	/* and/or to stderr */
	if(output_flag & RTS_LOG_STDERR) {
		fprintf(stderr,"%s",(char *)buffer+4) ;
	}


	return 0 ;
}



static char *getCmd(void)
{


	static char *str = "(no-name)" ;
#ifdef __linux__	
	FILE *file ;
	static char name[128] ;
	char *ptr ;

	file = fopen("/proc/self/cmdline","r") ;
	if(file==NULL) return str ;

	fscanf(file,"%s",name) ;
	fclose(file) ;

	if((ptr = strrchr(name,'/'))) {
		return ptr+1 ;
	}
	else {
		return name ;
	}
#else	// solaris
	int fd, ret ;
	static struct psinfo ps ;

	fd = open("/proc/self/psinfo",O_RDONLY,0666) ;
	if(fd < 0) return str ;

	ret = read(fd,(char *)&ps,sizeof(ps)) ;
	close(fd) ;

	if(ret != sizeof(ps)) {
		return str ;
	}

	return ps.pr_fname ;
#endif
}

#ifdef __cplusplus
}
#endif

