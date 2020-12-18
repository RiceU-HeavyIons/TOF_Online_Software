#ifndef lint
static char vcid[] = "$Id: recoveryd.cc 874 2013-03-18 20:45:38Z jschamba $";
static const char __attribute__ ((used )) *Get_vcid(){return vcid;}
#endif /* lint */

#include <vector>
using namespace std;

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

const int THUB_NW = 101;
const int THUB_NE = 102;
const int THUB_SW = 103;
const int THUB_SE = 106;
const int MTD_S = 104;
const int MTD_N = 107;
const int VPD_E = 105;
const int VPD_W = 100;

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
  int s[MAXSOCK+100]; //fg the range of active canbus IDs is moved up from 0-7 to 100-107
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
  bool doRecovery, doRecoveryMTD, doRecoveryTOF;
  struct ifreq ifr;
  struct timeval t;
  int max_socket_fd;
  vector<int> tofTHUBs;
  vector<int> mtdTHUBs;

  daemonize();

  // output svn version to log
  log_message(LOG_FILE, vcid);

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
  max_socket_fd = 0;
  log_message(LOG_FILE, "Opening can interfaces");


  for (i=100; i < 100+currmax; i++) { //fg the range of active canbus IDs is moved up from 0-7 to 100-107
#ifdef DEBUG
    printf("opening can%d.\n", i);
#endif
    s[i] = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s[i] < 0) {
      sprintf(logstr, "error opening can%d: socket", i);
      log_message(LOG_FILE, logstr);
      perror("socket");
      return 1;
    }

    addr.can_family = AF_CAN;

    memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
    sprintf(ifr.ifr_name, "can%d", i);
    if (ioctl(s[i], SIOCGIFINDEX, &ifr) < 0) {
      sprintf(logstr, "error opening can%d: SIOCGIFINDEX", i);
      log_message(LOG_FILE, logstr);
      perror("SIOCGIFINDEX");
      exit(1);
    }
    addr.can_ifindex = ifr.ifr_ifindex;

    if ( (i == VPD_E) || (i == VPD_W) ) {
      // disable default receive filter on this RAW socket
      // This is obsolete as we do not read from the socket at all, but for
      // this reason we can remove the receive list in the Kernel to save a
      // little (really a very little!) CPU usage.
      setsockopt(s[i], SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
    }
    else {
      // CAN Filter and mask for this socket
      rfilter.can_id = 0x407;
      rfilter.can_mask = 0xc00007ff;
      setsockopt(s[i], SOL_CAN_RAW, CAN_RAW_FILTER,
		 &rfilter, sizeof(struct can_filter));
    }

    if (bind(s[i], (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      sprintf(logstr, "error opening can%d: bind", i);
      log_message(LOG_FILE, logstr);
      perror("bind");
      return 1;
    }
  }

  // fill vectors:
  tofTHUBs.push_back(THUB_NW);
  tofTHUBs.push_back(THUB_SW);
  tofTHUBs.push_back(THUB_NE);
  tofTHUBs.push_back(THUB_SE);

  mtdTHUBs.push_back(MTD_S);
  mtdTHUBs.push_back(MTD_N);

  // check the highest socket we are interested in:
  for(vector<int>::iterator it = tofTHUBs.begin(); it != tofTHUBs.end(); ++it) {
    if(s[*it] > max_socket_fd) max_socket_fd = s[*it];
  }

  for(vector<int>::iterator it = mtdTHUBs.begin(); it != mtdTHUBs.end(); ++it) {
    if(s[*it] > max_socket_fd) max_socket_fd = s[*it];
  }

  // nfds argument of select is max socket + 1
  max_socket_fd += 1;



  /* these settings are static and can be held out of the hot path */
  iov.iov_base = &frame;
  msg.msg_name = &addr;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = &ctrlmsg;

  while (running) {
    doRecovery = doRecoveryMTD = doRecoveryTOF = false;
    // select set: all THUBs
    FD_ZERO(&rdfs);
    for(vector<int>::iterator it = tofTHUBs.begin(); it != tofTHUBs.end(); ++it) {
      FD_SET(s[*it], &rdfs);
    }
    for(vector<int>::iterator it = mtdTHUBs.begin(); it != mtdTHUBs.end(); ++it) {
      FD_SET(s[*it], &rdfs);
    }

    //    for (i=0; i<currmax; i++)
    //FD_SET(s[i], &rdfs);


    // wait forever
    ret = select(max_socket_fd, &rdfs, NULL, NULL, NULL);
    if (ret <= 0) {
      if (errno != EINTR) {
	sprintf(logstr, "select error: %d", errno);
	log_message(LOG_FILE, logstr);
	perror("select"); // select error, exit at next iteration
      }
      running = 0;
      continue;
    }
    // else if (ret == 0) continue; // timeout

    // select set: all THUBs
    FD_ZERO(&rdfs);
    for(vector<int>::iterator it = tofTHUBs.begin(); it != tofTHUBs.end(); ++it) {
      FD_SET(s[*it], &rdfs);
    }
    for(vector<int>::iterator it = mtdTHUBs.begin(); it != mtdTHUBs.end(); ++it) {
      FD_SET(s[*it], &rdfs);
    }
    // Wait a little
    usleep(400000); //  0.4 sec

    // now check one more time to see if additional THUBs are ready
    // do this with 0 timeout to return immediately
    // timeout values (watch out, select may change this)
    t.tv_sec  = 0;
    t.tv_usec = 0;
    //t.tv_usec = 500000; // 0.5 seconds
    ret = select(max_socket_fd, &rdfs, NULL, NULL, &t);
    if (ret < 0) {
      if (errno != EINTR) {
	sprintf(logstr, "select error: %d", errno);
	log_message(LOG_FILE, logstr);
	perror("select"); // select error, exit at next iteration
      }
      running = 0;
      continue;
    }

    // Now iterate over TOF THUBs
    for(vector<int>::iterator it = tofTHUBs.begin(); it != tofTHUBs.end(); ++it) {
      i = *it;
      if ( FD_ISSET(s[i], &rdfs) ) {

	//int idx;
	/* these settings may be modified by recvmsg() */
	iov.iov_len = sizeof(frame);
	msg.msg_namelen = sizeof(addr);
	msg.msg_controllen = sizeof(ctrlmsg);
	msg.msg_flags = 0;

	nbytes = recvmsg(s[i], &msg, 0);
	if (nbytes < 0) {
	  sprintf(logstr, "can%d: read error %d", i, nbytes);
	  log_message(LOG_FILE, logstr);
	  perror("read");
	  running = 0; continue;
	}

	if ((size_t)nbytes < sizeof(struct can_frame)) {
#ifdef DEBUG
	  fprintf(stderr, "read: incomplete CAN frame\n");
#endif
	  sprintf(logstr, "can%d: read only %d bytes, expected %d", i, nbytes,
		  (int)sizeof(struct can_frame));
	  log_message(LOG_FILE, logstr);
	  running = 0; continue;
	}

	if ((frame.data[0] == 0xff) && (frame.data[1] == 0x55)) {
#ifdef DEBUG
	  // idx = idx2dindex(addr.can_ifindex, s[i]);
	  // printf("%*s, i = %d", (int)max_devname_len, devname[idx], i);
	  //fprint_long_canframe(stdout, &frame, NULL, view);
	  printf("can%d: 0x%x: 0x%x 0x%x 0x%x 0x%x", i, frame.can_id,
		 frame.data[0], frame.data[1], frame.data[2], frame.data[3]);
	  printf("\n");
#endif
	  sprintf(logstr, "can%d: 0x%x: 0x%x 0x%x 0x%x 0x%x", i, frame.can_id,
		  frame.data[0], frame.data[1], frame.data[2], frame.data[3]);
	  log_message(LOG_FILE, logstr);
	  doRecoveryTOF = doRecovery = true;
	}
      }
    }

    // Now iterate over MTD THUBs
    for(vector<int>::iterator it = mtdTHUBs.begin(); it != mtdTHUBs.end(); ++it) {
      i = *it;
      if ( FD_ISSET(s[i], &rdfs) ) {

	//int idx;
	/* these settings may be modified by recvmsg() */
	iov.iov_len = sizeof(frame);
	msg.msg_namelen = sizeof(addr);
	msg.msg_controllen = sizeof(ctrlmsg);
	msg.msg_flags = 0;

	nbytes = recvmsg(s[i], &msg, 0);
	if (nbytes < 0) {
	  sprintf(logstr, "can%d: read error %d", i, nbytes);
	  log_message(LOG_FILE, logstr);
	  perror("read");
	  running = 0; continue;
	}

	if ((size_t)nbytes < sizeof(struct can_frame)) {
#ifdef DEBUG
	  fprintf(stderr, "read: incomplete CAN frame\n");
#endif
	  sprintf(logstr, "can%d: read only %d bytes, expected %d", i, nbytes,
		  (int)sizeof(struct can_frame));
	  log_message(LOG_FILE, logstr);
	  running = 0; continue;
	}

	if ((frame.data[0] == 0xff) && (frame.data[1] == 0x55)) {
#ifdef DEBUG
	  // idx = idx2dindex(addr.can_ifindex, s[i]);
	  // printf("%*s, i = %d", (int)max_devname_len, devname[idx], i);
	  //fprint_long_canframe(stdout, &frame, NULL, view);
	  printf("can%d: 0x%x: 0x%x 0x%x 0x%x 0x%x", i, frame.can_id,
		 frame.data[0], frame.data[1], frame.data[2], frame.data[3]);
	  printf("\n");
#endif
	  sprintf(logstr, "can%d: 0x%x: 0x%x 0x%x 0x%x 0x%x", i, frame.can_id,
		  frame.data[0], frame.data[1], frame.data[2], frame.data[3]);
	  log_message(LOG_FILE, logstr);
	  doRecoveryMTD = doRecovery = true;
	}
      }
    }


    // Now do the actual recovery
    if (doRecovery) {
      log_message(LOG_FILE, "Recovery...");
      // First do all of the THUB resets
      if (doRecoveryTOF) {
	log_message(LOG_FILE, "TOF...");
	if ((nbytes = write(s[THUB_NW], &thubframe, sizeof(frame))) != sizeof(frame)) {
	  perror("write"); running=0; continue;
	}
	if ((nbytes = write(s[THUB_NE], &thubframe, sizeof(frame))) != sizeof(frame)) {
	  perror("write"); running=0; continue;
	}
	if ((nbytes = write(s[THUB_SW], &thubframe, sizeof(frame))) != sizeof(frame)) {
	  perror("write"); running=0; continue;
	}
	if ((nbytes = write(s[THUB_SE], &thubframe, sizeof(frame))) != sizeof(frame)) {
	  perror("write"); running=0; continue;
	}
      }

      if (doRecoveryMTD) {
	log_message(LOG_FILE, "MTD...");
	if ((nbytes = write(s[MTD_S], &thubframe, sizeof(frame))) != sizeof(frame)) {
	  perror("write"); running=0; continue;
	}
	if ((nbytes = write(s[MTD_N], &thubframe, sizeof(frame))) != sizeof(frame)) {
	  perror("write"); running=0; continue;
	}
      }

      // Wait a little to let the THUBs reset first
      usleep(700000); // 0.7 sec

      // Now do all the TCPU resets
      if (doRecoveryTOF) {
	// TOF
	if ((nbytes = write(s[THUB_NW], &tcpuframe, sizeof(frame))) != sizeof(frame)) {
	  perror("write THUB_NW"); running=0; continue;
	}
	if ((nbytes = write(s[THUB_NE], &tcpuframe, sizeof(frame))) != sizeof(frame)) {
	  perror("write THUB_NE"); running=0; continue;
	}
	if ((nbytes = write(s[THUB_SW], &tcpuframe, sizeof(frame))) != sizeof(frame)) {
	  perror("write THUB_SW"); running=0; continue;
	}
	if ((nbytes = write(s[THUB_SE], &tcpuframe, sizeof(frame))) != sizeof(frame)) {
	  perror("write THUB_SE"); running=0; continue;
	}
	// VPD
	if ((nbytes = write(s[VPD_W], &tcpuframe, sizeof(frame))) != sizeof(frame)) {
	  perror("write VPD_W"); running=0; continue;
	}
	if ((nbytes = write(s[VPD_E], &tcpuframe, sizeof(frame))) != sizeof(frame)) {
	  perror("write VPD_E"); running=0; continue;
	}
      }

      if (doRecoveryMTD) {
	// MTD
	if ((nbytes = write(s[MTD_S], &tcpuframe, sizeof(frame))) != sizeof(frame)) {
	  perror("write MTD_S"); running=0; continue;
	}
	if ((nbytes = write(s[MTD_N], &tcpuframe, sizeof(frame))) != sizeof(frame)) {
	  perror("write MTD_N"); running=0; continue;
	}
      }

      // Wait a while to let TCPUs reset
      usleep(700000); // 0.7 sec

      // Finally do a bunch reset
      if ((nbytes = write(s[VPD_W], &brstframe, sizeof(frame))) != sizeof(frame)) {
	perror("write VPD_W"); running=0; continue;
      }
      log_message(LOG_FILE, "...finished");

    } // if doRecovery


  } // while running

  log_message(LOG_FILE, "exiting...");
  for (i=0; i<currmax; i++)
    close(s[i]);

  return 0;
}
