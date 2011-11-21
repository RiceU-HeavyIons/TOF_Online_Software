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
#include <libpcan.h>

// local utilities
#include "can_utils.h"

//****************************************************************************
// DEFINES

//#define LOCAL_DEBUG

//****************************************************************************
// GLOBALS
HANDLE h = NULL;

//****************************************************************************
// LOCALS

//****************************************************************************
// CODE 

// centralized entry point for all exits
static void my_private_exit(int error)
{
  if (h)
  {
    
#ifdef LOCAL_DEBUG
  cout << "x_readHPTDCconfig_l: closing h\n";
#endif
    CAN_Close_l(h); 
  }
#ifdef LOCAL_DEBUG
  cout << "x_readHPTDCconfig_l: finished (" << error << ")\n";
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
int x_readConfig_l(const char *filename, unsigned int tdigNodeID, unsigned int tcpuNodeID, int TDC, WORD devID)
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

  if((errno = openCAN_l(devID)) != 0) {
    my_private_exit(errno);
  }

  conf.open(filename, ios_base::out | ios_base::trunc);

  int nodeIDVal = (((tdigNodeID << 4) | 0x004) << 18) | tcpuNodeID;

  TPCANRdMsg mr[12];
  TPCANMsg ms;
  ms.MSGTYPE = MSGTYPE_EXTENDED;
  ms.ID = nodeIDVal;
  ms.LEN = 1;
  ms.DATA[0] = 0x40+TDC;

#ifdef LOCAL_DEBUG
  printCANMsg(ms, "x_readHPTDCconfig_l: Sending RS_CONFIGTDCx command");
#endif

  if ( CAN_Write_l(h, &ms, devID) != 0)
    my_private_exit(errno);

  for (i=0; i<12; i++)
    LINUX_CAN_Read_Timeout_l(h, &mr[i], 4000000); // timeout = 4sec

#ifdef LOCAL_DEBUG
  for (i=0; i<12; i++)
    printCANMsg(mr[i].Msg, "message received");
#endif

  for (j=0; j<12; j++) {
    for (k=1; k<mr[j].Msg.LEN; k++) {
      for (i=7; i>=0; i--)
	conf << ((mr[j].Msg.DATA[k] >> i)&1);
      conf << endl;
    }
  }


  conf.close();

  my_private_exit(errno);
  
  return errno;
}


int main(int argc, char *argv[])
{
  unsigned int tdigNodeID, tcpuNodeID;
  WORD devID = 255;

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
    if (devID > 255) {
      printf("Invalid Device ID 0x%x. Use a device ID between 0 and 255\n", devID);
      return -1;
    }
  }
  
  return x_readConfig_l(argv[4], tdigNodeID, tcpuNodeID, TDC, devID);
}
