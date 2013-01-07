/*
 *  
 */
#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id$";
#endif /* lint */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <libgen.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>


#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#define MAXSOCK 8    /* max. number of CAN interfaces given on the cmdline */
#define MAXIFNAMES 30 /* size of receive name index to omit ioctls */
#define ANYDEV "any"  /* name of interface to receive from any CAN interface */
#define ANL "\r\n"    /* newline in ASC mode */

#define RUNNING_DIR	"/tmp"
#define LOCK_FILE	"recoveryd.pid"
#define LOG_FILE	"recoveryd.log"

//#define DEBUG 1

static char devname[MAXIFNAMES][IFNAMSIZ+1];
static int  dindex[MAXIFNAMES];
static size_t  max_devname_len; /* to prevent frazzled device name output */ 

extern int optind, opterr, optopt;

static volatile int running = 1;

void log_message(const char *filename, const char *message)
{
  FILE *logfile;
  time_t ltime; /* calendar time */
  struct tm tm;
  char timestring[25];

  ltime = time(NULL); /* get current cal time */
  tm = *localtime(&ltime);
  strftime(timestring, 24, "%Y-%m-%d %H:%M:%S", &tm);
  logfile = fopen(filename,"a");
  if(!logfile) return;
  fprintf(logfile,"%s: %s\n", timestring, message);
  fclose(logfile);
}


//void sigterm(int signo)
//{
//  running = 0;
//}

void signal_handler(int sig)
{
  switch(sig) {
  case SIGHUP:
    log_message(LOG_FILE, "hangup signal caught");
    break;
  case SIGTERM:
    log_message(LOG_FILE, "terminate signal caught");
    running = 0;
    break;
  case SIGINT:
    log_message(LOG_FILE, "interrupt signal caught");
    running = 0;
    break;
  }
}

void daemonize()
{
  int i,lfp;
  char str[10];
  
  if(getppid() == 1) return; // already a daemon
  i = fork();
  if (i < 0) exit(1); // fork error
  if (i > 0) exit(0); // parent exits
  // child (daemon) continues
  setsid(); // obtain a new process group
  for (i=getdtablesize(); i>=0; --i) close(i); // close all descriptors
  i = open("/dev/null", O_RDWR); dup(i); dup(i); // handle standard I/O
  umask(027); // set newly created file permissions
  chdir(RUNNING_DIR); // change running directory
  lfp = open(LOCK_FILE, O_RDWR|O_CREAT, 0640);
  if (lfp < 0) exit(1); // can not open
  if (lockf(lfp, F_TLOCK, 0) < 0) exit(0); // can not lock
  // first instance continues
  sprintf(str, "%d\n", getpid());
  write(lfp, str, strlen(str)); // record pid to lockfile
  signal(SIGCHLD, SIG_IGN); // ignore child
  signal(SIGTSTP, SIG_IGN); // ignore tty signals
  signal(SIGTTOU, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGHUP, signal_handler); // catch hangup signal
  signal(SIGTERM, signal_handler); // catch kill signal
  signal(SIGINT, signal_handler); // catch interrupt signal
}

#ifdef DEBUG
void print_usage(char *prg)
{
  fprintf(stderr, "\nUsage: %s [options] <CAN interface>+\n", prg);
  fprintf(stderr, "  (use CTRL-C to terminate %s)\n\n", prg);
}
#endif

int idx2dindex(int ifidx, int socket) {

  int i;
  struct ifreq ifr;

  for (i=0; i < MAXIFNAMES; i++) {
    if (dindex[i] == ifidx)
      return i;
  }

  /* create new interface index cache entry */

  /* remove index cache zombies first */
  for (i=0; i < MAXIFNAMES; i++) {
    if (dindex[i]) {
      ifr.ifr_ifindex = dindex[i];
      if (ioctl(socket, SIOCGIFNAME, &ifr) < 0)
	dindex[i] = 0;
    }
  }

  for (i=0; i < MAXIFNAMES; i++)
    if (!dindex[i]) /* free entry */
      break;

  if (i == MAXIFNAMES) {
#ifdef DEBUG
    fprintf(stderr, "Interface index cache only supports %d interfaces.\n",
	    MAXIFNAMES);
#endif
    exit(1);
  }

  dindex[i] = ifidx;

  ifr.ifr_ifindex = ifidx;
  if (ioctl(socket, SIOCGIFNAME, &ifr) < 0)
    perror("SIOCGIFNAME");

  if (max_devname_len < strlen(ifr.ifr_name))
    max_devname_len = strlen(ifr.ifr_name);

  strcpy(devname[i], ifr.ifr_name);

#ifdef DEBUG
  printf("new index %d (%s)\n", i, devname[i]);
#endif

  return i;
}

int main(int argc, char **argv)
{
  char logstr[80];
  fd_set rdfs;
  int s[MAXSOCK];
  int ret;
  int currmax;
  struct sockaddr_can addr;
  char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32))];
  struct iovec iov;
  struct msghdr msg;
  struct can_filter rfilter;
  struct can_frame frame;
  struct can_frame thubframe;
  struct can_frame tcpuframe;
  struct can_frame brstframe;
  int nbytes, i;
  bool doRecovery;
  struct ifreq ifr;

  daemonize();

  // THUB FPGA reset command
  thubframe.can_id = 0x402;
  thubframe.can_dlc = 1;
  thubframe.data[0] = 0x0d;

  // TCPU FPGA reset broadcast command
  tcpuframe.can_id = 0x7f2;
  tcpuframe.can_dlc = 1;
  tcpuframe.data[0] = 0x60;

  // TOCK bunch reset command
  brstframe.can_id = 0x412;
  brstframe.can_dlc = 1;
  brstframe.data[0] = 0x70;


  // signal(SIGTERM, sigterm);
  // signal(SIGHUP, sigterm);
  // signal(SIGINT, sigterm);

  //signal(SIGTERM, signal_handler);
  //signal(SIGHUP, signal_handler);
  //signal(SIGINT, signal_handler);

  //print_usage(basename(argv[0]));

  currmax = 8;

  log_message(LOG_FILE, "Opening can interfaces");
  for (i=0; i < currmax; i++) {
#ifdef DEBUG
    printf("opening can%d.\n", i);
#endif
    s[i] = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s[i] < 0) {
      perror("socket");
      return 1;
    }

    addr.can_family = AF_CAN;

    memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
    sprintf(ifr.ifr_name, "can%d", i);
    if (ioctl(s[i], SIOCGIFINDEX, &ifr) < 0) {
      perror("SIOCGIFINDEX");
      exit(1);
    }
    addr.can_ifindex = ifr.ifr_ifindex;

    if ( (i == 1) || (i == 5) ) {
      // CAN Filter and mask for this socket
      rfilter.can_id = 0x407; 
      rfilter.can_mask = 0xc00007ff;
      setsockopt(s[i], SOL_CAN_RAW, CAN_RAW_FILTER,
		 &rfilter, sizeof(struct can_filter));
    } 
    else {
      // disable default receive filter on this RAW socket
      // This is obsolete as we do not read from the socket at all, but for 
      // this reason we can remove the receive list in the Kernel to save a 
      // little (really a very little!) CPU usage.
      setsockopt(s[i], SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
    }

    if (bind(s[i], (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      perror("bind");
      return 1;
    }
  }

  /* these settings are static and can be held out of the hot path */
  iov.iov_base = &frame;
  msg.msg_name = &addr;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = &ctrlmsg;

  while (running) {
    doRecovery = false;
    // select set:
    FD_ZERO(&rdfs);
    FD_SET(s[1], &rdfs); // TOF
    FD_SET(s[5], &rdfs); // MTD

    //    for (i=0; i<currmax; i++)
    //FD_SET(s[i], &rdfs);

    if ((ret = select(s[currmax-1]+1, &rdfs, NULL, NULL, NULL)) < 0) {
      if (errno != EINTR) perror("select"); // select error, exit at next iteration
      running = 0;
      continue;
    }

    i = 1;  // THUB NW
    if (FD_ISSET(s[i], &rdfs)) {
      //int idx;

      /* these settings may be modified by recvmsg() */
      iov.iov_len = sizeof(frame);
      msg.msg_namelen = sizeof(addr);
      msg.msg_controllen = sizeof(ctrlmsg);  
      msg.msg_flags = 0;
      
      nbytes = recvmsg(s[i], &msg, 0);
      if (nbytes < 0) {
	perror("read");
	running = 0; continue;
      }
      
      if ((size_t)nbytes < sizeof(struct can_frame)) {
#ifdef DEBUG
	fprintf(stderr, "read: incomplete CAN frame\n");
#endif
	running = 0; continue;
      }
      
      if ((frame.data[0] == 0xff) && (frame.data[1] == 0x55)) {
#ifdef DEBUG
	// idx = idx2dindex(addr.can_ifindex, s[i]);
	// printf("%*s, i = %d", (int)max_devname_len, devname[idx], i);
	//fprint_long_canframe(stdout, &frame, NULL, view);
	printf("can1: 0x%x: 0x%x 0x%x 0x%x 0x%x", frame.can_id,
	       frame.data[0], frame.data[1], frame.data[2], frame.data[3]);
	printf("\n");
#endif
	sprintf(logstr, "can%d: 0x%x: 0x%x 0x%x 0x%x 0x%x", i, frame.can_id,
		frame.data[0], frame.data[1], frame.data[2], frame.data[3]);
	log_message(LOG_FILE, logstr);
	doRecovery = true;
      }
    }
    
    i = 5;  // THUB MTD
    if (FD_ISSET(s[i], &rdfs)) {
      //int idx;

      iov.iov_len = sizeof(frame);
      msg.msg_namelen = sizeof(addr);
      msg.msg_controllen = sizeof(ctrlmsg);  
      msg.msg_flags = 0;
      
      nbytes = recvmsg(s[i], &msg, 0);
      if (nbytes < 0) {
	perror("read");
	running = 0; continue;
      }
      
      if ((size_t)nbytes < sizeof(struct can_frame)) {
#ifdef DEBUG
	fprintf(stderr, "read: incomplete CAN frame\n");
#endif
	running = 0; continue;
      }
      
      if ((frame.data[0] == 0xff) && (frame.data[1] == 0x55)) {
#ifdef DEBUG
	printf("can5: 0x%x: 0x%x 0x%x 0x%x 0x%x", frame.can_id,
	       frame.data[0], frame.data[1], frame.data[2], frame.data[3]);
	printf("\n");
#endif
	sprintf(logstr, "can%d: 0x%x: 0x%x 0x%x 0x%x 0x%x", i, frame.can_id,
		frame.data[0], frame.data[1], frame.data[2], frame.data[3]);
	log_message(LOG_FILE, logstr);
	doRecovery = true;
      }
    }

    if (doRecovery) {
      log_message(LOG_FILE, "Recovery...");
      // First do all of the THUB resets
      if ((nbytes = write(s[1], &thubframe, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
      if ((nbytes = write(s[2], &thubframe, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
      if ((nbytes = write(s[3], &thubframe, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
      if ((nbytes = write(s[5], &thubframe, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
      if ((nbytes = write(s[6], &thubframe, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
      if ((nbytes = write(s[7], &thubframe, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
	
      // Wait a little to let the THUBs reset first
      usleep(300000);
	
      // Now do all the TCPU resets
      // TOF
      if ((nbytes = write(s[1], &tcpuframe, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
      if ((nbytes = write(s[2], &tcpuframe, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
      if ((nbytes = write(s[3], &tcpuframe, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
      if ((nbytes = write(s[6], &tcpuframe, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
      // MTD
      if ((nbytes = write(s[5], &tcpuframe, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
      if ((nbytes = write(s[7], &tcpuframe, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
      // VPD
#ifdef NOTNOW
      if ((nbytes = write(s[0], &tcpuframe, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
      if ((nbytes = write(s[4], &tcpuframe, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
#endif
	
      // Wait a while to let TCPUs reset
      sleep(1);
	
      // Finally do a bunch reset
      if ((nbytes = write(s[0], &brstframe, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
      log_message(LOG_FILE, "...finished");

    } // if doRecovery


  } // while running

  log_message(LOG_FILE, "exiting...");
  for (i=0; i<currmax; i++)
    close(s[i]);

  return 0;
}
