/* File name     : THUBchecksum.cc
 * Creation date : 9/17/2008
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: THUBchecksum.cc,v 1.1 2008-09-17 22:29:25 jschamba Exp $";
#endif /* lint */

/* 
 * Program to calculate the checksum of the firmware
 * contained in the hex file between given addresses
 */

// #define LOCAL_DEBUG

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

//****************************************************************************
// GLOBALS
ifstream hexfile;


//****************************************************************************
// LOCALS


//****************************************************************************
// CODE 

// centralized entry point for all exits
static void my_private_exit(int error)
{
  if (hexfile.is_open()) hexfile.close();
#ifdef LOCAL_DEBUG
  cout << "checksum_mcu_program: finished (" << error << ")\n";
#endif
  exit(error);
}



//**********************************************
// here all is done
int checksum_mcu_program(const char *filename, 
			 unsigned int startAddr,
			 unsigned int endAddr)
{
  string buf;
  unsigned int address;
  unsigned int extendAddress = 0;
  unsigned short checksum;
  unsigned short dataByte;
  
  // start address needs to be aligned by 2
  startAddr &= 0xFFFE;
  // end address needs to be an odd address
  endAddr |= 0x1;

  cout << "Checksumming MCU firmware with filename " << filename 
       << " from address " << showbase << hex << startAddr
       << " to " << endAddr
       << " ...\n";

  // "empty" program locations contain 0xff
  checksum = (endAddr - startAddr + 1) * 0xff;

  hexfile.open(filename); // "in" is default
  if ( !hexfile.good() ) {
    cerr << filename << ": file open error\n";
    return -1;
  }

  hexfile.seekg(0,ios::beg);    //move file pointer to beginning of file

  errno = 0;


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

    address = addr + extendAddress;


    
#ifdef LOCAL_DEBUG
    cout << "length = " << setw(2) << dec << len 
	 <<" type = " << rtype
	 << " address = " << showbase << hex << setw(6) << addr 
	 << " program address = " << setw(6) << address 
	 << " checksum = " << checksum
	 << endl;
#endif


    // **************** RECORD TYPE = 4: Address ***************************
    if(rtype == 0x4) { // linear address record
      subs = buf.substr(9,4);
      subInt = strtol(subs.c_str(), 0, 16);
#ifdef LOCAL_DEBUG
      cout << " upper address = " << subInt << endl;
#endif
      extendAddress = subInt << 16;

    }


    // **************** RECORD TYPE = 1: EOF ***************************
    else if (rtype == 0x1) { //eof record
#ifdef LOCAL_DEBUG
      cout << "EOF record";
      cout << endl;
#endif
      break;
    }

    // **************** RECORD TYPE = 0: Data ***************************
    else if (rtype == 0x00) { // data record

      if ((address >= startAddr) && (address <= endAddr) && (len != 0)) {
#ifdef LOCAL_DEBUG
	cout << " Data: ";
#endif
	// every word consists of 2 bytes
	for (int i=0; i<len; i+=2) {
	  if (address > endAddr) break;
	  // 1st
	  subs = buf.substr(9+2*i,2);
	  dataByte = (unsigned short)strtol(subs.c_str(), 0, 16);
#ifdef LOCAL_DEBUG
	  cout << " " << dataByte;
#endif
	  checksum += dataByte;
	  // 2nd
	  subs = buf.substr(9+2*i+2,2);
	  dataByte = (unsigned short)strtol(subs.c_str(), 0, 16);
#ifdef LOCAL_DEBUG
	  cout << " " << dataByte;
#endif
	  checksum += dataByte;

	  // subtract 2 "emtpy" (0xff) cells 
	  checksum -= 0x1fe; // 2 * 0xff
	  address += 2;
	}


#ifdef LOCAL_DEBUG
	cout << endl;
#endif
      }
    }
      
      
    // **************** All others: invalid ***************************
    else // invalid record type
      cerr << ":  unexpected record type\n";
      
  }
      
  hexfile.close();

  cout << "\nChecksum = " << showbase << hex << checksum << endl;

  errno = 0;
  my_private_exit(errno);
  
  return 0;
}

//****************************************************************************
int main(int argc, char *argv[])
{
  unsigned short startAddr, endAddr;
  cout << vcid << endl;
  cout.flush();

  if ( argc < 2 ) {
    cout << "USAGE: " << argv[0] << " <fileName> [<startAddress>] [<endAddress>]\n";
    return 1;
  }

  startAddr = 0x0000;
  endAddr = 0xFFFF;

  if (argc == 4) {
    endAddr = strtol(argv[3],(char **)NULL, 0);
  }

  if (argc > 2) {
    startAddr = strtol(argv[2],(char **)NULL, 0);
  }

  if (startAddr >= endAddr) {
    printf("endAddress must be > startAddress\n");
    return (-1);
  }
  
  
  return checksum_mcu_program(argv[1], startAddr, endAddr);
}
