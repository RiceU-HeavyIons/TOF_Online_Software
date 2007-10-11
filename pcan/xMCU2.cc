/* File name     : xMCU2.cc
 * Creation date : 10/11/2007
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: xMCU2.cc,v 1.1 2007-10-11 19:08:44 jschamba Exp $";
#endif /* lint */

/* 
 * This program is used to re-program the MCU on TDIG
 * and TCPU according to Bill Burton's instructions
 */

#define LOCAL_DEBUG

//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

// other headers
#include <errno.h>
#include <unistd.h>
#include <signal.h>

// pcan include file
#include <libpcan.h>

// local utilities
#include "can_utils.h"

//****************************************************************************
// GLOBALS
HANDLE h = NULL;
ifstream hexfile;

const int ERASE_NONE = 0;
const int ERASE_NORMAL = 1;
const int ERASE_PRESERVE = 2;

unsigned int validl[] = {          0x100,       0x4000};  // this tells the lowest valid address
unsigned int validh[] = {          0x200,       0x8FFF};  // this tells the highest valid address
int   erasetype[] = { ERASE_PRESERVE, ERASE_NORMAL};  // this tells the kind of "erase" flag

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



//****************************************************************************
int address_in_range (unsigned int address, int numlimits, unsigned int *limitl, unsigned int *limith) {
/* this routine determines whether the address is within any of the numlimits ranges specified by:
 * limitl[]<= address <= limith[]
 * returns 0 if not in any region;
 * else returns the number of the region (index +1)
 */
    for (int i=0; i<numlimits; i++) {
        if ((address >= *(limitl+i)) && (address <= *(limith+i))) return (i+1);
    }
    return (0);     // wasn't in range
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
  TPCANMsg ms;
  TPCANRdMsg mr;

  if (bytecount != 0) {

    checksum = 0;
    // ************** MCU2:WriteBlockStart ****************************************
  
    ms.MSGTYPE = MSGTYPE_EXTENDED;
    ms.ID = msgIDVal;
    ms.LEN = 1;
    ms.DATA[0] = 0x10;	// Block Start
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "MCU2: Sending BlockStart command:");
#endif
    
    if ( sendCAN_and_Compare(ms, "MCU2:BlockStart", 4000000, 2, true) != 0) { // timeout = 4 sec
      my_private_exit(errno);
    }

    
    unsigned char *tmpPtr = bytes;
    int numFullMsgs = bytecount/7;
    int remainingToSend = bytecount%7;

    ms.DATA[0] = 0x20;	// Block Data
    ms.LEN = 8;
    // send "numFullMsgs" packets with 7 bytes each
    for (int i=0; i<numFullMsgs; i++) {
      // ************** MCU2:BlockData ************************************
      for (int j=1; j<8; j++) {
	ms.DATA[j] = *tmpPtr;
	checksum += *tmpPtr;
	tmpPtr++;
      }
#ifdef LOCAL_DEBUG
      printCANMsg(ms, "MCU2: Sending BlockData command:");
#endif
      
      if ( sendCAN_and_Compare(ms, "MCU2:BlockData", 4000000, 2, true) != 0) { // timeout = 4 sec
	my_private_exit(errno);
      }
      
    }

    // Now send the remainingToSend bytes in an extra packet:
    if (remainingToSend > 0) {
      ms.LEN = remainingToSend+1;
      for (int j=1; j<(remainingToSend+1); j++) {
	ms.DATA[j] = *tmpPtr;
	checksum += *tmpPtr;
	tmpPtr++;
      }
#ifdef LOCAL_DEBUG
      printCANMsg(ms, "MCU2: Sending BlockData command:");
#endif
      if ( sendCAN_and_Compare(ms, "MCU2:BlockData", 4000000, 2, true) != 0) { // timeout = 4 sec
	my_private_exit(errno);
      }

    }
    
    // ************** MCU2:BlockEnd ****************************************
    
    ms.DATA[0] = 0x30;	// Block End
    ms.LEN = 1;
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "MCU2: Sending BlockEnd command:");
#endif
    
    if ( (errno = CAN_Write(h, &ms)) ) {
      perror("MCU2:BlockEnd: CAN_Write()");
      return(errno);
    }
    errno = LINUX_CAN_Read_Timeout(h, &mr, 4000000); // timeout = 4 second
    if (errno != 0) {
      if (errno == CAN_ERR_QRCVEMPTY)
	cout << "Timeout during MCU2:BlockEnd\n";
      else
	cout << "CAN_Read_Timeout returned " << errno 
	     << " during MCU2:BlockEnd \n";
    }

    unsigned int expectedResponseID = ms.ID | (0x1 <<18);
    // Now check and compare
    if ( mr.Msg.ID != expectedResponseID ) {
      cout << "ERROR: BlockEnd Command " 
	   << " request: Got something other than writeResponse: ID " 
	   << showbase << hex << (unsigned int)mr.Msg.ID 
	   << ", expected response to " << (unsigned int)ms.ID << endl;	
      printCANMsg(mr.Msg, "response:");
      my_private_exit(-2);
    }

    if (mr.Msg.LEN != 8) { // check for correct length
      cout << "ERROR: BlockEnd Command" 
	   << " request: Got msg with incorrect data length " 
	   << dec << (int)mr.Msg.LEN << ", expected 8\n";
      cout << "BlockEnd Command response: first byte: " 
	   << showbase << hex << (unsigned int)mr.Msg.DATA[0] 
	   << " expected " << (unsigned int)ms.DATA[0] << endl;
      printCANMsg(mr.Msg, "response");
      my_private_exit(-3);
    }
    if (mr.Msg.DATA[0] != ms.DATA[0]) { 	// check for correct echo
      cout << "ERROR: BlockEnd Command response: first byte: " 
	   << showbase << hex << (unsigned int)mr.Msg.DATA[0] 
	   << " expected " << (unsigned int)ms.DATA[0] << endl;
      printCANMsg(mr.Msg, "response:");
      my_private_exit(-4);
    }
    if (mr.Msg.DATA[1] != 0) {	// check for correct status
      cout << "ERROR: BlockEnd Command response: second (status) byte: " 
	   << showbase << hex << (unsigned int)mr.Msg.DATA[0] << endl; 
      printCANMsg(mr.Msg, "response:");
      my_private_exit(-8);
    }

    int receivedByteCount = (int)(mr.Msg.DATA[3] << 8) + (int)(mr.Msg.DATA[2]);
    if (receivedByteCount != bytecount) { 	// check for correct returned byte count
      cout << "ERROR: BlockEnd Command: Replied block length " << dec << receivedByteCount
	   << " not " << bytecount << endl;
      printCANMsg(mr.Msg, "response:");
      my_private_exit(-9);
    }      

    unsigned int lwork;
    memcpy ((unsigned char *)&lwork, (unsigned char *)&(mr.Msg.DATA[4]), 4); // checksum
    if (lwork != checksum) {
      cout << "ERROR: BlockEnd Command: Replied checksum " << showbase << hex << lwork
	   << "; expected " << checksum << endl;
      printCANMsg(mr.Msg, "response:");
      my_private_exit(-10);
    }      
    
    // ************** MCU2:BlockTargetMCU2 ****************************************
    ms.LEN = 6;
    ms.DATA[0] = 0x4c;	// Block Target MCU2
    ms.DATA[1] = (unsigned char)(startAddr & 0xff);
    ms.DATA[2] = (unsigned char)((startAddr >>  8) & 0xff);
    ms.DATA[3] = (unsigned char)((startAddr >> 16) & 0xff);
    ms.DATA[4] = (unsigned char)((startAddr >> 24) & 0xff);
    ms.DATA[5] = ERASE_NONE;	// assume no erase needed on others
    if (eraseflag == ERASE_PRESERVE)
      ms.DATA[5] = ERASE_PRESERVE;	// preserve unprogrammed data
    else if ((startAddr & 0x003ff) == 0)
      ms.DATA[5] = ERASE_NORMAL;	// erase needed on boundary

#ifdef LOCAL_DEBUG
    printCANMsg(ms, "MCU2: Sending BlockTargetMCU2 command:");
#endif
    if ( sendCAN_and_Compare(ms, "MCU2:BlockTargetMCU2", 4000000, 2, true) != 0) { // timeout = 4 sec
      my_private_exit(errno);
    }

  } // if (bytecount != 0)
  
  return 0;
}


//**********************************************
// here all is done
int change_mcu_program(const char *filename, 
		       unsigned int tdigNodeID, 
		       unsigned int tcpuNodeID, 
		       WORD devID)
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
       << " at devID 0x" << hex << devID << "...\n";


  unsigned int msgIDVal = (((tdigNodeID<<4) | 0x002) << 18) | tcpuNodeID;
 
  hexfile.open(filename); // "in" is default
  if ( !hexfile.good() ) {
    cerr << filename << ": file open error\n";
    return -1;
  }

  hexfile.seekg(0,ios::beg);    //move file pointer to beginning of file

  errno = 0;

  // install signal handler for manual break
  signal(SIGINT, signal_handler);
  

  if((errno = openCAN(devID)) != 0) {
    my_private_exit(errno);
  }

  
  // swallow any packets that might be present first
  TPCANRdMsg mr;
  errno = LINUX_CAN_Read_Timeout(h, &mr, 100000); // timeout = 100 mseconds

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

    address = (addr + extendAddress) / 2;


    
#ifdef LOCAL_DEBUG
    cout << "length = " << setw(2) << dec << len 
	 <<" type = " << rtype
	 << " address = " << showbase << hex << setw(6) << addr << endl;
#endif


    // **************** RECORD TYPE = 4: Address ***************************
    if(rtype == 0x4) { // linear address record
      subs = buf.substr(9,4);
      subInt = strtol(subs.c_str(), 0, 16);
#ifdef LOCAL_DEBUG
      cout << " upper address = " << subInt << endl;
#endif
      extendAddress = subInt << 16;

      if (downloadbytes != 0) {
	xwrite_mcu_block(dataByte, baseAddress, downloadbytes, eraseflag, msgIDVal);
	baseAddress += downloadbytes;
	downloadbytes = 0;
      }
    }


    // **************** RECORD TYPE = 1: EOF ***************************
    else if (rtype == 0x1) { //eof record
#ifdef LOCAL_DEBUG
      cout << endl;
#endif
      break;
    }

    // **************** RECORD TYPE = 0: Data ***************************
    else if (rtype == 0x00) { // data record
      if (downloadbytes == 0)
	baseAddress = address;
      
      int indexAddr = address_in_range(address, 2, validl, validh);
      if ((len != 0) &&
	  (indexAddr != 0) &&
	  (address >= baseAddress) &&
	  (address < (baseAddress + 256)) ) {
	
	eraseflag = erasetype[indexAddr-1];
	// read the data bytes:
	for (int i=0; i<len; i++) {
	  subs = buf.substr(9+2*i,2);
	  dataByte[downloadbytes] = (unsigned char)strtol(subs.c_str(), 0, 16);
	  downloadbytes++;
	  if (downloadbytes == 256) { // if buffer is full
	    xwrite_mcu_block(dataByte, baseAddress, downloadbytes, eraseflag, msgIDVal);
	    baseAddress += downloadbytes;
	    downloadbytes = 0;
	  }
	}
      }
    }
      
      
    // **************** All others: invalid ***************************
    else // invalid record type
      cerr << ":  unexpected record type\n";
      
  }
      
  hexfile.close();
  cout << "... Download of new MCU program successful!\n";

  errno = 0;
  my_private_exit(errno);
  
  return 0;
}

//****************************************************************************
int main(int argc, char *argv[])
{
  unsigned int tdigNodeID, tcpuNodeID;
  WORD devID = 255;


  cout << vcid << endl;
  cout.flush();

  if ( argc < 4 ) {
    cout << "USAGE: " << argv[0] << " <TDIG node ID> <TCPU node ID> <fileName> [<devID>]\n";
    return 1;
  }
  
  tdigNodeID = strtol(argv[1], (char **)NULL, 0);
  if ((tdigNodeID < 1) || (tdigNodeID > 0x3F)) {
    cerr << "TDIG nodeID = " << tdigNodeID 
	 << " is an invalid entry.  Use a value between 1 and 0x3F (63) instead."  
	 << endl;
    return -1;
  }
  
  tcpuNodeID = strtol(argv[2], (char **)NULL, 0);
  if ((tcpuNodeID < 1) || (tcpuNodeID > 0x3F)) {
    cerr << "TCPU nodeID = " << tcpuNodeID 
	 << " is an invalid entry.  Use a value between 1 and 0x3F (63) instead."  
	 << endl;
    return -1;
  }
  
  if (argc == 5) {
    devID = strtol(argv[4],(char **)NULL, 0);
    if (devID > 255) {
      printf("Invalid Device ID 0x%x. Use a device ID between 0 and 255\n", devID);
      return -1;
    }
  }

#ifdef LOCAL_DEBUG
  cout << "TDIG nodeID 0x" << hex << tdigNodeID
       << " TCPU nodeID 0x" << tcpuNodeID
       << " filename " << argv[2]
       << " devID 0x" << devID << endl;
#endif

  return change_mcu_program(argv[3], tdigNodeID, tcpuNodeID, devID);
}
