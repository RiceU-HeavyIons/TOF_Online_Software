/* File name     : progMCU.cc
 * Creation date : 7/2/2012
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: progMCU.cc 756 2012-07-05 20:44:12Z jschamba $";
#endif /* lint */

// #define LOCAL_DEBUG

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

//****************************************************************************
// LOCALS


//****************************************************************************
// CODE 

// centralized entry point for all exits
static void my_private_exit(int error)
{
  if (h > 0)
  {
    close(h); 
  }
  if (hexfile.is_open()) hexfile.close();
#ifdef LOCAL_DEBUG
  cout << "progMCU: finished (" << error << ")\n";
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
int send_64bytes(unsigned char *bytes,
		 const unsigned int startAddr,
		 const int checksum_64,
		 const unsigned int nodeID
		 )
{
  struct can_frame ms;
  struct can_frame mr;


  // ************** progMCU:WriteAddress ****************************************
  
  ms.can_id = 0x002 | (nodeID << 4);
  //ms.ID = 0x402;

  ms.data[0] = 0x21;	// write Address
  ms.data[1] = (startAddr & 0x0000FF);
  ms.data[2] = ((startAddr & 0x00FF00) >> 8);
  ms.data[3] = ((startAddr & 0xFF0000) >> 16);
  ms.can_dlc = 4;
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "progMCU: Sending progMCU:WriteAddress command:");
#endif
  
  if (write(h, &ms, sizeof(ms)) != sizeof(ms)) {
    perror("progMCU: write()");
    return -1;
  }
  errno = CAN_Read_Timeout(h, &mr, 1000000); // timeout = 1 second
  if (errno < 0) {
    cout << "CAN_Read_Timeout returned " << errno 
	 << " during progMCU:WriteAddress\n";
    return -1;
  }
  else if (errno == 0)
    cout << "Timeout during progMCU:WriteAddress\n";

  
  ms.data[0] = 0x22;
  ms.can_dlc = 8;
  unsigned char *tmpPtr = bytes;
  // send 9 packets with 7 bytes each = 63 bytes
  for (int i=0; i<9; i++) {
    // ************** progMCU:WriteDataBytes ************************************
    
    
    for (int j=1; j<8; j++) {
      ms.data[j] = *tmpPtr;
      tmpPtr++;
    }
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "progMCU: Sending progMCU:WriteDataBytes command:");
#endif
    
    // do this without response:
    if (write(h, &ms, sizeof(ms)) != sizeof(ms)) {
      perror("progMCU: write()");
      return -1;
    }
    // waste some time, so packets aren't sent too fast
    //nanosleep(&timesp, NULL);
    // for (int j=0; j<4300000; j++) ;
      
    errno = CAN_Read_Timeout(h, &mr, 1000000); // timeout = 1 second
    if (errno < 0) {
      cout << "CAN_Read_Timeout returned " << errno 
	   << " during progMCU:WriteDataBytes\n";
      return -1;
    }
    else if (errno == 0)
      cout << "Timeout during progMCU:WriteDataBytes\n";
    
      
  }
  
  // ************** progMCU:Program64 ****************************************
  
  ms.data[0] = 0x25;
  ms.data[1] = *tmpPtr;
  ms.can_dlc = 2;
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "progMCU: Sending progMCU:Program64 command:");
#endif
  
  if ( sendCAN_and_Compare(h, ms, "progMCU: progMCU:Program64", 1000000) != 0) { // timeout = 1 sec
    my_private_exit(errno);
  }
  
  return 0;
}


//**********************************************
// here all is done
int change_mcu_program(const char *filename, unsigned int nodeID, int devID)
{
  string buf;
  int checksum_64, final_checksum;
  unsigned int startAddr, endAddr;
  unsigned  char pgmByte[64];
  bool firstRecord;

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
  

  if((h = CAN_Open(devID)) < 0) {
    my_private_exit(-1);
  }

  // swallow any packets that might be present first
  struct can_frame mr;
  errno = CAN_Read_Timeout(h, &mr, 100000); // timeout = 100 mseconds

  firstRecord = true;
  while (hexfile.good()) {
    string subs;
    unsigned int subInt, addr, rtype;
    int len;
    unsigned char dataByte[64];

    
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
      if (firstRecord) {
	if(addr < 0x4000) { // make sure the lower address doesn't get overriden
	  cout << "Program begins below allowed program region at 0x4000\n"
	       << " Program first address is 0x" << hex << addr
	       << "quitting\n";
	  my_private_exit(-10);
	}
	startAddr = endAddr = addr;
	firstRecord = false;
      }
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



#ifdef START_NEW_FIRMWARE
  struct can_frame ms;

  // And jump to the new program:
  ms.can_id = 0x002 | (nodeID<<4);
  ms.can_dlc = 5;

  // "CHANGE_MCU_PROGRAM:Jump_PC"
  ms.data[0] = 0x26;
  ms.data[1] = 0xff; // last address in EEPROM (low byte)
  ms.data[2] = 0xff; // last address in EEPROM (hi byte)
  ms.data[3] = 0x0;  // data to write
  ms.data[4] = 0xa5; // yes, reset


#ifdef LOCAL_DEBUG
  printCANMsg(ms, "CHANGE_MCU_PROGRAM:Jump_PC, sent msg");
#endif

  errno = 0;
  sendCAN_and_Compare(h, ms, "CHANGE_MCU_PROGRAM:Jump_PC", 5000000); // timeout = 5 sec
  
  my_private_exit(errno);
#endif
  
  cout << "MCU programmed successfully!\n";
  return 0;
}

//****************************************************************************
int main(int argc, char *argv[])
{
  unsigned int nodeID;
  int devID = 0;


  cout << vcid << endl;
  cout.flush();

  if ( argc < 3 ) {
    cout << "USAGE: " << argv[0] << " <node ID> <fileName> [<devID>]\n";
    return 1;
  }
  
  nodeID = strtol(argv[1], (char **)NULL, 0);
  
  if (argc == 4) {
    devID = strtol(argv[3],(char **)NULL, 0);
    if (devID > 7) {
      printf("Invalid Device ID 0x%x. Use a device ID between 0 and 7\n", devID);
      return -1;
    }
  }
  cout << "nodeID 0x" << hex << nodeID
       << " filename " << argv[2]
       << " devID 0x" << devID << endl;
  return change_mcu_program(argv[2], nodeID, devID);
}
