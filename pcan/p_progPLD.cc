/* File name     : p_progPLD.cc
 * Creation date : 8/17/06
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: p_progPLD.cc,v 1.2 2007-04-03 21:54:21 jschamba Exp $";
#endif /* lint */

// #define LOCAL_DEBUG

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
#include <time.h>

//****************************************************************************
// DEFINES
#define TDIG

//****************************************************************************
// GLOBALS
HANDLE h = NULL;


//****************************************************************************
// LOCALS

//****************************************************************************

//****************************************************************************
// CODE 
//****************************************************************************

// centralized entry point for all exits
static void my_private_exit(int error)
{
  if (h)
  {
    CAN_Close(h); 
  }
#ifdef LOCAL_DEBUG
  cout << "p_progPLD: finished (" << error << ")\n";
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
  TPCANRdMsg mr;
  __u32 status;
#ifdef LOCAL_DEBUG
  char msgTxt[256];
#endif
  
  // send the message
  if ( (errno = CAN_Write(h, &ms)) ) {
    perror("p_progPLD: CAN_Write()");
    return(errno);
  }
  
  errno = LINUX_CAN_Read_Timeout(h, &mr, 60000000); // timeout = 60 seconds
  if (errno == 0 ) { // data received
#ifdef LOCAL_DEBUG
    sprintf(msgTxt, "%s, message received", errorMsg);
    printCANMsg(mr.Msg, msgTxt);
#endif
    // check if a CAN status is pending	     
    if (mr.Msg.MSGTYPE & MSGTYPE_STATUS) {
      status = CAN_Status(h);
      if ((int)status < 0) {
	errno = nGetLastError();
	perror("p_progPLD: CAN_Status()");
	return(errno);
      }
      else
	cout << "p_progPLD: pending CAN status " << showbase << hex << status << " read.\n";
    } 
    else if (mr.Msg.MSGTYPE == MSGTYPE_STANDARD) {
      // now interprete the received message:
      // check if it's a proper response
      if ( mr.Msg.ID != 0x403 ) {
	cout << "p_progPLD request: Got something other than writeResponse: ID " 
	     << showbase << hex << (unsigned int)mr.Msg.ID 
	     << ", expected response to " << (unsigned int)ms.ID << endl;	
	printCANMsg(mr.Msg, "p_progPLD: response:");
	return (-2);
      }
      if (mr.Msg.LEN != ms.LEN) { // check for correct length
	cout << "ERROR: " << errorMsg << " request: Got msg with incorrect data length " 
	     << dec << (int)mr.Msg.LEN << ", expected " << (int)ms.LEN << endl;
	cout << errorMsg << " response: first byte: " 
	     << showbase << hex << (unsigned int)mr.Msg.DATA[0] 
	     << " expected " << (unsigned int)ms.DATA[0] << endl;
	printCANMsg(mr.Msg, "p_progPLD: response:");
	return (-3);
      }
      if (mr.Msg.DATA[0] != ms.DATA[0]) {
	cout << errorMsg << " response: first byte: " 
	     << showbase << hex << (unsigned int)mr.Msg.DATA[0] 
	     << " expected " << (unsigned int)ms.DATA[0] << endl;
	printCANMsg(mr.Msg, "p_progPLD: response:");
	return (-4);
      }
      // Message is good, continue
      
      
    } // data read
  } // data received
  else if (errno == CAN_ERR_QRCVEMPTY) {	
    cout << "ERROR: Sent " << errorMsg << " packet, but did not receive response within 60 sec" << endl;
    return (-5);
  }
  else {// other read error
    cout << "LINUX_CAN_Read_Timeout returned " << errno << endl;
    return (-6);
  }

  return 0;
}


int p_progPLD(const char *filename, int pldNum)
{
  char txt[255]; // temporary string storage
  ifstream conf;
  unsigned char confByte[256];
  int fileSize, noPages;

  TPDIAG my_PDiag;
  char devName[255];
  WORD devID = 255;

  struct timespec timesp;

  cout << "Configuring PLD with filename " << filename << "...\n";

  errno = 0;

  // install signal handler for manual break
  signal(SIGINT, signal_handler);

  conf.open(filename, ios_base::binary); // "in" is default 
  if ( !conf.good() ) {
    cerr << filename << ": file error\n";
    return -1;
  }
  
  conf.seekg(0,ios::end);
  fileSize = conf.tellg();
  noPages = fileSize/256;

  timesp.tv_sec = 0;
  timesp.tv_nsec = 1000;	// 1 ms

  cout << "Filesize = " << fileSize << " bytes, " << fileSize/1024 << " kbytes, "
       << noPages << " pages\n";

  conf.seekg(0,ios::beg);    // move file pointer to beginning of file


  // search for correct device ID:
  for (int i=0; i<8; i++) {
    sprintf(devName, "/dev/pcan%d", 32+i);
    h = LINUX_CAN_Open(devName, O_RDWR);
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
    perror("p_progPLD: CAN_Open()");
    my_private_exit(errno);
  }

    
  // get version info
  errno = CAN_VersionInfo(h, txt);
  if (!errno) {
#ifdef LOCAL_DEBUG
    cout << "p_progPLD: driver version = "<< txt << endl;
#endif
  }
  else {
    perror("p_progPLD: CAN_VersionInfo()");
    my_private_exit(errno);
  }

  // open CAN Port for TDIG, init PCAN-USB
  errno = CAN_Init(h, CAN_BAUD_1M,  CAN_INIT_TYPE_ST);
  if (errno) {
    perror("p_progPLD: CAN_Init()");
    my_private_exit(errno);
  } 
  
  cout << "Starting Configuration Procedure....\n";

  TPCANMsg ms;
  TPCANRdMsg mr;

  // swallow any packets that might be present first
  errno = LINUX_CAN_Read_Timeout(h, &mr, 100000); // timeout = 100 mseconds

  // ***************************************************************************

  // ************** progPLD:Start ****************************************

  ms.MSGTYPE = CAN_INIT_TYPE_ST;
  ms.ID = 0x0402;
  ms.LEN = 2;

  ms.DATA[0] = 0x20;
  ms.DATA[1] = pldNum;
  //ms.DATA[1] = 0x02;	// program FPGA B (2)

  cout << "Starting Bulk Erase...\n";
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "p_progPLD: Sending progPLD:Start command:");
#endif

  if ( sendCAN_and_Compare(ms, "p_progPLD: progPLD:Start") != 0)
    my_private_exit(errno);

  cout << "Bulk Erase finished...\nStarting page programming...\n";
  cout.flush();
  sleep(2);
  for (int page=0; page<noPages/2; page++) {
    int EPCS_Address = page*256;
    // read 256 bytes
    conf.read((char *)confByte, 256);
    
    bool allFF = true;
    for (int i=0; i<256; i++) {
      if(confByte[i] != 0xff) {
	allFF = false;
	break;
      }
    }

    if (allFF) continue;

    // ************** progPLD:WriteAddress ****************************************

    ms.DATA[0] = 0x21;	// write Address
    ms.DATA[1] = ((EPCS_Address & 0xFF0000) >> 16);
    ms.DATA[2] = ((EPCS_Address & 0x00FF00) >> 8);
    ms.DATA[3] = (EPCS_Address & 0x0000FF);
    ms.LEN = 4;
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "p_progPLD: Sending progPLD:WriteAddress command:");
#endif

    // do this without response:
    if ( (errno = CAN_Write(h, &ms)) ) {
      perror("p_progPLD: CAN_Write()");
      return(errno);
    }
    errno = LINUX_CAN_Read_Timeout(h, &mr, 1000000); // timeout = 1 second
    if (errno != 0) {
      if (errno == CAN_ERR_QRCVEMPTY)
	cout << "Timeout during progPLD:WriteAddress, page " << page << endl;
      else
	cout << "CAN_Read_Timeout returned " << errno 
	     << " during progPLD:WriteAddress page, " << page << endl;
    }
    

    ms.DATA[0] = 0x22;
    ms.LEN = 8;
    unsigned char *tmpPtr = confByte;
    for (int i=0; i<36; i++) {
      // ************** progPLD:WriteDataBytes ************************************


      for (int j=1; j<8; j++) {
	ms.DATA[j] = *tmpPtr;
	tmpPtr++;
      }
#ifdef LOCAL_DEBUG
      printCANMsg(ms, "p_progPLD: Sending progPLD:WriteDataBytes command:");
#endif

      // do this without response:
      if ( (errno = CAN_Write(h, &ms)) ) {
      	perror("p_progPLD: CAN_Write()");
      	return(errno);
      }
      // waste some time, so packets aren't sent too fast
      //nanosleep(&timesp, NULL);
      for (int j=0; j<4300000; j++) ;

      /*
      errno = LINUX_CAN_Read_Timeout(h, &mr, 1000000); // timeout = 1 second
      if (errno != 0) {
	if (errno == CAN_ERR_QRCVEMPTY)
	  cout << "Timeout during progPLD:WriteDataBytes, page " << page << endl;
	else
	  cout << "CAN_Read_Timeout returned " << errno 
	       << " during progPLD:WriteDataBytes, page " << page << endl;
      }
      */
      
    }
    
    // ************** progPLD:Program256 ****************************************
    
    ms.DATA[0] = 0x23;
    ms.LEN = 5;
    for (int j=1; j<5; j++) {
      ms.DATA[j] = *tmpPtr;
      tmpPtr++;
    }
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "p_progPLD: Sending progPLD:Program256 command:");
#endif
    
    if ( sendCAN_and_Compare(ms, "p_progPLD: progPLD:Program256") != 0) {
      cout << "page = " << dec << page << endl;
      my_private_exit(errno);
    }
    
    //#ifdef LOCAL_DEBUG
    if(page<11) cout << "Page " << dec << page << "...\n";
    else if((page%100) == 0) cout << "Page " << dec << page << "...\n";
    //#endif


  } // for (int page
  
  cout << "Page Programming finished...\nSending asDone...\n";
  ms.DATA[0] = 0x24;
  ms.LEN = 1;
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "p_progPLD: Sending progPLD:asDone command:");
#endif

    if ( sendCAN_and_Compare(ms, "p_progPLD: progPLD:asDone") != 0)
      my_private_exit(errno);



    conf.close();

  // ************************* finished !!!! ****************************************
  // ********************************************************************************

  cout << "... Configuration finished successfully.\n";


  my_private_exit(errno);

  return errno;
}


int main(int argc, char *argv[])
{
  cout << vcid << endl;
  cout.flush();
  
  if ( argc != 3 ) {
    cout << "USAGE: " << argv[0] << " <fileName>, <PLD #>\n";
    return 1;
  }
  
  int pldNum = atoi(argv[2]);
  
  
  return p_progPLD(argv[1], pldNum);
}
