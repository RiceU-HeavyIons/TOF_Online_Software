/* File name     : p_config.cc
 * Creation date : 8/18/04
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: p_config.cc,v 1.2 2004-09-03 14:41:31 jschamba Exp $";
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
  cout << "p_config: finished (" << error << ")\n";
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
int p_config(const char *filename, unsigned int nodeID, int TDC)
{
  //__u32 dwPort = 0;
  //__u16 wIrq = 0;
  //__u16 wBTR0BTR1 = CAN_BAUD_250K;
  int i,j;
  char txt[255]; // temporary string storage
  unsigned char sendData;

  bool isHexEntry = false;
  int TDCVal; // value used to encode TDC ID in CAN msg ID (equal to 0, if using a Jalapeno board)

  ifstream conf;
  unsigned char confByte[81];
  unsigned int tempval;
  char buffer[255];

  cout << "Configuring TDC " << TDC
       << " at NodeID 0x" << hex << nodeID
       << " with filename " << filename << "...\n";

  errno = 0;
  
  // install signal handler for manual break
  signal(SIGINT, signal_handler);
  
  // open file "filename" and extract control bits
  conf.open(filename); // "in" is default 
  if ( !conf.good() ) {
    cerr << filename << ": file error\n";
    return -1;
  }
  
  conf.seekg(0,ios::beg);    //move file pointer to beginning of file
  conf.getline(buffer, 80);
  // check if lines contain hex values (with the letter "x" in it)
  if( strchr(buffer, 'x') != NULL) isHexEntry = true;
  conf.seekg(0,ios::beg);    //move file pointer to beginning of file again
  
  conf >> hex;
  
  // clear buffer
  buffer[0]='\0';
  for (i=0; (i<81 && conf.good()); i++) { 
    conf >> tempval;
    if ( !conf.good() ) {
      cout << "ERROR: " << filename << ": error reading value for control byte " << dec << i << endl;
      conf.close();
      return -1;
    }
    
    if (isHexEntry)
      confByte[i] = (unsigned char)tempval;
    else
      confByte[i] =	(tempval & 0x00000001) |
	((tempval & 0x00000010)>>3) |
	((tempval & 0x00000100)>>6) |
	((tempval & 0x00001000)>>9) |
	((tempval & 0x00010000)>>12) |
	((tempval & 0x00100000)>>15) |
	((tempval & 0x01000000)>>18) |
	((tempval & 0x10000000)>>21);
  }

  conf.close();
  
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
    perror("p_config: LINUX_CAN_Open()");
    my_private_exit(errno);
  }

    
  // get version info
  errno = CAN_VersionInfo(h, txt);
  if (!errno) {
#ifdef LOCAL_DEBUG
    cout << "p_config: driver version = "<< txt << endl;
#endif
  }
  else {
    perror("p_config: CAN_VersionInfo()");
    my_private_exit(errno);
  }
  
  // TDCs 1-4 are encoded in the CAN MsgID as follows:
  // MsgID[5:4] = 00  => TDC1
  // MSgID[5:4] = 01  => TDC2
  // MsgID[5:4] = 10  => TDC3
  // MSgID[5:4] = 11  => TDC4
  TDCVal = (TDC-1)<<4;

  // open CAN Port for TDIG, init PCAN-USB
  errno = CAN_Init(h, CAN_BAUD_1M,  CAN_INIT_TYPE_ST);
  if (errno) {
    perror("p_config: CAN_Init()");
    my_private_exit(errno);
  } 
  
  cout << "Starting Configuration Procedure....\n";

  TPCANMsg m, ms;
  __u32 status;
    
  struct pollfd pfd;
  pfd.fd = pcd->nFileNo;
  pfd.events = POLLIN;
    
  ms.MSGTYPE = CAN_INIT_TYPE_ST;
  ms.ID = 0x200 | TDCVal | nodeID;
  ms.LEN = 1;

  // "CONFIGURE_TDC:Start"
  sendData = ms.DATA[0] = 0;

#ifdef LOCAL_DEBUG
  cout << "\nSending START command:\n";
  printf("p_config: message assembled: %c %c 0x%08x %1d  ", 
	 (ms.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
	 (ms.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
	 ms.ID, 
	 ms.LEN); 
  for (i = 0; i < ms.LEN; i++)
    printf("0x%02x ", ms.DATA[i]);
  printf("\n");
#endif

  
  // send the message
  if ( (errno = CAN_Write(h, &ms)) ) {
    perror("p_config: CAN_Write()");
    my_private_exit(errno);
  }
  

  errno = poll(&pfd, 1, 1000); // timeout = 1 seconds
    //printf("poll returned 0x%x\n", errno);
    //printf("revents = 0x%x\n", pfd.revents);
    
  if (errno == 1) { // data received
    if ((errno = CAN_Read(h, &m))) {
      perror("p_config: CAN_Read()");
      my_private_exit(errno);
    }
    else { // data read
#ifdef LOCAL_DEBUG
      printf("p_config: message received : %c %c 0x%08x %1d  ", 
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
	  perror("p_config: CAN_Status()");
	  my_private_exit(errno);
	}
	else
	  printf("p_config: pending CAN status 0x%04x read.\n", (__u16)status);
      } 
      else if (m.MSGTYPE == MSGTYPE_STANDARD) {
	// now interprete the received message:
	if (m.ID != (0x180 | TDCVal | nodeID)) { // check if it's a p_config response
	  cout << "p_config request: Got smething other than p_config response: ID 0x" 
	       << hex << m.ID 
	       << ", expected 0x" << (0x180 | TDCVal | nodeID) << endl;	
	  my_private_exit(errno);
	}
	if (m.LEN != 1) { // check for correct length
	  cout << "ERROR:	p_config request: Got msg with incorrect data length" 
	       << dec << m.LEN << ", expected 1\n";
	  my_private_exit(errno);
	}
	if (m.DATA[0] != sendData) {
	  cout << "CONFIGURE TDC Start response: first byte: 0x" 
	       << hex << m.DATA[0] 
	       << " expected 0x" << sendData << endl;
	  my_private_exit(errno);
	}
	// Message is good, continue
#ifdef LOCAL_DEBUG
	cout << "Received Start response:\n";
#endif

      }
      
    } // data read
  } // data received
  else {	
    cout << "ERROR: Sent TDC_CONFIGURE:Start packet, but did not receive response within 1 sec" << endl;
    my_private_exit(errno);
  }


  // "CONFIGURE_TDC:Data", 11 messages with 7 bytes each
  ms.LEN = 8;

  for (i=1; i<12; i++) {
    sendData = ms.DATA[0] = 0x40 | i;
    for (j=0; j<7; j++) ms.DATA[j+1] = confByte[(i-1)*7+j];
    
#ifdef LOCAL_DEBUG
    cout << "\nSending DATA packet:\n";
    printf("p_config: message assembled: %c %c 0x%08x %1d  ", 
	   (ms.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
	   (ms.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
	   ms.ID, 
	   ms.LEN); 
    for (j = 0; j < ms.LEN; j++)
      printf("0x%02x ", ms.DATA[j]);
    printf("\n");
#endif
    
    
    // send the message
    if ( (errno = CAN_Write(h, &ms)) ) {
      perror("p_config: CAN_Write()");
      my_private_exit(errno);
    }
    
    
    errno = poll(&pfd, 1, 1000); // timeout = 1 seconds
    //printf("poll returned 0x%x\n", errno);
    //printf("revents = 0x%x\n", pfd.revents);
    
    if (errno == 1) { // data received
      if ((errno = CAN_Read(h, &m))) {
	perror("p_config: CAN_Read()");
	my_private_exit(errno);
      }
      else { // data read
#ifdef LOCAL_DEBUG
	printf("p_config: message received : %c %c 0x%08x %1d  ", 
	       (m.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
	       (m.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
	       m.ID, 
	       m.LEN); 
	
	for (j = 0; j < m.LEN; j++)
	  printf("0x%02x ", m.DATA[j]);
	printf("\n");
#endif
	
	// check if a CAN status is pending	     
	if (m.MSGTYPE & MSGTYPE_STATUS) {
	  status = CAN_Status(h);
	  if ((int)status < 0) {
	    errno = nGetLastError();
	    perror("p_config: CAN_Status()");
	    my_private_exit(errno);
	  }
	  else
	    printf("p_config: pending CAN status 0x%04x read.\n", (__u16)status);
	} 
	else if (m.MSGTYPE == MSGTYPE_STANDARD) {
	  // now interprete the received message:
	  if (m.ID != (0x180 | TDCVal | nodeID)) { // check if it's a p_config response
	    cout << "CONFIGURE_TDC:Data: Got smething other than p_config response: ID 0x" 
		 << hex << m.ID 
		 << ", expected 0x" << (0x180 | TDCVal | nodeID) << endl;	
	    my_private_exit(errno);
	  }
	  if (m.LEN != 8) { // check for correct length
	    cout << "ERROR: CONFIGURE_TDC:Data: Got msg with incorrect data length" 
		 << dec << m.LEN << ", expected 1\n";
	    my_private_exit(errno);
	  }
	  if (m.DATA[0] != sendData) {
	    cout << "CONFIGURE_TDC Data response: first byte: 0x" 
		 << hex << m.DATA[0] 
		 << " expected 0x" << sendData << endl;
	    my_private_exit(errno);
	  }
	  // Message is good, continue
#ifdef LOCAL_DEBUG
	  cout << "Received Data response:\n";
#endif
	  
	}
	
      } // data read
    } // data received
    else {	
      cout << "ERROR: Sent TDC_CONFIGURE:Data packet, but did not receive response within 1 sec" << endl;
      my_private_exit(errno);
    }
  } // end "for(i=1, ... " loop


  // "CONFIGURE_TDC:Data", ... and 1 last message with 4 bytes
  ms.LEN = 5;

  sendData = ms.DATA[0] = 0x4c;
  for (j=0; j<4; j++) ms.DATA[j+1] = confByte[77+j];
  
#ifdef LOCAL_DEBUG
  cout << "\nSending DATA packet 12:\n";
  printf("p_config: message assembled: %c %c 0x%08x %1d  ", 
	 (ms.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
	 (ms.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
	 ms.ID, 
	 ms.LEN); 
  for (j = 0; j < ms.LEN; j++)
    printf("0x%02x ", ms.DATA[j]);
  printf("\n");
#endif
  
  
  // send the message
  if ( (errno = CAN_Write(h, &ms)) ) {
    perror("p_config: CAN_Write()");
    my_private_exit(errno);
  }
  
  
  errno = poll(&pfd, 1, 1000); // timeout = 1 seconds
  //printf("poll returned 0x%x\n", errno);
  //printf("revents = 0x%x\n", pfd.revents);
  
  if (errno == 1) { // data received
    if ((errno = CAN_Read(h, &m))) {
      perror("p_config: CAN_Read()");
      my_private_exit(errno);
    }
    else { // data read
#ifdef LOCAL_DEBUG
      printf("p_config: message received : %c %c 0x%08x %1d  ", 
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
	  perror("p_config: CAN_Status()");
	  my_private_exit(errno);
	}
	else
	  printf("p_config: pending CAN status 0x%04x read.\n", (__u16)status);
      } 
      else if (m.MSGTYPE == MSGTYPE_STANDARD) {
	// now interprete the received message:
	if (m.ID != (0x180 | TDCVal | nodeID)) { // check if it's a p_config response
	  cout << "CONFIGURE_TDC:Data: Got smething other than p_config response: ID 0x" 
	       << hex << m.ID 
	       << ", expected 0x" << (0x180 | TDCVal | nodeID) << endl;	
	  my_private_exit(errno);
	}
	if (m.LEN != 5) { // check for correct length
	  cout << "ERROR: CONFIGURE_TDC:Data: Got msg with incorrect data length" 
	       << dec << m.LEN << ", expected 1\n";
	  my_private_exit(errno);
	}
	if (m.DATA[0] != sendData) {
	  cout << "CONFIGURE_TDC Data response: first byte: 0x" 
	       << hex << m.DATA[0] 
	       << " expected 0x" << sendData << endl;
	  my_private_exit(errno);
	}
	// Message is good, continue
#ifdef LOCAL_DEBUG
	cout << "Received Data response:\n";
#endif
	
      }
      
    } // data read
  } // data received
  else {	
    cout << "ERROR: Sent TDC_CONFIGURE:Data packet, but did not receive response within 1 sec" << endl;
    my_private_exit(errno);
  }

  // "CONFIGURE_TDC:Config_end"
  ms.LEN = 1;
  ms.DATA[0] = sendData = 0x80;
  
#ifdef LOCAL_DEBUG
  cout << "\nSending Config_end command:\n";
  printf("p_config: message assembled: %c %c 0x%08x %1d  ", 
	 (ms.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
	 (ms.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
	 ms.ID, 
	 ms.LEN); 
  for (j = 0; j < ms.LEN; j++)
    printf("0x%02x ", ms.DATA[j]);
  printf("\n");
#endif
  
  
  // send the message
  if ( (errno = CAN_Write(h, &ms)) ) {
    perror("p_config: CAN_Write()");
    my_private_exit(errno);
  }
  
  
  errno = poll(&pfd, 1, 1000); // timeout = 1 seconds
  //printf("poll returned 0x%x\n", errno);
  //printf("revents = 0x%x\n", pfd.revents);
  
  if (errno == 1) { // data received
    if ((errno = CAN_Read(h, &m))) {
      perror("p_config: CAN_Read()");
      my_private_exit(errno);
    }
    else { // data read
#ifdef LOCAL_DEBUG
      printf("p_config: message received : %c %c 0x%08x %1d  ", 
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
	  perror("p_config: CAN_Status()");
	  my_private_exit(errno);
	}
	else
	  printf("p_config: pending CAN status 0x%04x read.\n", (__u16)status);
      } 
      else if (m.MSGTYPE == MSGTYPE_STANDARD) {
	// now interprete the received message:
	if (m.ID != (0x180 | TDCVal | nodeID)) { // check if it's a p_config response
	  cout << "CONFIGURE_TDC:Config_end: Got smething other than p_config response: ID 0x" 
	       << hex << m.ID 
	       << ", expected 0x" << (0x180 | TDCVal | nodeID) << endl;	
	  my_private_exit(errno);
	}
	if (m.LEN != 1) { // check for correct length
	  cout << "ERROR: CONFIGURE_TDC:Config_end: Got msg with incorrect data length" 
	       << dec << m.LEN << ", expected 1\n";
	  my_private_exit(errno);
	}
	if (m.DATA[0] != sendData) {
	  cout << "CONFIGURE_TDC Data response: first byte: 0x" 
	       << hex << m.DATA[0] 
	       << " expected 0x" << sendData << endl;
	  my_private_exit(errno);
	}
	// Message is good, continue
#ifdef LOCAL_DEBUG
	cout << "Received Config_end response:\n";
#endif
	
      }
      
    } // data read
  } // data received
  else {	
    cout << "ERROR: Sent TDC_CONFIGURE:Config_end packet, but did not receive response within 1 sec" << endl;
    my_private_exit(errno);
  }

  // "CONFIGURE_TDC:Program"
  ms.DATA[0] = sendData = 0xc0;
  
#ifdef LOCAL_DEBUG
  cout << "\nSending Program command:\n";
  printf("p_config: message assembled: %c %c 0x%08x %1d  ", 
	 (ms.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
	 (ms.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
	 ms.ID, 
	 ms.LEN); 
  for (j = 0; j < ms.LEN; j++)
    printf("0x%02x ", ms.DATA[j]);
  printf("\n");
#endif
  
  
  // send the message
  if ( (errno = CAN_Write(h, &ms)) ) {
    perror("p_config: CAN_Write()");
    my_private_exit(errno);
  }
  
  
  errno = poll(&pfd, 1, 1000); // timeout = 1 seconds
  //printf("poll returned 0x%x\n", errno);
  //printf("revents = 0x%x\n", pfd.revents);
  
  if (errno == 1) { // data received
    if ((errno = CAN_Read(h, &m))) {
      perror("p_config: CAN_Read()");
      my_private_exit(errno);
    }
    else { // data read
#ifdef LOCAL_DEBUG
      printf("p_config: message received : %c %c 0x%08x %1d  ", 
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
	  perror("p_config: CAN_Status()");
	  my_private_exit(errno);
	}
	else
	  printf("p_config: pending CAN status 0x%04x read.\n", (__u16)status);
      } 
      else if (m.MSGTYPE == MSGTYPE_STANDARD) {
	// now interprete the received message:
	if (m.ID != (0x180 | TDCVal | nodeID)) { // check if it's a p_config response
	  cout << "CONFIGURE_TDC:Program: Got smething other than p_config response: ID 0x" 
	       << hex << m.ID 
	       << ", expected 0x" << (0x180 | TDCVal | nodeID) << endl;	
	  my_private_exit(errno);
	}
	if (m.LEN != 1) { // check for correct length
	  cout << "ERROR: CONFIGURE_TDC:Program: Got msg with incorrect data length" 
	       << dec << m.LEN << ", expected 1\n";
	  my_private_exit(errno);
	}
	if (m.DATA[0] != sendData) {
	  cout << "CONFIGURE_TDC Data response: first byte: 0x" 
	       << hex << m.DATA[0] 
	       << " expected 0x" << sendData << endl;
	  my_private_exit(errno);
	}
	// Message is good, continue
#ifdef LOCAL_DEBUG
	cout << "Received Program response\n";
#endif
	cout << "... Configuration finished successfully.\n";
	
      }
      
    } // data read
  } // data received
  else {	
    cout << "ERROR: Sent TDC_CONFIGURE:Program packet, but did not receive response within 1 sec" << endl;
  }

  my_private_exit(errno);
  
  return errno;
}


int main(int argc, char *argv[])
{
  unsigned int nodeID;

  cout << "p_config, last modified August 18, 2004" << endl;
  cout.flush();
  
  if ( argc != 4 ) {
    cout << "USAGE: " << argv[0] << " <TDC ID> <node ID> <fileName> \n";
    return 1;
  }
  
  int TDC = atoi(argv[1]);
  if ((TDC < 1) || (TDC > 4)) { 
    cerr << "TDC = " << TDC << " invalid entry. Use 1-4 instead." << endl;
    return 1;
  }

  nodeID = atoi(argv[2]);
  if ((nodeID < 1) || (nodeID > 8)) { 
    cerr << "nodeID = " << nodeID 
	 << " is an invalid entry.  Use a value between 1 and 8 instead.\n";
    return 1;
  }
  
  return p_config(argv[3], nodeID, TDC);
}
