/* File name     : change_mcu_program.cc
 * Creation date : 9/13/04
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: change_mcu_program.cc,v 1.4 2009-06-25 20:36:49 jschamba Exp $";
#endif /* lint */

#define LOCAL_DEBUG

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
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>

// pcan include file
#include <libpcan.h>
//****************************************************************************
// GLOBALS
HANDLE h = NULL;
ifstream hexfile;

#define LOCAL_STRING_LEN 64       // length of internal used strings
typedef struct
{
  char szVersionString[LOCAL_STRING_LEN];
  char szDevicePath[LOCAL_STRING_LEN];
  int  nFileNo;
} PCAN_DESCRIPTOR;
PCAN_DESCRIPTOR *pcd;

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
  if (hexfile.is_open()) hexfile.close();
#ifdef LOCAL_DEBUG
  cout << "change_mcu_program: finished (" << error << ")\n";
#endif
  exit(error);
}

//****************************************************************************
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
  char msgTxt[256];

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


//****************************************************************************
int send_Start(const unsigned int nodeID)
{
  TPCANMsg ms;
    
  ms.MSGTYPE = MSGTYPE_STANDARD;
  ms.ID = 0x400 | nodeID;
  ms.LEN = 1;

  // "CHANGE_MCU_PROGRAM:Start"
  ms.DATA[0] = 0;

#ifdef LOCAL_DEBUG
  printCANMsg(ms, "CHANGE_MCU_PROGRAM:Start, sent msg");
#endif
  

  //return 0;
  return (sendCAN_and_Compare(ms, "CHANGE_MCU_PROGRAM:Start"));
}

//****************************************************************************
int send_64bytes(unsigned char *bytes,
		 const unsigned int startAddr,
		 const int checksum_64,
		 const unsigned int nodeID
		 )
{
  TPCANMsg ms;
  int status;

  if ((errno = send_Start(nodeID)) != 0) {
    cerr << "change_mcu_program: send_Start() failed with errno 0x"
		<< hex << errno << endl;
    my_private_exit(errno);
  }

  ms.MSGTYPE = MSGTYPE_STANDARD;
  ms.ID = 0x400 | nodeID;
  ms.LEN = 7;
  ms.DATA[0] = 1;
  ms.DATA[1] = (startAddr>>8) & 0xff; // upper 8 bits of address

  // send 16 Data messages with 4 data bytes each 
  for (int i=0; i<16; i++) {
    // "CHANGE_MCU_PROGRAM:Data"
    ms.DATA[2] = ((startAddr & 0xff) + (i*4)); // lower 8 bits of address for these 4 bytes
    for (int j=0; j<4; j++) 
      ms.DATA[3+j] = bytes[i*4+j];
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "CHANGE_MCU_PROGRAM:Data, sent msg");
#endif
    
    if( (status = sendCAN_and_Compare(ms, "CHANGE_MCU_PROGRAM:Data")) != 0) return status;

  } // for (int i=0; i<16; ....


  // "CHANGE_MCU_PROGRAM:Checksum_64"
  ms.LEN = 4;
  ms.DATA[0] = 0x2;
  ms.DATA[1] = (startAddr>>8) & 0xff; // upper 8 bits of address
  ms.DATA[2] = startAddr & 0xff; // lower 8 bits of address for these 4 bytes
  ms.DATA[3] = checksum_64 & 0xff; // checksum for 64-byte block 
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "CHANGE_MCU_PROGRAM:Checksum_64, sent msg");
#endif
  
  if( (status = sendCAN_and_Compare(ms, "CHANGE_MCU_PROGRAM:Checksum_64")) != 0) return status;
  
  // "CHANGE_MCU_PROGRAM:Program_64"
  ms.LEN = 3;
  ms.DATA[0] = 0x3;
  ms.DATA[1] = (startAddr>>8) & 0xff; // upper 8 bits of address
  ms.DATA[2] = startAddr & 0xff; // lower 8 bits of address for these 4 bytes
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "CHANGE_MCU_PROGRAM:Program_64, sent msg");
#endif
  
  //return 0;
  return (sendCAN_and_Compare(ms, "CHANGE_MCU_PROGRAM:Program_64"));
}


//**********************************************
// here all is done
int change_mcu_program(const char *filename, unsigned int nodeID, WORD devID)
{
  char txt[255]; // temporary string storage
  string buf;
  int checksum_64, final_checksum;
  unsigned int startAddr, endAddr;
  unsigned  char pgmByte[64];

  TPDIAG my_PDiag;
  char devName[255];


  cout << "Changing MCU program at NodeID 0x" << hex << nodeID
       << " with filename " << filename 
       << " at devID 0x" << hex << devID << "...\n";


  // initialize:
  for (int i=0; i<64; i++) pgmByte[i] = 0xff;
  checksum_64 = 0;
  final_checksum = 0;
  startAddr = 0;
  endAddr = 0;


  hexfile.open(filename); // "in" is default
  if ( !hexfile.good() ) {
    cerr << filename << ": file open error\n";
    return -1;
  }

  hexfile.seekg(0,ios::beg);    //move file pointer to beginning of file

  errno = 0;

  // install signal handler for manual break
  signal(SIGINT, signal_handler);
  
  // open the CAN port
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
    perror("change_mcu_program: CAN_Open()");
    my_private_exit(errno);
  }

  // setup polling structure
  pcd = (PCAN_DESCRIPTOR *)h;
  pfd.fd = pcd->nFileNo;
  pfd.events = POLLIN;
    
  // get version info
  errno = CAN_VersionInfo(h, txt);
  if (!errno) {
#ifdef LOCAL_DEBUG
    cout << "change_mcu_program: driver version = "<< txt << endl;
#endif
  }
  else {
    perror("change_mcu_program: CAN_VersionInfo()");
    my_private_exit(errno);
  }

  //  CAN Port for TDIG at 1Mb/s, init PCAN-USB
  //errno = CAN_Init(h, CAN_BAUD_1M,  CAN_INIT_TYPE_ST);
  //  CAN Port for TDIG at 500kb/s, init PCAN-USB
  errno = CAN_Init(h, CAN_BAUD_500K,  CAN_INIT_TYPE_ST);
  if (errno) {
    perror("change_mcu_program: CAN_Init()");
    my_private_exit(errno);
  } 

  errno = 0;
  if (send_Start(nodeID) != 0) {
    errno = nGetLastError();
    perror("change_mcu_program: send_Start()");
    my_private_exit(errno);
  }
    

  while (hexfile.good()) {
    string subs;
    unsigned int subInt, addr, rtype;
    int len;
    unsigned char dataByte[256];

    
    /*	read a record (line) from the hex file. Each record has the following format:
      
    	":BBAAAATTHHHH...HHCC"
    
    	where:
    
    	BB	number of data bytes
    	AAAA	start address of data record
    	TT	record type:
    		00 - data record
    		01 - end-of-file record
    		02 - segment address record
    		04 - linear address record (address in data bytes)
    	HH	data byte
    	CC	checksum (2's complement of sum of preceding bytes)
    
    	addresses greater than 0x1ffff are not valid for internal flash memory, 
	except for configuration bits.

	it looks like the length produced by MPLAB is at most "16", so we'll assume this
	from now on to make the program easier!!!!
    */
    
    hexfile >> buf;
    //cout << "read line: " << buf << endl;
    
    subs = buf.substr(1,2);
    len = strtol(subs.c_str(), 0, 16);
    subs = buf.substr(3,4);
    addr = strtol(subs.c_str(), 0, 16);
    subs = buf.substr(7,2);
    rtype = strtol(subs.c_str(), 0, 16);
    
#ifdef LOCAL_DEBUG
    cout << "length = " << setw(2) << dec << len 
	 <<" type = " << rtype
	 << " address = " << showbase << hex << setw(6) << addr;
#endif

    if(rtype == 0x4) {
      subs = buf.substr(9,4);
      subInt = strtol(subs.c_str(), 0, 16);
#ifdef LOCAL_DEBUG
      cout << " upper address = " << subInt << endl;
#endif
      if (subInt > 1) break; // outside the program memory
    }
    else if (rtype == 0x1) { //eof record
#ifdef LOCAL_DEBUG
      cout << endl;
#endif
      break;
    }
    else if (rtype == 0x00) { // data record
      // read the data bytes:
      for (int i=0; i<len; i++) {
	subs = buf.substr(9+2*i,2);
	dataByte[i] = (unsigned char)strtol(subs.c_str(), 0, 16);
      }
#ifdef LOCAL_DEBUG
      cout << " data:" << noshowbase;
      for (int i=0; i<len; i++) 
	cout << " " << setw(2) << ((unsigned int)(dataByte[i]));
      cout << endl;
#endif
      // process the data record
      if (addr >= endAddr) {
	if (endAddr > startAddr) {
#ifdef LOCAL_DEBUG
	  cout << " finished a 64byte block:\n";
	  for (int i=0; i<4; i++) {
	    cout << "\taddr " << showbase << hex << setw(6) << (startAddr+i*16);
	    cout << ":" << noshowbase;
	    for (int j=0; j<16; j++) cout << setw(3) << ((unsigned int)pgmByte[i*16+j]);
	    cout << endl;
	  }
#endif
	  // calculate the checksum
	  for (int i=0; i<64; i++)
	    checksum_64 += pgmByte[i];
	  checksum_64 = checksum_64 & 0xff;
	  final_checksum += checksum_64;
	  final_checksum = final_checksum & 0xff;
	    
	  // send the block over CAN
	  if( send_64bytes(pgmByte, startAddr, checksum_64, nodeID) != 0)
	    my_private_exit(errno);
	}
	
	// now start a new block
	for (int i=0; i<64; i++) pgmByte[i] = 0xff;
	startAddr = addr & 0xffc0;

	// if there is a gap in the address...
	if (startAddr != endAddr) {
#ifdef LOCAL_DEBUG
	  cout << "\n\n !!!!!! GAP !!!!!!\n\n";
#endif
	  // ... fill in a bunch of 64-byte blocks with 0xff's
	  checksum_64 = 0;
	  for (int i=0; i<64; i++) 
	    checksum_64 += pgmByte[i];
	  checksum_64 = checksum_64 & 0xff;

	  for (unsigned int tmpAddr=endAddr; tmpAddr<startAddr; tmpAddr+=64) {
	    final_checksum += checksum_64;
	    final_checksum = final_checksum & 0xff;
	    
#ifdef LOCAL_DEBUG
	    cout << " finished a 64byte block:\n";
	    for (int i=0; i<4; i++) {
	      cout << "\taddr " << showbase << hex << setw(6) << (tmpAddr+i*16);
	      cout << ":" << noshowbase;
	      for (int j=0; j<16; j++) cout << setw(3) << ((unsigned int)pgmByte[i*16+j]);
	      cout << endl;
	    }
#endif
	    if( send_64bytes(pgmByte, tmpAddr, checksum_64, nodeID) != 0)
	      my_private_exit(errno);
	  }
	  // here we should be caught up on the address again
#ifdef LOCAL_DEBUG
	  cout << "\n\n !!!!!! END OF GAP !!!!!!\n\n";
#endif
	}
	endAddr = startAddr + 64;
	checksum_64 = 0;
      }
      
      // now fill in the bytes:
      for (int i=0; i<len; i++) pgmByte[addr-startAddr+i] = dataByte[i];
      
    } // else if (rtype == 0x00...
    else
      cerr << ":  unexpected record type\n";

  }
      
  hexfile.close();

  if (endAddr > startAddr) {
#ifdef LOCAL_DEBUG
    cout << " finished last 64byte block:\n";
    for (int i=0; i<4; i++) {
      cout << "\taddr " << showbase << hex << setw(6) << (startAddr+i*16);
      cout << ":" << noshowbase;
      for (int j=0; j<16; j++) cout << setw(3) << ((unsigned int)pgmByte[i*16+j]);
      cout << endl;
    }
#endif
    // calculate the checksum
    for (int i=0; i<64; i++)
      checksum_64 += pgmByte[i];
    checksum_64 = checksum_64 & 0xff;
    final_checksum += checksum_64;
    final_checksum = final_checksum & 0xff;
    
    // send the block over CAN (to be done...)
    if( send_64bytes(pgmByte, startAddr, checksum_64, nodeID) != 0)
      my_private_exit(errno);
  }

  // Do a final checksum
  TPCANMsg ms;
  ms.MSGTYPE = MSGTYPE_STANDARD;
  ms.ID = 0x400 | nodeID;
  ms.LEN = 4;

  // "CHANGE_MCU_PROGRAM:Final_chksum"
  ms.DATA[0] = 0x4;
  ms.DATA[1] = (endAddr>>8) & 0xff; // upper 8 bits of final address
  ms.DATA[2] = endAddr & 0xff; // lower 8 bits of final address
  ms.DATA[3] = final_checksum & 0xff; // checksum for 64-byte block 

#ifdef LOCAL_DEBUG
  printCANMsg(ms, "CHANGE_MCU_PROGRAM:Final_chksum, sent msg");
#endif
  
  errno = 0;
  if ( sendCAN_and_Compare(ms, "CHANGE_MCU_PROGRAM:Final_chksum") != 0)
    my_private_exit(errno);


  // And jump to the new program:
  ms.LEN = 1;

  // "CHANGE_MCU_PROGRAM:Jump_PC"
  ms.DATA[0] = 0x5;

#ifdef LOCAL_DEBUG
  printCANMsg(ms, "CHANGE_MCU_PROGRAM:Jump_PC, sent msg");
#endif

  errno = 0;
  sendCAN_and_Compare(ms, "CHANGE_MCU_PROGRAM:Jump_PC");
  
  my_private_exit(errno);
  
  return 0;
}

//****************************************************************************
int main(int argc, char *argv[])
{
  unsigned int nodeID;
  WORD devID = 255;


  cout << vcid << endl;
  cout.flush();

  if ( argc < 3 ) {
    cout << "USAGE: " << argv[0] << " <node ID> <fileName> [<devID>]\n";
    return 1;
  }
  
  nodeID = atoi(argv[1]);
  if ((nodeID < 0) || (nodeID > 7)) { 
    cerr << "nodeID = " << nodeID 
	 << " is an invalid entry.  Use a value between 0 and 7 instead.\n";
    return -1;
  }
  
  if (argc == 4) {
    devID = atoi(argv[3]);
    if (devID > 255) {
      printf("Invalid Device ID 0x%x. Use a device ID between 0 and 255\n", devID);
      return -1;
    }
  }

  return change_mcu_program(argv[2], nodeID, devID);
}
