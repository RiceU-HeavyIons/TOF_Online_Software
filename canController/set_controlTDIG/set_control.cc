/* File name     : set_control.cc
 * Creation date : 8/18/04
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: set_control.cc,v 1.1 2004-08-20 20:44:02 jschamba Exp $";
#endif /* lint */

//#define LOCAL_DEBUG

//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
#include <fstream>
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
#define TDIG

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
  cout << "set_control: finished (" << error << ")\n";
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
int set_control(const char *filename, unsigned int nodeID, int TDC, int resolution)
{
#ifdef LOCAL_DEBUG
  char tempBuff[255];
  int j;
#endif
  //__u32 dwPort = 0;
  //__u16 wIrq = 0;
  //__u16 wBTR0BTR1 = CAN_BAUD_250K;
  int i;
  char txt[255]; // temporary string storage

  bool isHexEntry = false;
  int TDCVal; // value used to encode TDC ID in CAN msg ID (equal to 0, if using a Jalapeno board)

  ifstream ctrl;
  unsigned char ctrlByte[5];
  unsigned int tempval;
  char buffer[255];

  cout << "Setting control word in TDC " << TDC
       << " (" << (resolution ? "high" : "low") 
       << " resolution) at NodeID 0x" << hex << nodeID
       << ", with filename " << filename << "...\n\n";

  errno = 0;
  
  // install signal handler for manual break
  signal(SIGINT, signal_handler);
  
  // open file "filename" and extract control bits
  ctrl.open(filename); // "in" is default 
  if ( !ctrl.good() ) {
    cerr << filename << ": file error\n";
    return -1;
  }
  
  ctrl.seekg(0,ios::beg);    //move file pointer to beginning of file
  ctrl.getline(buffer, 80);
  // check if lines contain hex values (with the letter "x" in it)
  if( strchr(buffer, 'x') != NULL) isHexEntry = true;
  ctrl.seekg(0,ios::beg);    //move file pointer to beginning of file again
  
  ctrl >> hex;
  
  // clear buffer
  buffer[0]='\0';
  for (i=0; (i<5 && ctrl.good()); i++) { 
    ctrl >> tempval;
    if ( !ctrl.good() ) {
      cout << "ERROR: " << filename << ": error reading value for control byte " << dec << i << endl;
      ctrl.close();
      return -1;
    }
    
    if (isHexEntry)
      ctrlByte[i] = (unsigned char)tempval;
    else
      ctrlByte[i] =	(tempval & 0x00000001) |
	((tempval & 0x00000010)>>3) |
	((tempval & 0x00000100)>>6) |
	((tempval & 0x00001000)>>9) |
	((tempval & 0x00010000)>>12) |
	((tempval & 0x00100000)>>15) |
	((tempval & 0x01000000)>>18) |
	((tempval & 0x10000000)>>21);
#ifdef LOCAL_DEBUG
    sprintf(tempBuff, "ctrlByte[%d] = 0x%02x \n", i, (unsigned int)ctrlByte[i]);
    strcat(buffer, tempBuff);
#endif
  }

#ifdef LOCAL_DEBUG
  cout << "Control bytes read from file " << filename << ":\n" << buffer;
#endif

  ctrl.close();

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
    perror("set_control: LINUX_CAN_Open()");
    my_private_exit(errno);
  }

    
  // get version info
  errno = CAN_VersionInfo(h, txt);
  if (!errno) {
#ifdef LOCAL_DEBUG
    cout << "set_control: driver version = "<< txt << endl;
#endif
  }
  else {
    perror("set_control: CAN_VersionInfo()");
    my_private_exit(errno);
  }
  
#ifdef TDIG
  // TDCs 1-4 are encoded in the CAN MsgID as follows:
  // MsgID[5:4] = 00  => TDC1
  // MSgID[5:4] = 01  => TDC2
  // MsgID[5:4] = 10  => TDC3
  // MSgID[5:4] = 11  => TDC4
  TDCVal = (TDC-1)<<4;

  // if 'All TDCs' was selected, TDC val passed in will be -1.
  // Check for that and if so, set 'All TDCs' bit high (MsgID[3])
  if (TDCVal < 0) TDCVal = 8;
  
  // open CAN Port for TDIG, init PCAN-USB
  errno = CAN_Init(h, CAN_BAUD_1M,  CAN_INIT_TYPE_ST);
  if (errno) {
    perror("set_control: CAN_Init()");
    my_private_exit(errno);
  } 
  
  const int msgLength = 5;
#else
  // For Jalapeno, TDCs are numbered 1-2 and are not part of the MsgID
  TDCVal = 0;

  // open CAN Port for Jalapeno, init PCAN-USB
  errno = CAN_Init(h, CAN_BAUD_250K,  CAN_INIT_TYPE_ST);
  if (errno) {
    perror("set_control: CAN_Init()");
    my_private_exit(errno);
  } 

  const int msgLength = 6;
#endif

  cout << "Starting SET_CONTROL Procedure....\n";

  TPCANMsg m;
  __u32 status;
    
  struct pollfd pfd;
  pfd.fd = pcd->nFileNo;
  pfd.events = POLLIN;
    
  m.MSGTYPE = CAN_INIT_TYPE_ST;
  m.ID = 0x100 | TDCVal | nodeID;
  m.LEN = msgLength;

#ifdef TDIG
  // payload:
  // cccc cccc cccc cccc cccc cccc cccc cccc cccc cccc
  // [40-bit control word from file]
  for (i=0; i<5; i++) m.DATA[i] = ctrlByte[i];
#else
  
  // payload:
  // tttt tttt cccc cccc cccc cccc cccc cccc cccc cccc cccc cccc
  // [TDC ID]  [40-bit control word from file]
  // 1111 0000 -> TDC1 control word
  // 1111 0011 -> TDC2 control word
  m.DATA[0]	= 0xF0 | ((TDC == 1) ? 0 : 0x3);
  for (i=0; i<5; i++) msgSent.DATA[i+1] = ctrlByte[i];
#endif

#ifdef LOCAL_DEBUG
  cout << "\nSending SET_CONTROL command:\n";
  printf("set_control: message assembled: %c %c 0x%08x %1d  ", 
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
    perror("set_control: CAN_Write()");
    my_private_exit(errno);
  }
  

  errno = poll(&pfd, 1, 1000); // timeout = 1 seconds
    //printf("poll returned 0x%x\n", errno);
    //printf("revents = 0x%x\n", pfd.revents);
    
  if (errno == 1) { // data received
    if ((errno = CAN_Read(h, &m))) {
      perror("set_control: CAN_Read()");
      my_private_exit(errno);
    }
    else { // data read
#ifdef LOCAL_DEBUG
      printf("set_control: message received : %c %c 0x%08x %1d  ", 
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
	  perror("set_control: CAN_Status()");
	  my_private_exit(errno);
	}
	else
	  printf("set_control: pending CAN status 0x%04x read.\n", (__u16)status);
      } 
      else if (m.MSGTYPE == MSGTYPE_STANDARD) {
	// now interprete the received message:
	if (m.ID != (0x080 | TDCVal | nodeID)) { // check if it's a set_control response
	  cout << "SET_CONTROL request: Got smething other than set_control response: ID 0x" 
	       << hex << m.ID 
	       << ", expected 0x" << (0x080 | TDCVal | nodeID) << endl;	
	  my_private_exit(errno);
	}
	if (m.LEN != msgLength) { // check for correct length
	  cout << "ERROR:	SET_CONTROL request: Got msg with incorrect data length " 
	       << dec << m.LEN << ", expected " << msgLength << endl;
	  my_private_exit(errno);
	}
	// Message is okay, so log the data
	unsigned int enableChannel;
	tempval = m.DATA[m.LEN-1];
	enableChannel = tempval >> 5;
	tempval = m.DATA[m.LEN-2];
	enableChannel |= tempval << 3;
	tempval = m.DATA[m.LEN-3];
	enableChannel |= tempval << 11;
	tempval = m.DATA[m.LEN-4];
	enableChannel |= tempval << 19;
	tempval = m.DATA[m.LEN-5] & 0x1f;
	enableChannel |= tempval << 27;


#ifdef LOCAL_DEBUG
#ifdef TDIG
	cout << "\nSET_CONTROL Payload: \n[40-bit control word]\n";
#else
	cout << "\nSET_CONTROL Payload: \n[TDC ID] [40-bit control word]\n";
#endif

	for (i=0; i<m.LEN; i++) {
	  for (j=7; j>=0; j--) {
	    cout << ((m.DATA[i] & (1<<j)) == (1<<j));
	  }
	  cout << " ";
	}
	cout << "\n\n";

	cout << "Control word break-down:\n";
	cout << "Parity = " 	<< ((m.DATA[m.LEN-5] & 0x80) == 0x80) << endl; // bit 39
	cout << "pll_reset = " 	<< ((m.DATA[m.LEN-5] & 0x40) == 0x40) << endl; // bit 38
	cout << "dll_reset = " 	<< ((m.DATA[m.LEN-5] & 0x20) == 0x20) << endl; // bit 37
	for (i=31; i>=0; i--)
	  cout << "enable_channel[" << dec << i << "] = "
	       << ((enableChannel & (1<<i)) == (unsigned int)(1<<i)) << endl;
	cout << "Global_reset = " << (m.DATA[m.LEN-1] & 0x10 == 0x10) << endl; // bit 4
	cout << "enable_pattern = "
	     << ((m.DATA[m.LEN-1] & 0x08) == 0x08) // bit 3
	     << ((m.DATA[m.LEN-1] & 0x04) == 0x04) // bit 2
	     << ((m.DATA[m.LEN-1] & 0x02) == 0x02) // bit 1
	     << ((m.DATA[m.LEN-1] & 0x01) == 0x01) // bit 0
	     << "\n\n";
#endif
	
	// print which channels are enabled
	if (resolution == 1) {
	  for (i=0; i<32; i+=4) {
	    // Make sure that all four bits of hi-res channel are the same
	    if ( ((enableChannel & (0xf<<i)) == (unsigned int)(0xf<<i)) || ((enableChannel & (0xf<<i)) == 0) )
	      cout << "channel " << (i/4)
		   <<  ( ((enableChannel & (1<<i)) == (unsigned int)(1<<i)) ? " enabled\n" : " DISABLED\n" );
	    else {
	      cout << "ERROR: Component bits of hi-res channel are different: \n";
	      cout << "enable_channel[" << dec << i << "] = "
		   << ((enableChannel & (1<<i)) == (unsigned int)(1<<i)) << endl;
	      cout << "enable_channel[" << dec << (i+1) << "] = "
		   << ((enableChannel & (1<<(i+1))) == (unsigned int)(1<<(i+1))) << endl;
	      cout << "enable_channel[" << dec << (i+2) << "] = "
		   << ((enableChannel & (1<<(i+2))) == (unsigned int)(1<<(i+2))) << endl;
	      cout << "enable_channel[" << dec << (i+3) << "] = "
		   << ((enableChannel & (1<<(i+3))) == (unsigned int)(1<<(i+3))) << endl;

	      errno = 1;
	      my_private_exit(errno);
	    }
	  }
	}
	else {
	  for (i=0; i<32; i++) {
	    cout << "channel " << dec << i
		 <<  ( ((enableChannel & (1<<i)) == (unsigned int)(1<<i)) ? " enabled\n" : " DISABLED\n" );
	  }
	}

	cout << "\n... Control-word-fetching finished successfully.\n";
      }
      
    } // data read
  } // data received
  else {	
    cout << "ERROR: Sent SET CONTROL request, but did not receive response within 1 sec" << endl;
  }

  my_private_exit(errno);
  
  return errno;
}


int main(int argc, char *argv[])
{
  unsigned int nodeID;
  int resolution;


#ifdef LOCAL_DEBUG
  cout << "SET_CONTROL procedure, last modified August 18, 2004\n";
#endif

  if ( argc != 5 ) {
    cout << "USAGE: " << argv[0]  
	 << " <TDC ID> <node ID> <file name> <resolution(high=1,low=0)>\n";
    return -1;
  }
  
  // convert _TCHAR TDC_Number to an int
  int TDC = atoi(argv[1]);
  
#ifdef TDIG
  if ((TDC < 1) || (TDC > 4)) { 
    if (TDC != -1) {
      cout << "ERROR: TDC = " << TDC 
	   << " invalid entry. Use 1-4 for a specific TDC or -1 for all TDCs.\n"; 
      return -1;
    }
  }
#else 
  if ((TDC != 1) && (TDC != 2)) { 
    if (TDC != -1) {
      cout << "ERROR: TDC = " << TDC 
	   << " invalid entry. Use 1 or 2 for a specific TDC or -1 for both TDCs.\n"; 
      return -1;
    }
  }
#endif
  
  resolution = atoi(argv[4]);
  if ( (resolution != 0) && (resolution != 1) ) {
    cout << "ERROR: resolution = " << resolution 
	 << " is an invalid entry.  Use 0 for low or 1 for high.\n";
    return -1;
  }
  
  nodeID = (atoi(argv[2]));
  
  return set_control(argv[3], nodeID, TDC, resolution);
}
