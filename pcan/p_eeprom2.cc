/* File name     : p_eeprom2.cc
 * Creation date : 5/31/2007
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: p_eeprom2.cc,v 1.8 2011-10-31 14:49:41 jschamba Exp $";
#endif /* lint */


//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
#include <fstream>
using namespace std;

// other headers
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>


// pcan include file
#include <libpcan.h>

// local utilities
#include "can_utils.h"

//****************************************************************************
// DEFINES
//#define TDIG
#define TDIG_D
// #define LOCAL_DEBUG
#define LINE_UP "[1A[80D[0J"

//****************************************************************************
// GLOBALS
HANDLE h = NULL;

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
  cout << "p_eeprom2: finished (" << error << ")\n";
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
//*********************************************
int eeprom2(const char *filename, unsigned int nodeID, WORD devID)
{
  int i,j;

  ifstream conf;
  unsigned char confByte[256];
  int fileSize, noPages, leftover;

  cout << "Programming EEPROM2"
       << " at NodeID 0x" << hex << nodeID
       << " with filename " << filename 
       << " on CANbus ID 0x" << hex << devID << "...\n";

  errno = 0;
  
  // open file "filename" and extract control bits
  conf.open(filename, ios_base::binary); // "in" is default 
  if ( !conf.good() ) {
    cerr << filename << ": file error\n";
    return -1;
  }
  
  conf.seekg(0,ios::end);    //move file pointer to beginning of file
  fileSize = conf.tellg();
  noPages = fileSize/256;
  leftover = fileSize%256;

  if (leftover > 0) noPages++;

  cout << "Filesize = " << dec 
       << fileSize << " bytes, " << fileSize/1024 << " kbytes, "
       << noPages << " pages, leftover " << leftover << " bytes\n";
  
  conf.seekg(0,ios::beg);    // move file pointer to beginning of file


  cout << "Starting Configuration Procedure....\n";

  TPCANMsg ms;
  TPCANRdMsg mr;


  // swallow any packets that might be present first
  errno = LINUX_CAN_Read_Timeout(h, &mr, 100000); // timeout = 100 mseconds
  
  // ************** CONFIGURE_TDC:Write FPGA_CONFIG0 ****************************************
  ms.MSGTYPE = MSGTYPE_STANDARD;
  ms.ID = 0x002 | (nodeID<<4);
  ms.LEN = 1;
  
  // "FPGA_CONFIG0"
  ms.DATA[0] = 0x11;
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "p_eeprom2: Sending Write FPGA_CONFIG0 command:");
#endif
  
  if ( sendCAN_and_Compare(ms, "p_eeprom2: Write FPGA_CONFIG0", 500000, 2, true) != 0) // timeout = 0.5 sec
    printf("Old MCU Code?\n");
  


  // NOW START; Attempt to continue, even if error in previous CANbus message, since it
  // could be old MCU code
  for (int page=0; page<noPages; page++) {
  
    int eeprom_addrs = page*256;
    // read 256 bytes
    memset((void *)confByte, 0xff, 256);
    conf.read((char *)confByte, 256);

    // ************** CONFIGURE_TDC:Write Block Start ****************************************
    ms.MSGTYPE = MSGTYPE_STANDARD;
    ms.ID = 0x002 | (nodeID<<4);
    ms.LEN = 1;
    
    // "Write Block Start"
    ms.DATA[0] = 0x10;
    
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "p_eeprom2: Sending Write Block Start command:");
#endif
    
    if ( sendCAN_and_Compare(ms, "p_eeprom2: Write Block Start", 4000000, 2, true) != 0) // timeout = 4 sec
      my_private_exit(errno);
    
    
    // *************** "Write Block Data", 36 messages with 7 bytes each ***********
    ms.LEN = 8;
    
    unsigned char *tmpPtr = confByte;
    ms.DATA[0] = 0x20;
    for (i=0; i<36; i++) {
      for (j=1; j<8; j++) {
	ms.DATA[j] = *tmpPtr;
	tmpPtr++;
      }
      
#ifdef LOCAL_DEBUG
      printCANMsg(ms, "p_eeprom2: Sending Write Block Data packet:");
#endif
      
      
      if ( sendCAN_and_Compare(ms, "p_eeprom2: Write Block Data", 4000000, 2, true) != 0) // timeout = 4 sec
	my_private_exit(errno);
      
    } // end "for(i=1, ... " loop
    
    
    // **************** "Write Block Data", ... and 1 last message with 4 bytes ******
    ms.LEN = 5;
    
    for (j=1; j<5; j++) { 
      ms.DATA[j] = *tmpPtr;
      tmpPtr++;
    }
    
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "p_eeprom2: Sending Write Block Data packet 37:");
#endif
    
    
    if ( sendCAN_and_Compare(ms, "p_eeprom2: Write Block Data 37:", 4000000, 2, true) != 0) // timeout = 4 sec
      my_private_exit(errno);
    
    // *************************** Write Block End *************************
    ms.LEN = 1;
    ms.DATA[0] = 0x30;
    
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "p_eeprom2: Sending Write Block End packet:");
#endif
    
    
    if ( sendCAN_and_Compare(ms, "p_eeprom2: Write Block End:", 4000000, 8, true) != 0) // timeout = 4 sec
      my_private_exit(errno);
    
    
    // ****************************** "Write Block Target" **********************
    ms.LEN = 6;
    ms.DATA[0] = 0x4e;
    ms.DATA[1] = (unsigned char) eeprom_addrs & 0xFF;    // LSByte of address
    ms.DATA[2] = (unsigned char)(eeprom_addrs >>8) & 0xFF;    // 2nd Byte of address
    ms.DATA[3] = (unsigned char)(eeprom_addrs >>16) & 0xFF;   // 3rd Byte of address
    ms.DATA[4] = (unsigned char)(eeprom_addrs >>24) & 0xFF;   // MSByte of address
    // See if need to request "erase sector" before write (e.g. Sector Address is xx00xx)
    ms.DATA[5] = 0;     // Assume no erase needed on others
    if ( (eeprom_addrs & 0x00FF00) == 0) {
      ms.DATA[5] = 1;        // if erase needed on boundary
      // cout << " w/Erase";
    }
    
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "p_eeprom2: Sending Write Block Target packet:");
#endif
    
    if ( sendCAN_and_Compare(ms, "p_eeprom2: Write Block Target:", 4000000, 2, true) != 0) // timeout = 4 sec
      my_private_exit(errno);
    
    if(page<11) {cout << LINE_UP << "Page " << dec << page << "...\n"; flush(cout);}
    else if((page%100) == 0) {cout << LINE_UP << "Page " << dec << page << "...\n"; flush(cout);}
  } // for (int page=0 ...


  // ************************* finished !!!! ****************************************
  // ********************************************************************************



  // ************** CONFIGURE_TDC:Write FPGA_CONFIG1 ****************************************
  ms.MSGTYPE = MSGTYPE_STANDARD;
  ms.ID = 0x002 | (nodeID<<4);
  ms.LEN = 1;
  
  // "FPGA_CONFIG0"
  ms.DATA[0] = 0x12;
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "p_eeprom2: Sending Write FPGA_CONFIG0 command:");
#endif
  
  if ( sendCAN_and_Compare(ms, "p_eeprom2: Write FPGA_CONFIG0", 500000, 2, true) != 0) { // timeout = 0.5 sec
    printf("Old MCU code?\n");
  }


  cout << "... Configuration finished successfully.\n";
  fprintf(stderr, "successfully configured EEPROM2 TCPU 0x%x devID %d\n",
	  nodeID, devID); fflush(stderr);


  
  return errno;
}


//*********************************** main function **************************************
int main(int argc, char *argv[])
{
  WORD devID = 255;
  int retVal;

  cout << vcid << endl;
  cout.flush();
  
  if ( argc < 3 ) {
    cout << "USAGE: " << argv[0] << " <node ID> <fileName> [<devID>]\n";
    return 1;
  }
  
  unsigned int nodeID = strtol(argv[1], (char **)NULL, 0);

  if (argc == 4) {
    devID = strtol(argv[3],(char **)NULL, 0);
    if (devID > 255) {
      printf("Invalid Device ID 0x%x. Use a device ID between 0 and 255\n", devID);
      return -1;
    }
  }
  
  // install signal handler for manual break
  signal(SIGINT, signal_handler);

  if((errno = openCAN(devID)) != 0) {
    my_private_exit(errno);
  }

  if (nodeID != 0xff)
    retVal =  eeprom2(argv[2], nodeID, devID);
  else {
    // for nodeID = 0xff, do all TCPUs serially
    vector<unsigned int> tcpuIDs;
    vector<unsigned int>::iterator it;
   
    int numTCPUs = findAllTCPUs(&tcpuIDs);
    if (numTCPUs > 0) {
      cout << "found " << numTCPUs << " TCPUs on this network. Starting...\n";
      for (it=tcpuIDs.begin(); it<tcpuIDs.end(); it++)
	retVal =  eeprom2(argv[2], *it, devID);
    }
    else
      cout << "findAllTCPUs returned " << numTCPUs << ". Exiting...\n";
  }
  
  my_private_exit(errno);

  return 0;
}
