/* File name     : p_config.cc
 * Creation date : 8/18/04
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: p_config.cc,v 1.5 2005-02-10 20:29:15 jschamba Exp $";
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

struct pollfd pfd;

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



//
//****************************************************************************
void printCANMsg(const TPCANMsg &msg, const char *msgTxt)
{
  printf("%s: %c %c 0x%03x %1d  ",
	 msgTxt,
	 (msg.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
	 (msg.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
	 msg.ID, 
	 msg.LEN); 
  for (int i = 0; i < msg.LEN; i++)
    printf("0x%02x ", msg.DATA[i]);
  printf("\n");
}

//****************************************************************************
int sendCAN_and_Compare(TPCANMsg &ms, const char *errorMsg)
{
  TPCANMsg mr;
  __u32 status;
#ifdef LOCAL_DEBUG
  char msgTxt[256];
#endif

  // send the message
  if ( (errno = CAN_Write(h, &ms)) ) {
    perror("change_mcu_program: CAN_Write()");
    return(errno);
  }

  errno = poll(&pfd, 1, 1000); // timeout = 1 seconds
    
  if (errno == 1) { // data received
    if ((errno = CAN_Read(h, &mr))) {
      perror("change_mcu_program: CAN_Read()");
      return(errno);
    }
    else { // data read
#ifdef LOCAL_DEBUG
      sprintf(msgTxt, "%s, message received", errorMsg);
      printCANMsg(mr, msgTxt);
#endif
      // check if a CAN status is pending	     
      if (mr.MSGTYPE & MSGTYPE_STATUS) {
	status = CAN_Status(h);
	if ((int)status < 0) {
	  errno = nGetLastError();
	  perror("change_mcu_program: CAN_Status()");
	  return(errno);
	}
	else
	  cout << "change_mcu_program: pending CAN status " << showbase << hex << status << " read.\n";
      } 
      else if (mr.MSGTYPE == MSGTYPE_STANDARD) {
	// now interprete the received message:
	if ( mr.ID != (0x380 | (ms.ID&0x00f)) ) { // check if it's a CHANGE_MCU_PROGRAM response
	  cout << "change_mcu_program request: Got smething other than change_mcu_program response: ID " 
	       << showbase << hex << (unsigned int)mr.ID 
	       << ", expected " << (unsigned int)ms.ID << endl;	
	  return (-2);
	}
	if (mr.LEN != ms.LEN) { // check for correct length
	  cout << "ERROR: " << errorMsg << " request: Got msg with incorrect data length" 
	       << dec << (int)mr.LEN << ", expected " << (int)ms.LEN << endl;
	  cout << errorMsg << " response: first byte: " 
	       << showbase << hex << (unsigned int)mr.DATA[0] 
	       << " expected " << (unsigned int)ms.DATA[0] << endl;
	  return (-3);
	}
	if (mr.DATA[0] != ms.DATA[0]) {
	  cout << errorMsg << " response: first byte: " 
	       << showbase << hex << (unsigned int)mr.DATA[0] 
	       << " expected " << (unsigned int)ms.DATA[0] << endl;
	  return (-4);
	}
	// Message is good, continue

      }
      
    } // data read
  } // data received
  else {	
    cout << "ERROR: Sent " << errorMsg << " packet, but did not receive response within 1 sec" << endl;
    return (-5);
  }
  return 0;
}









//**********************************************
// here all is done
int p_config(const char *filename, unsigned int nodeID, int TDC, WORD devID)
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

  TPDIAG my_PDiag;
  char devName[255];

  cout << "Configuring TDC " << TDC
       << " at NodeID 0x" << hex << nodeID
       << " with filename " << filename 
       << " on CANbus ID 0x" << hex << devID << "...\n";

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
  //h = LINUX_CAN_Open("/dev/pcan32", O_RDWR|O_NONBLOCK);

  // search for correct device ID:
  for (int i=0; i<8; i++) {
    sprintf(devName, "/dev/pcan%d", 32+i);
    //h = CAN_Open(HW_USB, dwPort, wIrq);
    h = LINUX_CAN_Open(devName, O_RDWR|O_NONBLOCK);
    if (h == NULL) {
      //printf("Failed to open device %s\n", devName);
      //my_private_exit(errno);
      continue;
    }
    // get the hardware ID from the diag structure:
    LINUX_CAN_Statistics(h,&my_PDiag);
    printf("\tDevice at %s: Hardware ID = 0x%x\n", devName, 
	   my_PDiag.wIrqLevel);
    if (my_PDiag.wIrqLevel == devID) break;
    CAN_Close(h);
  }

  if (!h) {
    printf("Device ID 0x%x not found, exiting\n", devID);
    errno = nGetLastError();
    perror("p_config: CAN_Open()");
    my_private_exit(errno);
  }

  PCAN_DESCRIPTOR *pcd = (PCAN_DESCRIPTOR *)h;
  
    
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
    
  pfd.fd = pcd->nFileNo;
  pfd.events = POLLIN;
    
  ms.MSGTYPE = CAN_INIT_TYPE_ST;
  ms.ID = 0x200 | TDCVal | nodeID;
  ms.LEN = 1;

  // "CONFIGURE_TDC:Start"
  sendData = ms.DATA[0] = 0;

#ifdef LOCAL_DEBUG
  printCANMSG(ms, "p_config: Sending START command:");
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
	  cout << "p_config request: Got something other than p_config response: ID 0x" 
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
	    cout << "CONFIGURE_TDC:Data: Got something other than p_config response: ID 0x" 
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
	  cout << "CONFIGURE_TDC:Data: Got something other than p_config response: ID 0x" 
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
	  cout << "CONFIGURE_TDC:Config_end: Got something other than p_config response: ID 0x" 
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
	  cout << "CONFIGURE_TDC:Program: Got something other than p_config response: ID 0x" 
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
	//cout << "... Configuration finished successfully.\n";
	
      }
      
    } // data read
  } // data received
  else {	
    cout << "ERROR: Sent TDC_CONFIGURE:Program packet, but did not receive response within 1 sec" << endl;
  }




  // "TDC Reset at the end
  ms.ID = 0x500 | nodeID;
  ms.LEN = 3;
  
  sendData = ms.DATA[0] = 0xbb;
  sendData = ms.DATA[1] = 0x04;
  sendData = ms.DATA[2] = 0x01;
  
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
	if (m.ID != (0x480 | nodeID)) { // check if it's a "DEBUG" response
	  cout << "DEBUG:TDC Reset: Got something other than DEBUG response: ID 0x" 
	       << hex << m.ID 
	       << ", expected 0x" << (0x480 | nodeID) << endl;	
	  my_private_exit(errno);
	}
	if (m.LEN != 3) { // check for correct length
	  cout << "ERROR: DEBUG:TDC Reset: Got msg with incorrect data length" 
	       << dec << m.LEN << ", expected 3\n";
	  my_private_exit(errno);
	}
	// Message is good, continue
#ifdef LOCAL_DEBUG
	cout << "Received DEBUG response:\n";
#endif
      }
      
    } // data read
  } // data received
  else {	
    cout << "ERROR: Sent DEBUG:Reset TDC packet, but did not receive response within 1 sec" << endl;
    my_private_exit(errno);
  }

  // "TDC Reset at the end, de-assert
  sendData = ms.DATA[2] = 0x00;
  
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
	if (m.ID != (0x480 | nodeID)) { // check if it's a "DEBUG" response
	  cout << "DEBUG:TDC Reset: Got something other than DEBUG response: ID 0x" 
	       << hex << m.ID 
	       << ", expected 0x" << (0x480 | nodeID) << endl;	
	  my_private_exit(errno);
	}
	if (m.LEN != 3) { // check for correct length
	  cout << "ERROR: DEBUG:TDC Reset: Got msg with incorrect data length" 
	       << dec << m.LEN << ", expected 3\n";
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
    cout << "ERROR: Sent DEBUG:Reset TDC packet, but did not receive response within 1 sec" << endl;
    my_private_exit(errno);
  }




  // "... and finally, a PLD Reset at the end
  ms.LEN = 4;
  
  sendData = ms.DATA[0] = 0x69;
  sendData = ms.DATA[1] = 0x96;
  sendData = ms.DATA[2] = 0xa5;
  sendData = ms.DATA[3] = 0x5a;
  
#ifdef LOCAL_DEBUG
  cout << "\nSending DEBUG message:\n";
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
	if (m.ID != (0x480 | nodeID)) { // check if it's a "DEBUG" response
	  cout << "DEBUG:PLD Reset: Got something other than DEBUG response: ID 0x" 
	       << hex << m.ID 
	       << ", expected 0x" << (0x480 | nodeID) << endl;	
	  my_private_exit(errno);
	}
	if (m.LEN != 4) { // check for correct length
	  cout << "ERROR: DEBUG:PLD Reset: Got msg with incorrect data length" 
	       << dec << m.LEN << ", expected 4\n";
	  my_private_exit(errno);
	}
	// Message is good, continue
#ifdef LOCAL_DEBUG
	cout << "Received DEBUG response:\n";
#endif
      }
      
    } // data read
  } // data received
  else {	
    cout << "ERROR: Sent DEBUG:Reset PLD packet, but did not receive response within 1 sec" << endl;
    my_private_exit(errno);
  }

  cout << "... Configuration finished successfully.\n";


  my_private_exit(errno);
  
  return errno;
}


int main(int argc, char *argv[])
{
  unsigned int nodeID;
  WORD devID = 255;

  cout << vcid << endl;
  cout.flush();
  
  if ( argc < 4 ) {
    cout << "USAGE: " << argv[0] << " <TDC ID> <node ID> <fileName> <devID>\n";
    return 1;
  }
  
  int TDC = atoi(argv[1]);
  if ((TDC < 1) || (TDC > 4)) { 
    cerr << "TDC = " << TDC << " invalid entry. Use 1-4 instead." << endl;
    return 1;
  }

  nodeID = atoi(argv[2]);
  if ((nodeID < 0) || (nodeID > 7)) { 
    cerr << "nodeID = " << nodeID 
	 << " is an invalid entry.  Use a value between 0 and 7 instead.\n";
    return 1;
  }
  
  if (argc == 5) {
    devID = atoi(argv[4]);
    if (devID > 255) {
      printf("Invalid Device ID 0x%x. Use a device ID between 0 and 255\n", devID);
      return -1;
    }
  }
  
  return p_config(argv[3], nodeID, TDC, devID);
}
