/* File name     : setThreshold.cc
 * Creation date : 8/17/04
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: setThreshold.cc,v 1.1 2004-08-20 20:44:02 jschamba Exp $";
#endif /* lint */

//#define LOCAL_DEBUG

//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
using namespace std;

// other headers
#include <errno.h>
#include <unistd.h>
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
  cout << "setThreshold: finished (" << error << ")\n";
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
int setThreshold(unsigned int nodeID, int mV)
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

  double D; // the result of the equation
  int decVal; // the decimal value D gets truncated to

  cout << "Setting discriminator threshold at nodeID 0x"
       << hex << nodeID 
       << " to " << dec << mV << " mV ...\n";

  errno = 0;
  
  // install signal handler for manual break
  signal(SIGINT, signal_handler);
  

  D = -20.003*mV + 2030.5;
  decVal = (int)D;
#ifdef LOCAL_DEBUG
  cout << " D = " << D << ", decVal = " << dec << decVal 
       << " (0x" << hex << decVal << ")\n";
#endif

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
    perror("setThreshold: LINUX_CAN_Open()");
    my_private_exit(errno);
  }

    
  // get version info
  errno = CAN_VersionInfo(h, txt);
  if (!errno) {
#ifdef LOCAL_DEBUG
    cout << "setThreshold: driver version = "<< txt << endl;
#endif
  }
  else {
    perror("setThreshold: CAN_VersionInfo()");
    my_private_exit(errno);
  }
  
  // init to a user defined bit rate
  if (wBTR0BTR1) {
    errno = CAN_Init(h, wBTR0BTR1, nExtended);
    if (errno) {
      perror("setThreshold: CAN_Init()");
      my_private_exit(errno);
    }
  }
  
  cout << "Starting SET THRESHOLD Procedure....\n";

  TPCANMsg m;
  __u32 status;
    
  struct pollfd pfd;
  pfd.fd = pcd->nFileNo;
  pfd.events = POLLIN;
    
  m.MSGTYPE = 0;
  m.ID = 0x600 | nodeID;
  m.LEN = 2;
  m.DATA[0] = (decVal>>8) & 0x0F;
  m.DATA[1] = decVal & 0xFF;

#ifdef LOCAL_DEBUG
  printf("setThreshold: message assembled: %c %c 0x%08x %1d  ", 
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
    perror("setThreshold: CAN_Write()");
    my_private_exit(errno);
  }
  

  errno = poll(&pfd, 1, 1000); // timeout = 1 seconds
    //printf("poll returned 0x%x\n", errno);
    //printf("revents = 0x%x\n", pfd.revents);
    
  if (errno == 1) { // data received
    if ((errno = CAN_Read(h, &m))) {
      perror("setThreshold: CAN_Read()");
      my_private_exit(errno);
    }
    else { // data read
#ifdef LOCAL_DEBUG
      printf("setThreshold: message received: %c %c 0x%08x %1d  ", 
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
	  perror("setThreshold: CAN_Status()");
	  my_private_exit(errno);
	}
	else
	  printf("setThreshold: pending CAN status 0x%04x read.\n", (__u16)status);
      } 
      else if (m.MSGTYPE == MSGTYPE_STANDARD) {
	// now interprete the received message:
	int ADC, expectedADC, actualVoltage;
 
	if (m.ID != (0x580 | nodeID)) {
	  cout << "SET THRESHOLD request: Got something other than SET THRESHOLD response: ID 0x" 
	       << hex << m.ID << ", expected 0x" << (0x580 | nodeID) << endl;
	  my_private_exit(errno);
	}
	ADC = (m.DATA[0]<<8) | m.DATA[1];
	expectedADC = (int)(0.1606*(double)decVal + 1.5168);
	actualVoltage = (int)(-0.3113*(double)ADC + 101.97);
#ifdef LOCAL_DEBUG
	cout << "Actual ADC word  : 0x" << hex << ADC << " (" << dec << ADC << ")\n";
	cout << "Expected ADC word: 0x" << hex << expectedADC << " (" 
	     << dec << expectedADC << ")\n";
#endif
	cout << "\nThreshold voltage set to: " << dec << actualVoltage << " mV" << endl;

	// change this to increase/decrease sensitivity to differences between requested voltage
	// and voltage actually set (which can be determined by the returned ADC word)
	int allowedDiff = 15; // 15 ADC points corresponds to 0.3 * 15 = 4.5 mv variation
	if (allowedDiff < abs((expectedADC-ADC))) {
#ifdef LOCAL_DEBUG
	  cout << "Test failed: allowedDiff < abs((expectedADC - ADC)) = "
	       << allowedDiff <<" < abs((" << expectedADC << " - "
	       << ADC << ")) = false\n";
#endif
	  cout << "ERROR: SET THRESHOLD response indicates that voltage set ("
	       << actualVoltage 
	       << " mV) is not within acceptable range of requested value.\n"; 
	}	  
	
	cout << "\n... Threshold setting finished successfully.\n";      
      }
      
    } // data read
  } // data received
  else {	
    cout << "ERROR: Sent SET THRESHOLD request, but did not receive response within 1 sec" << endl;
  }

  my_private_exit(errno);
  
  return errno;
}


int main(int argc, char *argv[])
{
  int mV;
  unsigned int nodeID;
  
#ifdef LOCAL_DEBUG
  cout << "setThreshold procedure, last modified August 18, 2004\n";
#endif

  if (argc != 3) {
    cout << "ERROR: USAGE: " << argv[0] << " <nodeID> <threshold voltage (mV)>\n";
    return 1;
  }
  
  nodeID = atoi(argv[1]);
  mV = atoi(argv[2]);
  if ((mV < -100) || (mV > 100)) {
    cout << "ERROR: Input voltage " << mV << " is out of range."
	 << " Voltage must be between -100 mV and 100 mV.\n";
    return 1;
  }
  
  return setThreshold(nodeID, mV);
}
