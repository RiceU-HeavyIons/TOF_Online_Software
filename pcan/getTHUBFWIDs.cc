/* File name     : getTHUBFWIDs.cc
 * Creation date : 10/6/2008
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: getTHUBFWIDs.cc,v 1.1 2008-10-29 15:32:36 jschamba Exp $";
#endif /* lint */

//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

// other headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <libpcan.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>


//****************************************************************************
// DEFINES
// #define LOCAL_DEBUG

#define FIFO_FILE       "/tmp/pcanfifo"
#define FIFO_RESPONSE "/tmp/pcanfifoRsp"


//****************************************************************************
// CODE 


//****************************************************************************
// here is where all is done
int getFWIDs(int devID)
{
  FILE *fp;
  int fifofd;
  int numRead; 
  TPCANMsg m;
  string cmdString;
  stringstream ss;
  int msgid;
  unsigned int fwID;
  int errno;


#ifdef LOCAL_DEBUG
  printf("open write FIFO\n");
#endif
  if((fp = fopen(FIFO_FILE, "w")) == NULL) {
    perror("fopen");
    exit(1);
  }
  
  /* this blocks until the other side is open for Write */
#ifdef LOCAL_DEBUG
  printf("open FIFO\n");
#endif
  fifofd = open(FIFO_RESPONSE, O_RDONLY);
  if (fifofd == -1) {
    perror("open response FIFO");
    exit(1);
  }
    
  struct pollfd pfd;
  pfd.fd = fifofd;
  pfd.events = POLLIN;
    
  // notify pcanloop that we want response messages
#ifdef LOCAL_DEBUG
  printf("sending w command\n");
#endif
  fputs("w", fp); fflush(fp);
  // read dummy response
  int dummy;
  numRead = read(fifofd, &dummy, 4);
  if(numRead < 0) {
    perror("dummy read");
    printf("didn't get connection response\n");
    exit(1);
  }

  // -----------------------------------------
  // first all  of the FPGAs

  msgid = 0x404;
  for (int fpganum = 0; fpganum<9; fpganum++) {
    
    
    // send the "GET_FIRMWARE_ID" CANbus HLP message
#ifdef LOCAL_DEBUG
    printf("sending command\n");
#endif
    
    ss.str("");
    ss << "m s 0x404 2 0x2 " << showbase << hex << fpganum
       << " " << dec << devID;
    cmdString = ss.str();
    
#ifdef LOCAL_DEBUG
    cout << "cmdString = " << cmdString << endl;
#endif
    
    fputs(cmdString.c_str(), fp); fflush(fp);

    
    
#ifdef LOCAL_DEBUG
    printf("reading response, sizeof(m) = %d\n", sizeof(m));
#endif
    errno = poll(&pfd, 1, 1000); // timeout = 1 second
    if (errno != 1) {
      cout << "no response within timeout";
      if (errno < 0)
	cout << ", poll returned " << errno;
      cout << endl;
      fputs("W", fp); fflush(fp);
      fclose(fp);
      close(fifofd);
      return errno;
    }
      
    numRead = read(fifofd, &m, sizeof(m));
    if(numRead < 0) perror("read");
    
    
#ifdef LOCAL_DEBUG
    printf("number of bytes read %d\n", numRead);
#endif
    
    if (numRead > 0) {
#ifdef LOCAL_DEBUG
      printf("testit: message received : %c %c 0x%08x %1d  ", 
	     (m.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
	     (m.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
	     m.ID, 
	     m.LEN); 
      
      for (int i=0; i<m.LEN; i++)
	printf("0x%02x ", m.DATA[i]);
      printf("\n");
#endif
    }
    
    // print firmware IDs
    fwID = m.DATA[0] + (m.DATA[1] << 8) + (m.DATA[2] << 16) + (m.DATA[3] << 24);
    cout << "FPGA " << dec << fpganum
	 << ": Firmware User ID = " << showbase << hex << fwID << endl;
  } // for (int fpganum...

  // --------------------------------------
  // Now do the MCU:

  ss.str("");
  ss << "m s 0x404 1 0x1 "
     << dec << devID;
  cmdString = ss.str();
  
#ifdef LOCAL_DEBUG
  cout << "cmdString = " << cmdString << endl;
#endif
  
  fputs(cmdString.c_str(), fp); fflush(fp);
  
  
  
#ifdef LOCAL_DEBUG
  printf("reading response, sizeof(m) = %d\n", sizeof(m));
#endif
  errno = poll(&pfd, 1, 1000); // timeout = 1 second
  if (errno != 1) {
    cout << "no response within timeout";
    if (errno < 0)
      cout << ", poll returned " << errno;
    cout << endl;
    fputs("W", fp); fflush(fp);
    fclose(fp);
    close(fifofd);
    return errno;
  }
  numRead = read(fifofd, &m, sizeof(m));
  if(numRead < 0) perror("read");
  
  
#ifdef LOCAL_DEBUG
  printf("number of bytes read %d\n", numRead);
#endif
  
  if (numRead > 0) {
#ifdef LOCAL_DEBUG
    printf("testit: message received : %c %c 0x%08x %1d  ", 
	   (m.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
	   (m.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
	   m.ID, 
	   m.LEN); 
    
    for (int i=0; i<m.LEN; i++)
      printf("0x%02x ", m.DATA[i]);
    printf("\n");
#endif
  }
  
  // print firmware IDs
  fwID = m.DATA[0] + (m.DATA[1] << 8);
  cout << "\nMCU Firmware ID = " << showbase << hex << fwID
       << endl;
    // --------------------------  
  // tell pcanloop that we no longer want response messages
#ifdef LOCAL_DEBUG
  printf("sending W command\n");
#endif
  fputs("W", fp); fflush(fp);
  fclose(fp);
  close(fifofd);

  return(0);

}



//****************************************************************************
// main entry point
int main(int argc, char *argv[])
{
  int devID;

  cout << vcid << endl;
  cout.flush();
  
  
  if (argc == 2) {
    devID = strtol(argv[1], (char **)NULL, 0);
    if ((devID < 1) || (devID > 255)) {
      cerr << "devID = " << devID << " invalid entry. Use 1..255 instead." << endl;
      return -1;
    }
  }
  else
    devID = 255;

  return getFWIDs(devID);
}
