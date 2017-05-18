/* File name     : x_MCU2_noIVT.cc
 * Creation date : 7/20/2012
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: x_MCU2_noIVT.cc 769 2012-07-20 18:45:38Z jschamba $";
#endif /* lint */

/* 
 * This program is used to re-program the MCU on TDIG
 * and TCPU according to Bill Burton's instructions
 */

//#define LOCAL_DEBUG
//#define NO_CAN

//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

// other headers
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

// pcan include file
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

// local utilities
#include "can_utils.h"

//****************************************************************************
// GLOBALS
int h = -1;
ifstream hexfile;

const int ERASE_NONE = 0;
const int ERASE_NORMAL = 1;
const int ERASE_PRESERVE = 2;

unsigned int validl[] = {          0x000,       0x4000};  // this tells the lowest valid address
unsigned int validh[] = {          0x000,       0x5FFF};  // this tells the highest valid address
/*
 * Highest available address in TDIG MCU: 0xABFD
 */

//****************************************************************************
// LOCALS


//****************************************************************************
// CODE 

// centralized entry point for all exits
static void my_private_exit(int error)
{
  if (h>0)
  {
    close(h); 
  }
  if (hexfile.is_open()) hexfile.close();
#ifdef LOCAL_DEBUG
  cout << "x_MCU2_withOffset: finished (" << error << ")\n";
#endif
  exit(error);
}

//****************************************************************************
// handles CTRL-C user interrupt
static void signal_handler(int signal)
{
  my_private_exit(0);
}



//****************************************************************************
int xwrite_mcu_block(unsigned char *bytes,
		    const unsigned int startAddr,
		    const int bytecount,
		    const int eraseflag,
		    const unsigned int msgIDVal
		 )
{
  unsigned int checksum;
  struct can_frame ms;
#ifndef NO_CAN
  struct can_frame mr;
#endif
  if (bytecount != 0) {

    checksum = 0;
    // ************** MCU2:WriteBlockStart ****************************************
  
    ms.can_id = msgIDVal | CAN_EFF_FLAG;
    ms.can_dlc = 1;
    ms.data[0] = 0x10;	// Block Start
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "x_MCU2: Sending BlockStart command:");
#endif
    
#ifndef NO_CAN
    if ( sendCAN_and_Compare(h, ms, "x_MCU2:BlockStart", 4000000, 2, true) != 0) { // timeout = 4 sec
      my_private_exit(errno);
    }
#endif
    
    unsigned char *tmpPtr = bytes;
    int numFullMsgs = bytecount/7;
    int remainingToSend = bytecount%7;

    ms.data[0] = 0x20;	// Block Data
    ms.can_dlc = 8;
    // send "numFullMsgs" packets with 7 bytes each
    for (int i=0; i<numFullMsgs; i++) {
      // ************** MCU2:BlockData ************************************
      for (int j=1; j<8; j++) {
	ms.data[j] = *tmpPtr;
	checksum += *tmpPtr;
	tmpPtr++;
      }
#ifdef LOCAL_DEBUG
      printCANMsg(ms, "x_MCU2: Sending BlockData command:");
#endif
      
#ifndef NO_CAN
      if ( sendCAN_and_Compare(h, ms, "x_MCU2:BlockData", 4000000, 2, true) != 0) { // timeout = 4 sec
	my_private_exit(errno);
      }
#endif
      
    }

    // Now send the remainingToSend bytes in an extra packet:
    if (remainingToSend > 0) {
      ms.can_dlc = remainingToSend+1;
      for (int j=1; j<(remainingToSend+1); j++) {
	ms.data[j] = *tmpPtr;
	checksum += *tmpPtr;
	tmpPtr++;
      }
#ifdef LOCAL_DEBUG
      printCANMsg(ms, "x_MCU2: Sending BlockData command:");
#endif
#ifndef NO_CAN
      if ( sendCAN_and_Compare(h, ms, "x_MCU2:BlockData", 4000000, 2, true) != 0) { // timeout = 4 sec
	my_private_exit(errno);
      }
#endif

    }
    
    // ************** MCU2:BlockEnd ****************************************
    
    ms.data[0] = 0x30;	// Block End
    ms.can_dlc = 1;
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "x_MCU2: Sending BlockEnd command:");
#endif
    
#ifndef NO_CAN
    if (write(h, &ms, sizeof(ms)) != sizeof(ms)) {
      perror("x_MCU2:BlockEnd: write()");
      return -1;
    }
    errno = CAN_Read_Timeout(h, &mr, 4000000); // timeout = 4 second
    if (errno < 0) {
      cout << "CAN_Read_Timeout returned " << errno 
	   << " during x_MCU2:BlockEnd \n";
      return -1;
    }
    else if (errno == 0)
      cout << "Timeout during x_MCU2:BlockEnd\n";

    unsigned int expectedResponseID = ms.can_id | (0x1 <<18);
    // Now check and compare
    if ( mr.can_id != expectedResponseID ) {
      cout << "ERROR: BlockEnd Command " 
	   << " request: Got something other than writeResponse: ID " 
	   << showbase << hex << (unsigned int)(mr.can_id & CAN_EFF_MASK) 
	   << ", expected response to " << (unsigned int)(ms.can_id & CAN_EFF_MASK) << endl;	
      printCANMsg(mr, "response:");
      my_private_exit(-2);
    }

    if (mr.can_dlc != 8) { // check for correct length
      cout << "ERROR: BlockEnd Command" 
	   << " request: Got msg with incorrect data length " 
	   << dec << (int)mr.can_dlc << ", expected 8\n";
      cout << "BlockEnd Command response: first byte: " 
	   << showbase << hex << (unsigned int)mr.data[0] 
	   << " expected " << (unsigned int)ms.data[0] << endl;
      printCANMsg(mr, "response");
      my_private_exit(-3);
    }
    if (mr.data[0] != ms.data[0]) { 	// check for correct echo
      cout << "ERROR: BlockEnd Command response: first byte: " 
	   << showbase << hex << (unsigned int)mr.data[0] 
	   << " expected " << (unsigned int)ms.data[0] << endl;
      printCANMsg(mr, "response:");
      my_private_exit(-4);
    }
    if (mr.data[1] != 0) {	// check for correct status
      cout << "ERROR: BlockEnd Command response: second (status) byte: " 
	   << showbase << hex << (unsigned int)mr.data[0] << endl; 
      printCANMsg(mr, "response:");
      my_private_exit(-8);
    }

    int receivedByteCount = (int)(mr.data[3] << 8) + (int)(mr.data[2]);
    if (receivedByteCount != bytecount) { 	// check for correct returned byte count
      cout << "ERROR: BlockEnd Command: Replied block length " << dec << receivedByteCount
	   << " not " << bytecount << endl;
      printCANMsg(mr, "response:");
      my_private_exit(-9);
    }      

    unsigned int lwork;
    memcpy ((unsigned char *)&lwork, (unsigned char *)&(mr.data[4]), 4); // checksum
    if (lwork != checksum) {
      cout << "ERROR: BlockEnd Command: Replied checksum " << showbase << hex << lwork
	   << "; expected " << checksum << endl;
      printCANMsg(mr, "response:");
      my_private_exit(-10);
    }      
#endif
    
    // ************** MCU2:BlockTargetMCU2 ****************************************
    
    //unsigned int startAddr2 = startAddr + 0x6000; // for use with offset
    unsigned int startAddr2 = startAddr;

    ms.can_dlc = 6;
    ms.data[0] = 0x4c;	// Block Target MCU2
    ms.data[1] = (unsigned char)(startAddr2 & 0xff);
    ms.data[2] = (unsigned char)((startAddr2 >>  8) & 0xff);
    ms.data[3] = (unsigned char)((startAddr2 >> 16) & 0xff);
    ms.data[4] = (unsigned char)((startAddr2 >> 24) & 0xff);
    ms.data[5] = ERASE_NONE;	// assume no erase needed on others
    if (eraseflag == ERASE_PRESERVE)
      ms.data[5] = ERASE_PRESERVE;	// preserve unprogrammed data
    else if ((startAddr & 0x003ff) == 0)
      ms.data[5] = ERASE_NORMAL;	// erase needed on boundary

#ifdef LOCAL_DEBUG
    printCANMsg(ms, "x_MCU2: Sending BlockTargetMCU2 command:");
#endif
#ifndef NO_CAN
    if ( sendCAN_and_Compare(h, ms, "x_MCU2:BlockTargetMCU2", 4000000, 2, true) != 0) { // timeout = 4 sec
      cout << "startAddr = 0x" << hex << startAddr2 << endl;
      my_private_exit(errno);
    }
#endif

  } // if (bytecount != 0)
  
  return 0;
}



//**********************************************
// here all is done
int change_mcu_program(const char *filename, 
		       unsigned int tdigNodeID, 
		       unsigned int tcpuNodeID, 
		       int devID)
{
  string buf;
  unsigned int address;
  unsigned int baseAddress = 0;
  unsigned int extendAddress = 0;
  int downloadbytes = 0;
  int eraseflag;
  

  cout << "Downloading MCU second image on TDIG NodeID 0x" << hex << tdigNodeID
       << " through TCPU NodeID 0x" << tcpuNodeID
       << "\n with filename " << filename 
       << " at devID " << dec << devID << "...\n";


  unsigned int msgIDVal = (((tdigNodeID<<4) | 0x002) << 18) | tcpuNodeID;
 
  hexfile.open(filename); // "in" is default
  if ( !hexfile.good() ) {
    cerr << filename << ": file open error\n";
    return -1;
  }

  hexfile.seekg(0,ios::beg);    //move file pointer to beginning of file

  errno = 0;

  
  // swallow any packets that might be present first
#ifndef NO_CAN
  struct can_frame mr;
  errno = CAN_Read_Timeout(h, &mr, 100000); // timeout = 100 mseconds
#endif



  // allocate space for program bytes

  unsigned char *ivtBytes, *progBytes;
  ivtBytes = (unsigned char *)NULL;
  progBytes = (unsigned char *)NULL;

  if ((validh[0] - validl[0]) > 2) // a valid IVT address range
    ivtBytes = (unsigned char *)malloc(validh[0] - validl[0]);
  else {
    ivtBytes = (unsigned char *)NULL;
    validh[0] = validl[0];
  }
  progBytes = (unsigned char *)malloc(validh[1] - validl[1]);
  memset((void *)ivtBytes, 0, (validh[0] - validl[0])); // default to 0
  memset((void *)progBytes, 0, (validh[1] - validl[1])); // default to 0


  // determine number of pages:
  int numPages = (int)((validh[1] - validl[1])/ 0x800); 
  // determine number of rows:
  int numRows = numPages * 8;
  
  unsigned char *pageEmpty, *rowEmpty;
  pageEmpty = (unsigned char *)malloc(numPages);
  rowEmpty = (unsigned char *)malloc(numRows);
  memset((void *)pageEmpty, 1, numPages);
  memset((void *)rowEmpty, 1, numRows);
		       

  while (hexfile.good()) {
    string subs;
    unsigned int subInt, addr, rtype;
    int len;
    
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

    address = (addr + extendAddress);


    
#ifdef LOCAL_DEBUG
    cout << "length = " << setw(2) << dec << len 
	 <<" type = " << rtype
	 << " address = " << showbase << hex << setw(6) << addr 
         << " addr + ext = " << address
	 << " prog address = " << (address/2)
	 << endl;
#endif


    // **************** RECORD TYPE = 4: Address ***************************
    if(rtype == 0x4) { // linear address record
      subs = buf.substr(9,4);
      subInt = strtol(subs.c_str(), 0, 16);
#ifdef LOCAL_DEBUG
      cout << "\tupper address = " << subInt << endl;
#endif
      extendAddress = subInt << 16;

    }


    // **************** RECORD TYPE = 1: EOF ***************************
    else if (rtype == 0x1) { //eof record
#ifdef LOCAL_DEBUG
      cout << "\tEOF record" << endl;
#endif
      break;
    }

    // **************** RECORD TYPE = 0: Data ***************************
    else if (rtype == 0x00) { // data record
      if ( ((address+len) < validh[0]) && (address >= validl[0])) {
	for (int i=0; i<len; i++) {
	  subs = buf.substr(9+2*i,2);
	  ivtBytes[(address - validl[0] + i)] = (unsigned char)strtol(subs.c_str(), 0, 16);
	}
      }
      else if ( ((address+len) < validh[1]) && (address >= validl[1])) {
	int pageIndex = (int)((address - validl[1])/0x800);
	pageEmpty[pageIndex] = 0;
	int rowIndex = (int)((address - validl[1])/0x100);
	rowEmpty[rowIndex] = 0;

	for (int i=0; i<len; i++) {
	  subs = buf.substr(9+2*i,2);
	  progBytes[(address - validl[1] + i)] = (unsigned char)strtol(subs.c_str(), 0, 16);
	}
      }

    }
      
      
    // **************** All others: invalid ***************************
    else // invalid record type
      cerr << ":  unexpected record type\n";
      
  }
      
  hexfile.close();

  // ******************** NOW START DOWNLOADING VALID DATA **************
  if (ivtBytes != (unsigned char *)NULL) { // a valid interrupt vector table range
    unsigned char *dataByte;
    int ivtRows;
    ivtRows = (int)((validh[0] + 0xFE - validl[0])/ 0x100); 
    // for now assume we are doing "real" ivt, which are contained in one page
    // and downloaded in two rows or four rows

    // first row (might be offset from page boundary)
    dataByte = ivtBytes;
    baseAddress = validl[0]/2;
    downloadbytes = 0x100 - (validl[0] & 0xff);
    eraseflag = ERASE_PRESERVE;

#ifdef LOCAL_DEBUG
    cout << "xwrite_mcu_block: baseAddress = " << hex << baseAddress
	 << " downloadbytes = " << dec << downloadbytes 
	 << " eraseflag = " << eraseflag << endl;
#endif
    xwrite_mcu_block(dataByte, baseAddress, downloadbytes, eraseflag, msgIDVal);


    // second row or third row
    // eraseflag = ERASE_NONE; // this won't work 
    // eraseflag needs to be ERASE_PRESERVE here, since the previous ERASE_PRESERVE was followed
    // by a ROW Write for the whole page, and thus this address needs to be erased again
    if (ivtRows > 2) {
      for (int row=1; row<(ivtRows-1); row++) {
	baseAddress += downloadbytes/2;
	dataByte += downloadbytes;
	downloadbytes = 0x100;
	     
#ifdef LOCAL_DEBUG
	cout << "xwrite_mcu_block: baseAddress = " << hex << baseAddress
	     << " downloadbytes = " << dec << downloadbytes 
	     << " eraseflag = " << eraseflag << endl;
#endif
	xwrite_mcu_block(dataByte, baseAddress, downloadbytes, eraseflag, msgIDVal);
      }
    }    

    // last row
    baseAddress += downloadbytes/2;
    dataByte += downloadbytes;
    downloadbytes = validh[0] - baseAddress*2;
#ifdef LOCAL_DEBUG
    cout << "xwrite_mcu_block: baseAddress = " << hex << baseAddress
	 << " downloadbytes = " << dec << downloadbytes 
	 << " eraseflag = " << eraseflag << endl;
#endif
    xwrite_mcu_block(dataByte, baseAddress, downloadbytes, eraseflag, msgIDVal);
  }


//   cout << endl;
//   unsigned char *ttt = progBytes;
//   ttt += 0x6800;
//   cout << hex;
//   for (int i=0; i<32; i++) {
//     for (int j=0; j<8; j++) 
//       cout << (unsigned int)(*ttt++) << " ";
//     cout << endl;
//   }
//   cout << endl;

  if ((validh[1] - validl[1]) > 2) { // a valid program data range
    unsigned char *dataByte;
    downloadbytes = 0x100; // 1 row
    eraseflag = ERASE_NORMAL;

    for (int page=0; page<numPages; page++) {
      dataByte = progBytes + 0x800*page;
      //cout << "pageEmpty[" << dec << page << "] = " << (int)pageEmpty[page] << endl;
      if(pageEmpty[page] == 0) {
	eraseflag = ERASE_NORMAL;
	baseAddress = validl[1]/2 + page * 0x400;
#ifdef LOCAL_DEBUG
	cout << "xwrite_mcu_block: baseAddress = " << hex << baseAddress
	     << " downloadbytes = " << dec << downloadbytes 
	     << " eraseflag = " << eraseflag << endl;
#endif
	xwrite_mcu_block(dataByte, baseAddress, downloadbytes, eraseflag, msgIDVal);
	
	eraseflag = 0;
	for (int row=1; row<8; row++) {
	  baseAddress += 0x80;
	  dataByte += 0x100;
	  if (rowEmpty[page*8 + row] == 0) {
#ifdef LOCAL_DEBUG
	    cout << "xwrite_mcu_block: baseAddress = " << hex << baseAddress
		 << " downloadbytes = " << dec << downloadbytes 
		 << " eraseflag = " << eraseflag << endl;
#endif
	    xwrite_mcu_block(dataByte, baseAddress, downloadbytes, eraseflag, msgIDVal);
	  }
	}	    
	
      }
    }

    
  }

  free((void *)ivtBytes);
  free((void *)progBytes);

  cout << "... Download of new MCU program successful!\n";

  errno = 0;
  
  return 0;
}

//******************************** main function *****************************************
int main(int argc, char *argv[])
{
  unsigned int tdigNodeID, tcpuNodeID;
  int devID = 0;


  cout << vcid << endl;
  cout.flush();

  if ( argc < 4 ) {
    cout << "USAGE: " << argv[0] << " <TDIG node ID> <TCPU node ID> <fileName> [<devID>]\n";
    return 1;
  }
  
  tdigNodeID = strtol(argv[1], (char **)NULL, 0);
//   if ((tdigNodeID < 1) || (tdigNodeID > 0x3F)) {
//     cerr << "TDIG nodeID = " << tdigNodeID 
// 	 << " is an invalid entry.  Use a value between 1 and 0x3F (63) instead."  
// 	 << endl;
//     return -1;
//   }
  
  tcpuNodeID = strtol(argv[2], (char **)NULL, 0);
//   if ((tcpuNodeID < 1) || (tcpuNodeID > 0x3F)) {
//     cerr << "TCPU nodeID = " << tcpuNodeID 
// 	 << " is an invalid entry.  Use a value between 1 and 0x3F (63) instead."  
// 	 << endl;
//     return -1;
//   }
  
  if (argc == 5) {
    devID = strtol(argv[4],(char **)NULL, 0);
    if (devID > 7) {
      printf("Invalid Device ID 0x%x. Use a device ID between 0 and 7\n", devID);
      return -1;
    }
  }

#ifdef LOCAL_DEBUG
  cout << "TDIG nodeID 0x" << hex << tdigNodeID
       << " TCPU nodeID 0x" << tcpuNodeID
       << " filename " << argv[2]
       << " devID " << dec << devID << endl;
#endif

  // start program memory on page boundary
  validl[1] &= 0xFFFFFC00;
  // end upper memory on page boundary
  validh[1] |= 0x3FF;
  // do everything in bytes rather than program addresses
  validl[0] *= 2;
  validl[1] *= 2;
  validh[0] = (validh[0]+1)*2;
  validh[1] = (validh[1]+1)*2;

  // install signal handler for manual break
  signal(SIGINT, signal_handler);
  

#ifndef NO_CAN
  if((h = CAN_Open(devID)) < 0) {
    my_private_exit(-1);
  }
#endif

  if (tcpuNodeID != 0xff)
    if (tdigNodeID == 0xff)
      for (unsigned int i=0x10; i<0x18; i++)
	change_mcu_program(argv[3], i, tcpuNodeID, devID);
    else
      change_mcu_program(argv[3], tdigNodeID, tcpuNodeID, devID);
  else {
    // for nodeID = 0xff, do all TCPUs serially
    vector<unsigned int> tcpuIDs;
    vector<unsigned int>::iterator it;
   
    int numTCPUs = findAllTCPUs(h, &tcpuIDs);
    if (numTCPUs > 0) {
      cout << "found " << numTCPUs << " TCPUs on this network. Starting...\n";
      for (it=tcpuIDs.begin(); it<tcpuIDs.end(); it++)
	if (tdigNodeID == 0xff)
	  for (unsigned int i=0x10; i<0x18; i++)
	    change_mcu_program(argv[3], i, tcpuNodeID, devID);
	else
	  change_mcu_program(argv[3], tdigNodeID, tcpuNodeID, devID);
    }
    else
      cout << "findAllTCPUs returned " << numTCPUs << ". Exiting...\n";
  }

  my_private_exit(errno);
  return 0;
}
