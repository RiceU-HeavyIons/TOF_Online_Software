#ifndef lint
static char vcid[] = "$Id$";
static const char __attribute__ ((used )) *Get_vcid(){return vcid;}
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

//#define DEBUG 1

static volatile int running = 1;

void signal_handler(int sig)
{
  switch(sig) {
  case SIGHUP:
    //log_message(LOG_FILE, "hangup signal caught");
    break;
  case SIGTERM:
    //log_message(LOG_FILE, "terminate signal caught");
    running = 0;
    break;
  case SIGINT:
    //log_message(LOG_FILE, "interrupt signal caught");
    running = 0;
    break;
  }
}


int main(int argc, char **argv)
{
  fd_set rdfs;
  int vsock;
  int ret;
  struct sockaddr_can addr;
  char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32))];
  struct iovec iov;
  struct msghdr msg;
  struct can_frame frame;
  struct can_frame rspframe;
  int nbytes;
  struct ifreq ifr;
  double temp;
		
  if (argc < 2) {
    printf("USAGE: %s devName\n", argv[0]);
    return 1;
  }

  signal(SIGTERM, signal_handler);
  signal(SIGHUP, signal_handler);
  signal(SIGINT, signal_handler);

#ifdef DEBUG
  printf("opening %s.\n", argv[1]);
#endif
  vsock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (vsock < 0) {
    perror("socket");
    return 1;
  }

  addr.can_family = AF_CAN;

  memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
  sprintf(ifr.ifr_name, "%s", argv[1]);
  if (ioctl(vsock, SIOCGIFINDEX, &ifr) < 0) {
    perror("SIOCGIFINDEX");
    exit(1);
  }
  addr.can_ifindex = ifr.ifr_ifindex;

  if (bind(vsock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    return 1;
  }
    
  /* these settings are static and can be held out of the hot path */
  iov.iov_base = &frame;
  msg.msg_name = &addr;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = &ctrlmsg;

  while (running) {
    // select set:
    FD_ZERO(&rdfs);
    FD_SET(vsock, &rdfs); // TOF

    if ((ret = select(vsock+1, &rdfs, NULL, NULL, NULL)) < 0) {
      if (errno != EINTR) {
	perror("select"); // select error, exit at next iteration
      }
      running = 0;
      continue;
    }

    /* these settings may be modified by recvmsg() */
    iov.iov_len = sizeof(frame);
    msg.msg_namelen = sizeof(addr);
    msg.msg_controllen = sizeof(ctrlmsg);  
    msg.msg_flags = 0;
    
    nbytes = recvmsg(vsock, &msg, 0);
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
      
#ifdef DEBUG
    // idx = idx2dindex(addr.can_ifindex, s[i]);
    // printf("%*s, i = %d", (int)max_devname_len, devname[idx], i);
    //fprint_long_canframe(stdout, &frame, NULL, view);
    printf("%s: 0x%x [%d] ", argv[1], frame.can_id, frame.can_dlc);
    for (int i=0; i<frame.can_dlc; i++)
      printf("0x%x ", frame.data[i]);
    //printf("\n");
    printf(":");
#endif

    //-------------------THUB----------------------------
    if ((frame.can_id & 0x80000ff0) == 0x400) { // THUB
#ifdef DEBUG      
      printf("THUB frame: ");
#endif
      frame.can_id |= 0x1;
      switch (frame.data[0]) {
      case 0x01: // MCU Firmware ID
#ifdef DEBUG
	printf("MCU Firmware ID\n");
#endif
	frame.can_dlc = 8;
	frame.data[1] = 0x54;
	frame.data[2] = 0x11;
	frame.data[3] = 0x12;
	frame.data[4] = 0x13;
	frame.data[5] = 0x14;
	frame.data[6] = 0x15;
	frame.data[7] = 0x16;
	break;
      case 0x02: // FPGA Firmware ID
#ifdef DEBUG
	printf("FPGA %d Firmware ID\n", frame.data[1]);
#endif
	frame.can_dlc = 4;
	if (frame.data[1] == 0) { //master
	  frame.data[2] = 0x21;
	  frame.data[3] = 0x22;
	}
	else { // serdes
	  frame.data[2] = 0x31;
	  frame.data[3] = 0x32;
	}
	break;
      case 0x03: // Temperature
#ifdef DEBUG
	printf("Temperature\n");
#endif
	temp = 32.0;
	frame.can_dlc = 2;
	frame.data[0] = 0xff & (int)(temp*128.0);
	frame.data[1] = 0xff & (int)(temp/2.0);
	break;
      case 0x05: // CRC
#ifdef DEBUG
	printf("CRC\n");
#endif
	frame.can_dlc = 2;
	frame.data[0] = 0x01;
	frame.data[1] = 0x0;
	break;
      case 0x91:
      case 0x92:
      case 0x93:
      case 0x94:
      case 0x95:
      case 0x96:
      case 0x97:
#ifdef DEBUG
	printf("Serdes\n");
#endif
	frame.can_dlc = 1;
	frame.data[0] = 0x1f;
	break;
      case 0x98:
#ifdef DEBUG
	printf("Serdes\n");
#endif
	frame.can_dlc = 1;
	frame.data[0] = 0x17;
	break;
      default:
#ifdef DEBUG
	printf("not handled\n");
#endif
	break;
      }
      // respond back:
      if ((nbytes = write(vsock, &frame, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
    }

    //-----------------------TCPU--------------------------
    else if ((frame.can_id & 0x80000e00) == 0x200) { // TCPU
      int i, j, len;
#ifdef DEBUG
      printf("TCPU frame: ");
#endif
      rspframe.can_id = frame.can_id | 0x1;
      rspframe.can_dlc = frame.can_dlc;
      for (i=0; i<frame.can_dlc; i++) rspframe.data[i] = frame.data[i];

      switch (frame.data[0]) {
      case 0x0e:
#ifdef DEBUG
	printf("read FPGA register\n");
#endif
	len = frame.can_dlc;
	rspframe.can_dlc = 1 + 2*(len - 1);
	for (i = 1; i < len; ++i) {
	  j = 2*i - 1;
	  if (frame.data[i] == 0x2) {
	    rspframe.data[j]     = 0x02;
	    rspframe.data[j + 1] = 0x0f;
	  }
	  else if (frame.data[i] == 0x3) {
	    rspframe.data[j]     = 0x03;
	    rspframe.data[j + 1] = 0x02;
	  }
	  else if (frame.data[i] == 0xe) {
	    rspframe.data[j]     = 0x00;
	    rspframe.data[j + 1] = 0x02;
	  }
	}
	break;
      case 0xb0: // status
#ifdef DEBUG
	printf("status\n");
#endif
	temp = 32.1;
	rspframe.can_dlc = 8;
	rspframe.data[1] = 0xff & (int)(temp*256.0); // temperature xx.16
	rspframe.data[2] = 0xff & (int)temp; // temperature 32.xx
	rspframe.data[3] = 0x8b; // ECSR
	rspframe.data[4] = 0; // AD 1L
	rspframe.data[5] = 0; // AD 1H
	rspframe.data[6] = 0; // AD 2L
	rspframe.data[7] = 0; // AD 1H
	break;
      case 0xb1: // Firmware ID
#ifdef DEBUG
	printf("Firmware\n");
#endif
	rspframe.can_dlc = 4;
	rspframe.data[1] = 0x45; // MCU.L
	rspframe.data[2] = 0x02; // MCU.H
	rspframe.data[3] = 0x8f; // FPGA
	break;
      case 0xb2: // Board Serial #
#ifdef DEBUG
	printf("Serial\n");
#endif
	rspframe.can_dlc = 8;
	rspframe.data[1] = 0x20;
	rspframe.data[2] = 0x21;
	rspframe.data[3] = 0x22;
	rspframe.data[4] = 0x23;
	rspframe.data[5] = 0x24;
	rspframe.data[6] = 0x25;
	rspframe.data[7] = 0x26;
	break;
      default:
#ifdef DEBUG
	printf("not handled\n");
#endif
	break;
      }
      // respond back:
      if ((nbytes = write(vsock, &rspframe, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
    }

    //-----------------------------TDIG---------------------------
    else if ((frame.can_id & 0x800000e0) ==  0x80000020) { // TDIG
      int i;
      bool isTDCResponse = false; 
#ifdef DEBUG
      printf("TDIG frame: ");
#endif
      rspframe.can_id = frame.can_id | 0x40000;
      rspframe.can_dlc = frame.can_dlc;
      for (i=0; i<frame.can_dlc; i++) rspframe.data[i] = frame.data[i];

      switch (frame.data[0]) {
      case 0x05:
      case 0x06:
      case 0x07: // TDC status
#ifdef DEBUG
	printf("TDC status word\n");
#endif
	// still needs to be handled. need 2 response packets....
	rspframe.can_dlc = 8;
	rspframe.data[1] = 0x50;
	rspframe.data[2] = 0x51;
	rspframe.data[3] = 0x52;
	rspframe.data[4] = 0x53;
	rspframe.data[5] = 0x54;
	rspframe.data[6] = 0x55;
	rspframe.data[7] = 0x56;
	isTDCResponse = true;
	break;
      case 0x08: // threshold
#ifdef DEBUG
	printf("threshold\n");
#endif
	temp = 1200.0;
	temp = temp*4095.0/3300.0 + 0.5;
	rspframe.can_dlc = 3;
	rspframe.data[1] = 0xff & (int)temp;
	rspframe.data[2] = 0xff & (((int)temp)>>8);
	break;
      case 0x0e: // fpga register read (assume reg3)
#ifdef DEBUG
	printf("read FPGA register\n");
#endif
	rspframe.can_dlc = 3;
	rspframe.data[2] = 0;
	break;
      case 0xb0: // status
#ifdef DEBUG
	printf("status\n");
#endif
	temp = 34.3;
	rspframe.can_dlc = 8;
	rspframe.data[1] = 0xff & (int)(temp*256.0); // temperature xx.16
	rspframe.data[2] = 0xff & (int)temp; // temperature 32.xx
	rspframe.data[3] = 0xbb; // ECSR
	rspframe.data[4] = 0; // AD 1L
	rspframe.data[5] = 0; // AD 1H
	rspframe.data[6] = 0; // AD 2L
	rspframe.data[7] = 0; // AD 1H
	break;
      case 0xb1: // Firmware ID
#ifdef DEBUG
	printf("Firmware\n");
#endif
	rspframe.can_dlc = 4;
	rspframe.data[1] = 0x46; // MCU.L
	rspframe.data[2] = 0x03; // MCU.H
	rspframe.data[3] = 0x79; // FPGA
	break;
      case 0xb2: // Board Serial #
#ifdef DEBUG
	printf("Serial\n");
#endif
	rspframe.can_dlc = 8;
	rspframe.data[1] = 0x10;
	rspframe.data[2] = 0x11;
	rspframe.data[3] = 0x12;
	rspframe.data[4] = 0x13;
	rspframe.data[5] = 0x14;
	rspframe.data[6] = 0x15;
	rspframe.data[7] = 0x16;
	break;
       default:
#ifdef DEBUG
	printf("not handled\n");
#endif
	break;
      }
      // respond back:
      if ((nbytes = write(vsock, &rspframe, sizeof(frame))) != sizeof(frame)) {
	perror("write"); running=0; continue;
      }
      if (isTDCResponse) { // send an additional response with 2 bytes
	isTDCResponse = false;
	rspframe.can_dlc = 2;
	if ((nbytes = write(vsock, &rspframe, sizeof(frame))) != sizeof(frame)) {
	  perror("write"); running=0; continue;
	}
      }
    }

  } // while running

  close(vsock);

  return 0;
}
