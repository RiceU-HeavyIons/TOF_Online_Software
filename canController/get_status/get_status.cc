/* File name     : get_status.cc
 * Creation date : 8/17/04
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: get_status.cc,v 1.1 2004-08-20 20:44:02 jschamba Exp $";
#endif /* lint */

//#define LOCAL_DEBUG

//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
using namespace std;

// other headers
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <libpcan.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>

//****************************************************************************
// DEFINES

//****************************************************************************
// GLOBALS
HANDLE h = NULL;

#define LOCAL_STRING_LEN 64       // length of internal used strings
typedef struct
{
  char szVersionString[LOCAL_STRING_LEN];
  char szDevicePath[LOCAL_STRING_LEN];
  int  nFileNo;
} PCAN_DESCRIPTOR;


//****************************************************************************
// LOCALS

//****************************************************************************
// CODE 

// centralized entry point for all exits
static void my_private_exit(int error)
{
  if (h)
  {
    CAN_Close(h); 
  }
#ifdef LOCAL_DEBUG
  cout << "get_status: finished (" << error << ")\n";
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
int get_status(int TDC, unsigned int nodeID)
{
#ifdef LOCAL_DEBUG
  int i;
#endif
  //__u32 dwPort = 0;
  //__u16 wIrq = 0;
  //__u16 wBTR0BTR1 = CAN_BAUD_250K;
  __u16 wBTR0BTR1 = CAN_BAUD_1M;
  int   nExtended = CAN_INIT_TYPE_ST;
  char txt[255]; // temporary string storage

  cout << "Getting status of TDC " << dec << TDC << " at node 0x" 
       << hex << nodeID << endl;

  errno = 0;
  
  // install signal handler for manual break
  signal(SIGINT, signal_handler);
  
  // open the CAN port
  // please use what is appropriate  
  // HW_DONGLE_SJA 
  // HW_DONGLE_SJA_EPP 
  // HW_ISA_SJA 
  // HW_PCI 
  // HW_USB  -- this is the one we are using

  //h = CAN_Open(HW_USB, dwPort, wIrq);
  h = LINUX_CAN_Open("/dev/pcan32", O_RDWR|O_NONBLOCK);
  
  PCAN_DESCRIPTOR *pcd = (PCAN_DESCRIPTOR *)h;
  
  if (!h) {
    errno = nGetLastError();
    perror("get_status: LINUX_CAN_Open()");
    my_private_exit(errno);
  }

    
  // get version info
  errno = CAN_VersionInfo(h, txt);
  if (!errno) {
#ifdef LOCAL_DEBUG
    cout << "get_status: driver version = "<< txt << endl;
#endif
  }
  else {
    perror("get_status: CAN_VersionInfo()");
    my_private_exit(errno);
  }
  
  // init to a user defined bit rate
  if (wBTR0BTR1) {
    errno = CAN_Init(h, wBTR0BTR1, nExtended);
    if (errno) {
      perror("get_status: CAN_Init()");
      my_private_exit(errno);
    }
  }
  
  int TDCVal = (TDC-1)<<4;
  TPCANMsg m;
  __u32 status;
    
  struct pollfd pfd;
  pfd.fd = pcd->nFileNo;
  pfd.events = POLLIN;
    
  m.MSGTYPE = 0;
  m.ID = 0x300 | TDCVal | nodeID;
  m.LEN = 0;

#ifdef LOCAL_DEBUG
  printf("get_status: message assembled: %c %c 0x%08x %1d  ", 
	 (m.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
	 (m.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
	 m.ID, 
	 m.LEN); 
  for (i = 0; i < m.LEN; i++)
    printf("0x%02x ", m.DATA[i]);
  printf("\n");
#endif
  
  // send the message
  if ( (errno = CAN_Write(h, &m)) ) {
    perror("get_status: CAN_Write()");
    my_private_exit(errno);
  }
  

  errno = poll(&pfd, 1, 1000); // timeout = 1 seconds
    //printf("poll returned 0x%x\n", errno);
    //printf("revents = 0x%x\n", pfd.revents);
    
  if (errno == 1) { // data received
    if ((errno = CAN_Read(h, &m))) {
      perror("get_status: CAN_Read()");
      my_private_exit(errno);
    }
    else { // data read
#ifdef LOCAL_DEBUG
      printf("get_status: message received: %c %c 0x%08x %1d  ", 
	     (m.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
	     (m.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
	     m.ID, 
	     m.LEN); 

      for (i = 0; i < m.LEN; i++)
	printf("0x%02x ", m.DATA[i]);
      printf("\n");
#endif
      
      // check if a CAN status is pending	     
      if (m.MSGTYPE & MSGTYPE_STATUS) {
	status = CAN_Status(h);
	if ((int)status < 0) {
	  errno = nGetLastError();
	  perror("get_status: CAN_Status()");
	  my_private_exit(errno);
	}
	else
	  printf("get_status: pending CAN status 0x%04x read.\n", (__u16)status);
      } 
      else if (m.MSGTYPE == MSGTYPE_STANDARD) {
	// now interprete the status bits:
	cout << "\nDLL lock = "         << ((m.DATA[0] & 0x10) == 0x10) << endl; // bit 60
	cout << "trigger_fifo_empty = " << ((m.DATA[0] & 0x08) == 0x08) << endl; // bit 59
	cout << "trigger_fifo_full = "  << ((m.DATA[0] & 0x04) == 0x04) << endl; // bit 58
	cout << "trigger_fifo_occupancy = "
	     << ((m.DATA[0] & 0x02) == 0x02) // bit 57
	     << ((m.DATA[0] & 0x01) == 0x01) // bit 56
	     << ((m.DATA[1] & 0x80) == 0x80) // bit 55
	     << ((m.DATA[1] & 0x40) == 0x40) // bit 54
	     << " ("
	     << dec << ((m.DATA[0]&0x03)<<2 + (m.DATA[1]&0xc0)>>6)
	     << " words)\n";
	cout << "group0_L1_occupancy = "
	     << ((m.DATA[1] & 0x20) == 0x20) // bit 53
	     << ((m.DATA[1] & 0x10) == 0x10) // bit 52
	     << ((m.DATA[1] & 0x08) == 0x08) // bit 51
	     << ((m.DATA[1] & 0x04) == 0x04) // bit 50
	     << ((m.DATA[1] & 0x02) == 0x02) // bit 49
	     << ((m.DATA[1] & 0x01) == 0x01) // bit 48
	     << ((m.DATA[2] & 0x80) == 0x80) // bit 47
	     << ((m.DATA[2] & 0x40) == 0x40) // bit 46
	     << " ("
	     << dec << ((m.DATA[1]&0x3f)<<2) + ((m.DATA[2]&0xc0)>>6)
	     << " words)\n";
	cout << "group1_L1_occupancy = "
	     << ((m.DATA[2] & 0x20) == 0x20) // bit 45
	     << ((m.DATA[2] & 0x10) == 0x10) // bit 44
	     << ((m.DATA[2] & 0x08) == 0x08) // bit 43
	     << ((m.DATA[2] & 0x04) == 0x04) // bit 42
	     << ((m.DATA[2] & 0x02) == 0x02) // bit 41
	     << ((m.DATA[2] & 0x01) == 0x01) // bit 40
	     << ((m.DATA[3] & 0x80) == 0x80) // bit 39
	     << ((m.DATA[3] & 0x40) == 0x40) // bit 38
	     << " ("
	     << dec << ((m.DATA[2]&0x3f)<<2) + ((m.DATA[3]&0xc0)>>6)
	     << " words)\n";
	cout << "group2_L1_occupancy = "
	     << ((m.DATA[3] & 0x20) == 0x20) // bit 37
	     << ((m.DATA[3] & 0x10) == 0x10) // bit 36
	     << ((m.DATA[3] & 0x08) == 0x08) // bit 35
	     << ((m.DATA[3] & 0x04) == 0x04) // bit 34
	     << ((m.DATA[3] & 0x02) == 0x02) // bit 33
	     << ((m.DATA[3] & 0x01) == 0x01) // bit 32
	     << ((m.DATA[4] & 0x80) == 0x80) // bit 31
	     << ((m.DATA[4] & 0x40) == 0x40) // bit 30
	     << " ("
	     << dec << ((m.DATA[3]&0x3f)<<2) + ((m.DATA[4]&0xc0)>>6)
	     << " words)\n";
	cout << "group3_L1_occupancy = "
	     << ((m.DATA[4] & 0x20) == 0x20) // bit 29
	     << ((m.DATA[4] & 0x10) == 0x10) // bit 28
	     << ((m.DATA[4] & 0x08) == 0x08) // bit 27
	     << ((m.DATA[4] & 0x04) == 0x04) // bit 26
	     << ((m.DATA[4] & 0x02) == 0x02) // bit 25
	     << ((m.DATA[4] & 0x01) == 0x01) // bit 24
	     << ((m.DATA[5] & 0x80) == 0x80) // bit 23
	     << ((m.DATA[5] & 0x40) == 0x40) // bit 22
	     << " ("
	     << dec << ((m.DATA[4]&0x3f)<<2) + ((m.DATA[5]&0xc0)>>6)
	     << " words)\n";
	cout << "readout_fifo_empty = " << ((m.DATA[5] & 0x20) == 0x20) << endl; // bit 21
	cout << "readout_fifo_full = "  << ((m.DATA[5] & 0x10) == 0x10) << endl; // bit 20
	cout << "readout_fifo_occupancy = "
	     << ((m.DATA[5] & 0x08) == 0x08) // bit 19
	     << ((m.DATA[5] & 0x04) == 0x04) // bit 18
	     << ((m.DATA[5] & 0x02) == 0x02) // bit 17
	     << ((m.DATA[5] & 0x01) == 0x01) // bit 16
	     << ((m.DATA[6] & 0x80) == 0x80) // bit 15
	     << ((m.DATA[6] & 0x40) == 0x40) // bit 14
	     << ((m.DATA[6] & 0x20) == 0x20) // bit 13
	     << ((m.DATA[6] & 0x10) == 0x10) // bit 12
	     << " ("
	     << dec << ((m.DATA[5]&0x0f)<<4) + ((m.DATA[6]&0xf0)>>4)
	     << " words)\n";
	cout << "have_token = " << ((m.DATA[6] & 0x08) == 0x08) << endl; // bit 11
	cout << "Status (Error) [10..0] = "
	     << ((m.DATA[6] & 0x04) == 0x04) // bit 10
	     << ((m.DATA[6] & 0x02) == 0x02) // bit 9
	     << ((m.DATA[6] & 0x01) == 0x01) // bit 8
	     << ((m.DATA[7] & 0x80) == 0x80) // bit 7
	     << ((m.DATA[7] & 0x40) == 0x40) // bit 6
	     << ((m.DATA[7] & 0x20) == 0x20) // bit 5
	     << ((m.DATA[7] & 0x10) == 0x10) // bit 4
	     << ((m.DATA[7] & 0x08) == 0x08) // bit 3
	     << ((m.DATA[7] & 0x04) == 0x04) // bit 2
	     << ((m.DATA[7] & 0x02) == 0x02) // bit 1
	     << ((m.DATA[7] & 0x01) == 0x01) // bit 0
	     << endl;

	if ( (m.DATA[6] & 0x04) == 0x04 ) cout << "JTAG INSTRUCTION ERROR \n";  	// bit 10
	if ( (m.DATA[6] & 0x02) == 0x02 ) cout << "CONTROL PARITY ERROR \n";  		// bit 9
	if ( (m.DATA[6] & 0x01) == 0x01 ) cout << "SETUP PARITY ERROR \n";  		// bit 8
	if ( (m.DATA[7] & 0x80) == 0x80 ) cout << "READOUT STATE ERROR \n";  		// bit 7
	if ( (m.DATA[7] & 0x40) == 0x40 ) cout << "READOUT FIFO PARITY ERROR \n";  	// bit 6
	if ( (m.DATA[7] & 0x20) == 0x20 ) cout << "TRIGGER MATCHING STATE ERROR \n";	// bit 5
	if ( (m.DATA[7] & 0x10) == 0x10 ) cout << "TRIGGER FIFO PARITY ERROR \n";  	// bit 4
	if ( (m.DATA[7] & 0x08) == 0x08 ) cout << "L1 BUFFER PARITY ERROR \n";  	// bit 3
	if ( (m.DATA[7] & 0x04) == 0x04 ) cout << "CHANNEL SELECT ERROR \n";  		// bit 2
	if ( (m.DATA[7] & 0x02) == 0x02 ) cout << "COURSE ERROR \n";  			// bit 1
	if ( (m.DATA[7] & 0x01) == 0x01 ) cout << "VERNIER ERROR \n";  			// bit 0

	cout << "\n... Status-fetching finished successfully.\n";      
      }
      
    } // data read
  } // data received
  else {	
    cout << "ERROR: Sent GET_STATUS request, but did not receive response within 1 sec" << endl;
  }

  my_private_exit(errno);
  
  return errno;
}


int main(int argc, char *argv[])
{
  unsigned int nodeID;

  cout << "get_status procedure, last modified August 18, 2004" << endl;

  if ( argc != 3 ) {
    cerr << "USAGE: " << argv[0] << "  <TDC_Number> <nodeID>\n";
    return -1;
  }
  
  // convert _TCHAR TDC_Number to an int
  int TDC = atoi(argv[1]);
  
  if ((TDC < 1) || (TDC > 4)) { 
    cerr << "TDC = " << TDC << " invalid entry. Use 1-4 instead." << endl;
    return -1;
  }
  
  nodeID = atoi(argv[2]);
  
  return get_status(TDC, nodeID);
}
