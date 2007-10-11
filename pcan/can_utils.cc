/* File name     : can_utils.cc
 * Creation date : 5/11/2007
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: can_utils.cc,v 1.5 2007-10-11 14:17:51 jschamba Exp $";
#endif /* lint */

// #define LOCAL_DEBUG

//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
using namespace std;

// other headers
#include <errno.h>
#include <fcntl.h>

// pcan include file
#include <libpcan.h>

//****************************************************************************
// GLOBALS
extern HANDLE h;

//****************************************************************************
// LOCALS


//****************************************************************************
// CODE 


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
int openCAN(WORD devID)
{
  char devName[255];
  TPDIAG my_PDiag;
  char txt[255]; // temporary string storage

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
    printf("Device ID 0x%x not found\n", devID);
    errno = nGetLastError();
    perror("CAN_Open()");
    return(errno);
  }

    
  // get version info
  errno = CAN_VersionInfo(h, txt);
  if (!errno) {
#ifdef LOCAL_DEBUG
    cout << "driver version = "<< txt << endl;
#endif
  }
  else {
    perror("CAN_VersionInfo()");
    return(errno);
  }

  // open CAN Port, init PCAN-USB
  //errno = CAN_Init(h, CAN_BAUD_1M,  CAN_INIT_TYPE_ST);
  errno = CAN_Init(h, CAN_BAUD_1M,  CAN_INIT_TYPE_EX); // open for Extended messages
  if (errno) {
    perror("CAN_Init()");
    return(errno);
  } 

  return (0);
}

//****************************************************************************
int sendCAN_and_Compare(TPCANMsg &ms, const char *errorMsg, 
			const int timeout, unsigned int expectedReceiveLen,
			bool checkStatus)
{
  TPCANRdMsg mr;
  __u32 status;
#ifdef LOCAL_DEBUG
  char msgTxt[256];
#endif
  unsigned int expectedID;

  // send the message
  if ( (errno = CAN_Write(h, &ms)) ) {
    perror("CAN_Write()");
    return(errno);
  }
  
  errno = LINUX_CAN_Read_Timeout(h, &mr, timeout);
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
	perror("CAN_Status()");
	return(errno);
      }
      else {
	cout << "ERROR: " << errorMsg 
	     << ": pending CAN status " << showbase << hex << status << " read.\n";
	return (-7);
      }
    } 
    else if (mr.Msg.MSGTYPE == MSGTYPE_STANDARD) {
      // now interprete the received message:
      // check if it's a proper response
      expectedID = ms.ID + 1;
      if ( mr.Msg.ID != expectedID ) {
	cout << "ERROR: " << errorMsg 
	     << " request: Got something other than writeResponse: ID " 
	     << showbase << hex << (unsigned int)mr.Msg.ID 
	     << ", expected response to " << (unsigned int)ms.ID << endl;	
	printCANMsg(mr.Msg, "response:");
	return (-2);
      }
      if (expectedReceiveLen == 0xffffffff) expectedReceiveLen = ms.LEN;
      if (mr.Msg.LEN != expectedReceiveLen) { // check for correct length
	cout << "ERROR: " << errorMsg << " request: Got msg with incorrect data length " 
	     << dec << (int)mr.Msg.LEN << ", expected " << expectedReceiveLen << endl;
	cout << errorMsg << " response: first byte: " 
	     << showbase << hex << (unsigned int)mr.Msg.DATA[0] 
	     << " expected " << (unsigned int)ms.DATA[0] << endl;
	printCANMsg(mr.Msg, "response");
	return (-3);
      }
      if (mr.Msg.DATA[0] != ms.DATA[0]) {
	cout << errorMsg << " response: first byte: " 
	     << showbase << hex << (unsigned int)mr.Msg.DATA[0] 
	     << " expected " << (unsigned int)ms.DATA[0] << endl;
	printCANMsg(mr.Msg, "response:");
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
	if (mr.Msg.DATA[1] != 0) {
	  cout << errorMsg << " response: second (status) byte: " 
	       << showbase << hex << (unsigned int)mr.Msg.DATA[1] << endl; 
	  printCANMsg(mr.Msg, "response:");
	  return (-8);
	}
      }

      // Message is good, continue
      
    } // data read
    else if (mr.Msg.MSGTYPE == MSGTYPE_EXTENDED) {
      // now interprete the received message:
      // check if it's a proper response
      // the response is in the "standard" part
      //   of the extended Msg ID, i.e. 18 bits up
      expectedID = ms.ID + (1<<18); 
      if ( mr.Msg.ID != expectedID ) {
	cout << "ERROR: " << errorMsg 
	     << " request: Got something other than writeResponse: ID " 
	     << showbase << hex << (unsigned int)mr.Msg.ID 
	     << ", expected response to " << (unsigned int)ms.ID << endl;	
	printCANMsg(mr.Msg, "response:");
	return (-2);
      }
      if (expectedReceiveLen == 0xffffffff) expectedReceiveLen = ms.LEN;
      if (mr.Msg.LEN != expectedReceiveLen) { // check for correct length
	cout << "ERROR: " << errorMsg << " request: Got msg with incorrect data length " 
	     << dec << (int)mr.Msg.LEN << ", expected " << expectedReceiveLen << endl;
	cout << errorMsg << " response: first byte: " 
	     << showbase << hex << (unsigned int)mr.Msg.DATA[0] 
	     << " expected " << (unsigned int)ms.DATA[0] << endl;
	printCANMsg(mr.Msg, "response");
	return (-3);
      }
      if (mr.Msg.DATA[0] != ms.DATA[0]) {
	cout << errorMsg << " response: first byte: " 
	     << showbase << hex << (unsigned int)mr.Msg.DATA[0] 
	     << " expected " << (unsigned int)ms.DATA[0] << endl;
	printCANMsg(mr.Msg, "response:");
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
	if (mr.Msg.DATA[1] != 0) {
	  cout << errorMsg << " response: second (status) byte: " 
	       << showbase << hex << (unsigned int)mr.Msg.DATA[0] << endl; 
	  printCANMsg(mr.Msg, "response:");
	  return (-8);
	}
      }

      // Message is good, continue
      
    } // data read
  } // data received
  else if (errno == CAN_ERR_QRCVEMPTY) {	
    cout << "ERROR: Sent " << errorMsg << " packet, but did not receive response within "
	 << dec << timeout/1000000 << " sec" << endl;
    return (-5);
  }
  else {// other read error
    cout << "ERROR: " << errorMsg 
	 << ": LINUX_CAN_Read_Timeout returned " << errno << endl;
    return (-6);
  }

  return 0;
}


