/* File name     : can_utils.cc
 * Creation date : 6/26/2012
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: can_utils.cc 756 2012-07-05 20:44:12Z jschamba $";
#endif /* lint */

//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

// other headers
#include <stdio.h>
#include <unistd.h>


#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

//****************************************************************************
// DEFINES
// #define LOCAL_DEBUG
#define PRINT_COLOR
const char *RED_ON_WHITE = "\033[47m\033[1;31m";
const char *NORMAL_COLORS = "\033[0m";

//****************************************************************************
// GLOBALS

//****************************************************************************
// CODE 

//****************************************************************************
int CAN_Read_Timeout(int cansock, struct can_frame *frame, int nMicroSeconds)
{
  struct timeval t;
  fd_set fdRead;
  int nbytes;
  int err;

  // calculate timeout values
  t.tv_sec  = nMicroSeconds / 1000000L;
  t.tv_usec = nMicroSeconds % 1000000L;

  // wait until timeout or a message is ready to read
  FD_ZERO(&fdRead);
  FD_SET(cansock, &fdRead);

  err = select(cansock + 1, &fdRead, NULL, NULL, &t);	
    
  if (err  < 0) { // error
    perror("select");
    return -1;
  }
  else if (err == 0) { // timeout
    nbytes = 0;
  }
  else {
    // the only one file descriptor is ready for read
    if ((nbytes = read(cansock, frame, sizeof(struct can_frame))) != sizeof(struct can_frame)) {
      perror("read");
      return -2;
    }
  }

  return nbytes;
}

//****************************************************************************
int CAN_Open(int devID)
{
  int cansock;
  struct sockaddr_can addr;
  struct ifreq ifr;

  if ((cansock = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
    perror("socket");
    return -1;
  }

  addr.can_family = AF_CAN;
  
  sprintf(ifr.ifr_name, "can%d", devID);

  if (ioctl(cansock, SIOCGIFINDEX, &ifr) < 0) {
    perror("SIOCGIFINDEX");
    return -2;
  }
  addr.can_ifindex = ifr.ifr_ifindex;

  if (bind(cansock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    return -3;
  }

  return cansock;
}

//****************************************************************************
void printCANMsg(const struct can_frame &msg, const char *msgTxt)
{
  cerr << msgTxt << " "
       << ((msg.can_id & CAN_RTR_FLAG) ? "r " : "m ")
       << ((msg.can_id & CAN_EFF_FLAG) ? "e " : "s ")
       << showbase << hex << (msg.can_id & CAN_EFF_MASK) << " " 
       << dec << (int)msg.can_dlc << hex << noshowbase << setfill('0'); 
  for (int i = 0; i < msg.can_dlc; i++)
    cerr << " 0x" << setw(2) << (unsigned int)msg.data[i];
  cerr << endl;
}

//****************************************************************************
int sendCAN_and_Compare(int h, // socket
			struct can_frame &ms, const char *errorMsg, 
			const int timeout, unsigned int expectedReceiveLen,
			bool checkStatus)
{
  int errno;
  struct can_frame mr;
#ifdef LOCAL_DEBUG
  char msgTxt[256];
#endif
  unsigned int expectedID;

  // send the message
  if ( (errno = write(h, &ms, sizeof(struct can_frame))) != sizeof(struct can_frame) ) {
#ifdef PRINT_COLOR
    cerr << RED_ON_WHITE;
#endif
    perror("CAN write()");
#ifdef PRINT_COLOR
    cerr << NORMAL_COLORS;
#endif
    return(errno);
  }
  
  errno = CAN_Read_Timeout(h, &mr, timeout);
  if (errno > 0 ) { // data received
#ifdef LOCAL_DEBUG
    sprintf(msgTxt, "%s, message received", errorMsg);
    printCANMsg(mr, msgTxt);
#endif

    if (mr.can_id < CAN_ERR_FLAG) { // "standard" message
      // now interprete the received message:
      // check if it's a proper response
      expectedID = ms.can_id + 1;
      if ( mr.can_id != expectedID ) {
#ifdef PRINT_COLOR
	cerr << RED_ON_WHITE;
#endif
	cerr << "ERROR: " << errorMsg 
	     << " request: Got something other than writeResponse: ID " 
	     << showbase << hex << (unsigned int)mr.can_id 
	     << ", expected response to " << (unsigned int)ms.can_id << endl;	
	printCANMsg(mr, "response:");
#ifdef PRINT_COLOR
	cerr << NORMAL_COLORS;
#endif
	return (-2);
      }
      if (expectedReceiveLen == 0xffffffff) expectedReceiveLen = ms.can_dlc;
      if (mr.can_dlc != expectedReceiveLen) { // check for correct length
#ifdef PRINT_COLOR
	cerr << RED_ON_WHITE;
#endif
	cerr << "ERROR: " << errorMsg << " request: Got msg with incorrect data length " 
	     << dec << (int)mr.can_dlc << ", expected " << expectedReceiveLen << endl;
	cerr << errorMsg << " response: first byte: " 
	     << showbase << hex << (unsigned int)mr.data[0] 
	     << " expected " << (unsigned int)ms.data[0] << endl;
	printCANMsg(mr, "response");
#ifdef PRINT_COLOR
	cerr << NORMAL_COLORS;
#endif
	return (-3);
      }
      if (mr.data[0] != ms.data[0]) {
#ifdef PRINT_COLOR
	cerr << RED_ON_WHITE;
#endif
	cerr << errorMsg << " response: first byte: " 
	     << showbase << hex << (unsigned int)mr.data[0] 
	     << " expected " << (unsigned int)ms.data[0] << endl;
	printCANMsg(mr, "response:");
#ifdef PRINT_COLOR
	cerr << NORMAL_COLORS;
#endif
	return (-4);
      }
      // CAN HLP version 3:
      // The second payload byte of all "WRITE-reply" messages should be a
      // status. If that status is non-zero, then there was an error; and the
      // reason is indicated by the table in the Excel spreadsheet labeled:
      // "Status codes (payload[1])".  The possible values during the download
      // will be:
      // 0 = C_STATUS_OK == operation completed OK.
      // 2 = C_STATUS_NOSTART == Block-Data without Block-Start
      // 2 = C_STATUS_NOSTART == Block-End without Block-Start
      // 2 = C_STATUS_NOSTART == Block-Target without Block-Start AND Block-End
      // 3 = C_STATUS_OVERRUN == downloaded more than 256 bytes
      
      // During the Target EEPROM2 sequence values might be:
      // 0 = C_STATUS_OK == operation completed OK.
      // 6 = C_STATUS_LTHERR == downloaded block was not length 256 bytes
      // 8 = C_STATUS_BADEE2 == eeprom2 readback error (mismatched data readback)
      if (checkStatus) {
	if (mr.data[1] != 0) {
#ifdef PRINT_COLOR
	  cerr << RED_ON_WHITE;
#endif
	  cerr << errorMsg << " response: second (status) byte: " 
	       << showbase << hex << (unsigned int)mr.data[1] << endl; 
	  printCANMsg(mr, "response:");
#ifdef PRINT_COLOR
	  cerr << NORMAL_COLORS;
#endif
	  return (-8);
	}
      }

      // Message is good, continue
      
    } // data read
    else if ((mr.can_id & CAN_EFF_FLAG)  == CAN_EFF_FLAG ) {
      // now interprete the received message:
      // check if it's a proper response
      // the response is in the "standard" part
      //   of the extended Msg ID, i.e. 18 bits up
      expectedID = (ms.can_id & CAN_EFF_MASK) + (1<<18); 
      if ( (mr.can_id & CAN_EFF_MASK) != expectedID ) {
#ifdef PRINT_COLOR
	cerr << RED_ON_WHITE;
#endif
	cerr << "ERROR: " << errorMsg 
	     << " request: Got something other than writeResponse: ID " 
	     << showbase << hex << (unsigned int)(mr.can_id & CAN_EFF_MASK)
	     << ", expected response to " << (unsigned int)(ms.can_id & CAN_EFF_MASK) << endl;	
	printCANMsg(mr, "response:");
#ifdef PRINT_COLOR
	cerr << NORMAL_COLORS;
#endif
	return (-2);
      }
      if (expectedReceiveLen == 0xffffffff) expectedReceiveLen = ms.can_dlc;
      if (mr.can_dlc != expectedReceiveLen) { // check for correct length
#ifdef PRINT_COLOR
	cerr << RED_ON_WHITE;
#endif
	cerr << "ERROR: " << errorMsg << " request: Got msg with incorrect data length " 
	     << dec << (int)mr.can_dlc << ", expected " << expectedReceiveLen << endl;
	cerr << errorMsg << " response: first byte: " 
	     << showbase << hex << (unsigned int)mr.data[0] 
	     << " expected " << (unsigned int)ms.data[0] << endl;
	printCANMsg(mr, "response");
#ifdef PRINT_COLOR
	cerr << NORMAL_COLORS;
#endif
	return (-3);
      }
      if (mr.data[0] != ms.data[0]) {
#ifdef PRINT_COLOR
	cerr << RED_ON_WHITE;
#endif
	cerr << errorMsg << " response: first byte: " 
	     << showbase << hex << (unsigned int)mr.data[0] 
	     << " expected " << (unsigned int)ms.data[0] << endl;
	printCANMsg(mr, "response:");
#ifdef PRINT_COLOR
	cerr << NORMAL_COLORS;
#endif
	return (-4);
      }
      // CAN HLP version 3:
      // The second payload byte of all "WRITE-reply" messages should be a
      // status. If that status is non-zero, then there was an error; and the
      // reason is indicated by the table in the Excel spreadsheet labeled:
      // "Status codes (payload[1])".  The possible values during the download
      // will be:
      // 0 = C_STATUS_OK == operation completed OK.
      // 2 = C_STATUS_NOSTART == Block-Data without Block-Start
      // 2 = C_STATUS_NOSTART == Block-End without Block-Start
      // 2 = C_STATUS_NOSTART == Block-Target without Block-Start AND Block-End
      // 3 = C_STATUS_OVERRUN == downloaded more than 256 bytes
      
      // During the Target EEPROM2 sequence values might be:
      // 0 = C_STATUS_OK == operation completed OK.
      // 6 = C_STATUS_LTHERR == downloaded block was not length 256 bytes
      // 8 = C_STATUS_BADEE2 == eeprom2 readback error (mismatched data readback)
      if (checkStatus) {
	if (mr.data[1] != 0) {
#ifdef PRINT_COLOR
	  cerr << RED_ON_WHITE;
#endif
	  cerr << errorMsg << " response: second (status) byte: " 
	       << showbase << hex << (unsigned int)mr.data[1] << endl; 
	  printCANMsg(mr, "response:");
#ifdef PRINT_COLOR
	  cerr << NORMAL_COLORS;
#endif
	  return (-8);
	}
      }

      // Message is good, continue
      
    } // data read
  } // data received
  else if (errno == 0) {	
#ifdef PRINT_COLOR
    cerr << RED_ON_WHITE;
#endif
    cerr << "ERROR: Sent " << errorMsg << " packet, but did not receive response within "
	 << dec << timeout/1000000 << " sec" << endl;
#ifdef PRINT_COLOR
	cerr << NORMAL_COLORS;
#endif
    return (-5);
  }
  else {// other read error
#ifdef PRINT_COLOR
    cerr << RED_ON_WHITE;
#endif
    cerr << "ERROR: " << errorMsg 
	 << ": LINUX_CAN_Read_Timeout returned " << errno << endl;
#ifdef PRINT_COLOR
	cerr << NORMAL_COLORS;
#endif
    return (-6);
  }

  return 0;
}


int findAllTCPUs(int h, vector<unsigned int> *pTcpuIDs) 
{
#ifdef LOCAL_DEBUG
  char txt[255]; // temporary string storage
#endif
  int errno = 0;
  struct can_frame m;
  struct can_frame mr;
  unsigned int tcpuID;
  int numTCPUs;
  
  if (h > 0) {
    // swallow all existing messages first
    while (errno == 0)
      errno = CAN_Read_Timeout(h, &mr, 1000); // timeout = 1ms
  }
  else {
#ifdef LOCAL_DEBUG
    cerr << "CAN handle not valid. Exiting...\n";
#endif
    return -1;
  }
    
  // send a broadcast message to all TCPUs
  m.can_id = 0x7f4;
  m.can_dlc = 1;
  m.data[0] = 0xb4;
#ifdef LOCAL_DEBUG
  printCANMsg(m, "message assembled:");
#endif
  if (write(h, &m, sizeof(m)) != sizeof(m)) {
#ifdef LOCAL_DEBUG
    perror("write()");
#endif
    return -2;
  }

  numTCPUs = 0;
  // now read all received messages
  while (errno > 0) {
    errno = CAN_Read_Timeout(h, &mr, 10000); // timeout = 10ms
    
    if (errno > 0) { // data received
      
#ifdef LOCAL_DEBUG
	printCANMsg(mr, "message received: ");
#endif
      
	if ((mr.can_id & 0x600) == 0x200) {
	  numTCPUs++;
	  tcpuID = (mr.can_id >> 4) & 0x3f;
#ifdef LOCAL_DEBUG
	  cout << "TCPU ID = " << showbase << tcpuID << endl;
#endif
	  pTcpuIDs->push_back(tcpuID);
	}
    } // end "data received"
  } // while (errno = 0)
  
  return numTCPUs;
}

