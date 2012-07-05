/* File name     : x_readHPTDCconfig_l.cc
 * Creation date : 05/17/2011
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

static char vcid[] = "$Id$";
static const char __attribute__ ((used )) *Get_vcid(){return vcid;}

//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
#include <fstream>
using namespace std;

// other headers
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>


// pcan include file
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

// local utilities
#include "can_utils.h"

//****************************************************************************
// DEFINES

//#define LOCAL_DEBUG

//****************************************************************************
// GLOBALS
int h = -1;

//****************************************************************************
// LOCALS

//****************************************************************************
// CODE 

// centralized entry point for all exits
static void my_private_exit(int error)
{
  if (h > 0)
  {
    
#ifdef LOCAL_DEBUG
  cout << "x_readHPTDCconfig: closing h\n";
#endif
    close(h); 
  }
#ifdef LOCAL_DEBUG
  cout << "x_readHPTDCconfig: finished (" << error << ")\n";
#endif
  exit(error);
}

// handles CTRL-C user interrupt
static void signal_handler(int signal)
{
  my_private_exit(0);
}



//**********************************************
// here all is done
//*********************************************
int x_readConfig(const char *filename, unsigned int tdigNodeID, unsigned int tcpuNodeID, int TDC, int devID)
{
  int i,j, k;
  ofstream conf;

  cout << "Storing configuration of TDC " << TDC
       << " at TDIG NodeID 0x" << hex << tdigNodeID
       << " through TCPU NodeID 0x" << tcpuNodeID
       << "\n into filename " << filename 
       << " on CANbus ID 0x" << devID << "...\n";

  errno = 0;
  
  // install signal handler for manual break
  signal(SIGINT, signal_handler);

  if((h = CAN_Open(devID)) < 0) {
    my_private_exit(-1);
  }

  conf.open(filename, ios_base::out | ios_base::trunc);

  int nodeIDVal = (((tdigNodeID << 4) | 0x004) << 18) | tcpuNodeID;

  struct can_frame mr[12];
  struct can_frame ms;
  ms.can_id = nodeIDVal | CAN_EFF_FLAG;
  ms.can_dlc = 1;
  ms.data[0] = 0x40+TDC;

#ifdef LOCAL_DEBUG
  printCANMsg(ms, "x_readHPTDCconfig: Sending RS_CONFIGTDCx command");
#endif

  if (write(h, &ms, sizeof(ms)) != sizeof(ms)) {
    perror("x_readHPTDCconfig:write()");
    my_private_exit(-1);
  }

  for (i=0; i<12; i++)
    CAN_Read_Timeout(h, &mr[i], 4000000); // timeout = 4sec

#ifdef LOCAL_DEBUG
  for (i=0; i<12; i++)
    printCANMsg(mr[i], "message received");
#endif

  for (j=0; j<12; j++) {
    for (k=1; k<mr[j].can_dlc; k++) {
      for (i=7; i>=0; i--)
	conf << ((mr[j].data[k] >> i)&1);
      conf << endl;
    }
  }


  conf.close();

  my_private_exit(0);
  
  return errno;
}


int main(int argc, char *argv[])
{
  unsigned int tdigNodeID, tcpuNodeID;
  int devID = 0;

  cout << vcid << endl;
  cout.flush();
  
  if ( argc < 5 ) {
    cout << "USAGE: " << argv[0] << " <TDC ID> <TDIG node ID> <TCPU node ID> <fileName> [<devID>]\n";
    return 1;
  }
  
  int TDC = atoi(argv[1]);
  if ((TDC < 1) || (TDC > 3)) { 
    cerr << "TDC = " << TDC << " invalid entry. Use 1-3 instead." << endl;
    return 1;
  }

  tdigNodeID =  strtol(argv[2], (char **)NULL, 0);
  if ((tdigNodeID < 1) || (tdigNodeID > 0x3f)) { 
    cerr << "tdigNodeID = " << tdigNodeID 
	 << " is an invalid entry.  Use a value between 1 and 0x3f (63) instead.\n";
    return 1;
  }
  
  tcpuNodeID =  strtol(argv[3], (char **)NULL, 0);
  if ((tcpuNodeID < 1) || (tcpuNodeID > 0x3f)) { 
    cerr << "tcpuNodeID = " << tcpuNodeID 
	 << " is an invalid entry.  Use a value between 1 and 0x3f (63) instead.\n";
    return 1;
  }
  
  if (argc == 6) {
    devID = atoi(argv[5]);
    if (devID > 7) {
      printf("Invalid Device ID 0x%x. Use a device ID between 0 and 7\n", devID);
      return -1;
    }
  }
  
  return x_readConfig(argv[4], tdigNodeID, tcpuNodeID, TDC, devID);
}
