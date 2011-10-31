/* File name     : can_utils.cc
 * Creation date : 5/11/2007
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: can_utils.cc,v 1.15 2011-10-31 14:25:42 jschamba Exp $";
#endif /* lint */

// #define LOCAL_DEBUG
#define PRINT_COLOR

#define FIFO_FILE       "/tmp/pcanfifo"
#define FIFO_RESPONSE "/tmp/pcanfifoRsp"
const char *RED_ON_WHITE = "\033[47m\033[1;31m";
const char *NORMAL_COLORS = "\033[0m";

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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>

// pcan include file
#include <libpcan.h>

//****************************************************************************
// GLOBALS
extern HANDLE h;
FILE *_sendFifoFp;
int _recvFifoFd;
struct pollfd _pfd;

//****************************************************************************
// LOCALS


//****************************************************************************
// CODE 


//****************************************************************************
void printCANMsg(const TPCANMsg &msg, const char *msgTxt)
{
  cerr << msgTxt << " "
       << ((msg.MSGTYPE & MSGTYPE_RTR) ? "r " : "m ")
       << ((msg.MSGTYPE & MSGTYPE_EXTENDED) ? "e " : "s ")
       << showbase << hex << msg.ID << " " 
       << dec << (int)msg.LEN << hex << noshowbase << setfill('0'); 
  for (int i = 0; i < msg.LEN; i++)
    cerr << " 0x" << setw(2) << (unsigned int)msg.DATA[i];
  cerr << endl;
}

void check_err(__u32  err,  char *txtbuff)
{
#define CAN_ERR_HWINUSE   0x0400  // Hardware ist von Netz belegt
#define CAN_ERR_NETINUSE  0x0800  // an Netz ist Client angeschlossen
#define CAN_ERR_ILLHW     0x1400  // Hardwarehandle war ungueltig
#define CAN_ERR_ILLNET    0x1800  // Netzhandle war ungueltig
#define CAN_ERR_ILLCLIENT 0x1C00  // Clienthandle war ungueltig

  strcpy(txtbuff, "Error bits: ") ;
  if ( err == CAN_ERR_OK  )		strcat(txtbuff, "OK ") ;		// 0x0000  // no error
  if ( err & CAN_ERR_XMTFULL )		strcat(txtbuff, "XMTFULL ") ;        	// 0x0001  // transmit buffer full
  if ( err & CAN_ERR_OVERRUN  )		strcat(txtbuff, "OVERRUN ") ;       	// 0x0002  // overrun in receive buffer
  if ( err & CAN_ERR_BUSLIGHT )		strcat(txtbuff, " BUSLIGHT ") ;      	// 0x0004  // bus error, errorcounter limit reached
  if ( err & CAN_ERR_BUSHEAVY )		strcat(txtbuff, "BUSHEAVY ") ;       	// 0x0008  // bus error, errorcounter limit reached
  if ( err & CAN_ERR_BUSOFF  )		strcat(txtbuff, "BUSOFF ") ;        	// 0x0010  // bus error, 'bus off' state entered
  if ( err & CAN_ERR_QRCVEMPTY )	strcat(txtbuff, "QRCVEMPTY ") ;      	// 0x0020  // receive queue is empty
  if ( err & CAN_ERR_QOVERRUN )		strcat(txtbuff, "QOVERRUN") ;    	// 0x0040  // receive queue overrun
  if ( err & CAN_ERR_QXMTFULL )		strcat(txtbuff, "QXMTFULL ") ;       	// 0x0080  // transmit queue full 
  if ( err & CAN_ERR_REGTEST )		strcat(txtbuff, "REGTEST ") ;        	// 0x0100  // test of controller registers failed
  if ( err & CAN_ERR_NOVXD )        	strcat(txtbuff, "NOVXD  ") ;         	// 0x0200  // Win95/98/ME only
  if ( err & CAN_ERR_RESOURCE )        	strcat(txtbuff, "RESOURCE  ") ;      	// 0x2000  // can't create resource
  if ( err & CAN_ERR_ILLPARAMTYPE )	strcat(txtbuff, "ILLPARAMTYPE  ") ;  	// 0x4000  // illegal parameter
  if ( err & CAN_ERR_ILLPARAMVAL )     	strcat(txtbuff, "ILLPARAMVAL ") ;    	// 0x8000  // value out of range

  if ( (err & CAN_ERRMASK_ILLHANDLE) == CAN_ERR_HWINUSE ) 	strcat(txtbuff, "HWINUSE ") ;
  if ( (err & CAN_ERRMASK_ILLHANDLE) == CAN_ERR_NETINUSE ) 	strcat(txtbuff, "NETINUSE ") ;
  if ( (err & CAN_ERRMASK_ILLHANDLE) == CAN_ERR_ILLHW )		strcat(txtbuff, "ILLHW ") ;
  if ( (err & CAN_ERRMASK_ILLHANDLE) == CAN_ERR_ILLCLIENT )	strcat(txtbuff, "ILLCLIENT ") ;
  if ( (err & CAN_ERRMASK_ILLHANDLE) == CAN_ERR_ILLNET ) 	strcat(txtbuff, "ILLNET ") ;

  return;
}

//****************************************************************************
int openCAN(WORD devID)
{
  char devName[255];
//   TPDIAG my_PDiag;
  char txt[255]; // temporary string storage
  int nFileHandle;
  TPEXTRAPARAMS params;

  // search for correct device ID:
  for (int i=0; i<8; i++) {
    //sprintf(devName, "/dev/pcan%d", 32+i);
    sprintf(devName, "/dev/pcanusb%d", i);
    h = LINUX_CAN_Open(devName, O_RDWR);
    if (h == NULL) {
      //printf("Failed to open device %s\n", devName);
      //my_private_exit(errno);
      continue;
    }
//     // get the hardware ID from the diag structure:
//     LINUX_CAN_Statistics(h,&my_PDiag);
//     printf("\tDevice at %s: Hardware ID = 0x%x\n", devName, 
// 	   my_PDiag.wIrqLevel);
//     if (my_PDiag.wIrqLevel == devID) break;

    // get the hardware ID from the special ioctl call
    nFileHandle = LINUX_CAN_FileHandle(h);
    params.nSubFunction = SF_GET_HCDEVICENO;
    errno = ioctl(nFileHandle, PCAN_EXTRA_PARAMS, &params);
    printf("\tDevice at %s: Hardware ID = 0x%x\n", devName, 
	   params.func.ucHCDeviceNo);
    if (params.func.ucHCDeviceNo == devID) break;

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
  //errno = CAN_Init(h, CAN_BAUD_1M,  CAN_INIT_TYPE_EX); // open for Extended messages
  errno = CAN_Init(h, CAN_BAUD_500K,  CAN_INIT_TYPE_EX); // open for Extended messages
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
#ifdef PRINT_COLOR
    cerr << RED_ON_WHITE;
#endif
    perror("CAN_Write()");
#ifdef PRINT_COLOR
    cerr << NORMAL_COLORS;
#endif
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
#ifdef PRINT_COLOR
	cerr << RED_ON_WHITE;
#endif
	perror("CAN_Status()");
#ifdef PRINT_COLOR
	cerr << NORMAL_COLORS;
#endif
	return(errno);
      }
      else {
#ifdef PRINT_COLOR
	cerr << RED_ON_WHITE;
#endif
	cout << "ERROR: " << errorMsg 
	     << ": pending CAN status " << showbase << hex << status << " read.\n";
#ifdef PRINT_COLOR
	cerr << NORMAL_COLORS;
#endif
	return (-7);
      }
    } 
    else if (mr.Msg.MSGTYPE == MSGTYPE_STANDARD) {
      // now interprete the received message:
      // check if it's a proper response
      expectedID = ms.ID + 1;
      if ( mr.Msg.ID != expectedID ) {
#ifdef PRINT_COLOR
	cerr << RED_ON_WHITE;
#endif
	cerr << "ERROR: " << errorMsg 
	     << " request: Got something other than writeResponse: ID " 
	     << showbase << hex << (unsigned int)mr.Msg.ID 
	     << ", expected response to " << (unsigned int)ms.ID << endl;	
	printCANMsg(mr.Msg, "response:");
#ifdef PRINT_COLOR
	cerr << NORMAL_COLORS;
#endif
	return (-2);
      }
      if (expectedReceiveLen == 0xffffffff) expectedReceiveLen = ms.LEN;
      if (mr.Msg.LEN != expectedReceiveLen) { // check for correct length
#ifdef PRINT_COLOR
	cerr << RED_ON_WHITE;
#endif
	cerr << "ERROR: " << errorMsg << " request: Got msg with incorrect data length " 
	     << dec << (int)mr.Msg.LEN << ", expected " << expectedReceiveLen << endl;
	cerr << errorMsg << " response: first byte: " 
	     << showbase << hex << (unsigned int)mr.Msg.DATA[0] 
	     << " expected " << (unsigned int)ms.DATA[0] << endl;
	printCANMsg(mr.Msg, "response");
#ifdef PRINT_COLOR
	cerr << NORMAL_COLORS;
#endif
	return (-3);
      }
      if (mr.Msg.DATA[0] != ms.DATA[0]) {
#ifdef PRINT_COLOR
	cerr << RED_ON_WHITE;
#endif
	cerr << errorMsg << " response: first byte: " 
	     << showbase << hex << (unsigned int)mr.Msg.DATA[0] 
	     << " expected " << (unsigned int)ms.DATA[0] << endl;
	printCANMsg(mr.Msg, "response:");
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
	if (mr.Msg.DATA[1] != 0) {
#ifdef PRINT_COLOR
	  cerr << RED_ON_WHITE;
#endif
	  cerr << errorMsg << " response: second (status) byte: " 
	       << showbase << hex << (unsigned int)mr.Msg.DATA[1] << endl; 
	  printCANMsg(mr.Msg, "response:");
#ifdef PRINT_COLOR
	  cerr << NORMAL_COLORS;
#endif
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
#ifdef PRINT_COLOR
	cerr << RED_ON_WHITE;
#endif
	cerr << "ERROR: " << errorMsg 
	     << " request: Got something other than writeResponse: ID " 
	     << showbase << hex << (unsigned int)mr.Msg.ID 
	     << ", expected response to " << (unsigned int)ms.ID << endl;	
	printCANMsg(mr.Msg, "response:");
#ifdef PRINT_COLOR
	cerr << NORMAL_COLORS;
#endif
	return (-2);
      }
      if (expectedReceiveLen == 0xffffffff) expectedReceiveLen = ms.LEN;
      if (mr.Msg.LEN != expectedReceiveLen) { // check for correct length
#ifdef PRINT_COLOR
	cerr << RED_ON_WHITE;
#endif
	cerr << "ERROR: " << errorMsg << " request: Got msg with incorrect data length " 
	     << dec << (int)mr.Msg.LEN << ", expected " << expectedReceiveLen << endl;
	cerr << errorMsg << " response: first byte: " 
	     << showbase << hex << (unsigned int)mr.Msg.DATA[0] 
	     << " expected " << (unsigned int)ms.DATA[0] << endl;
	printCANMsg(mr.Msg, "response");
#ifdef PRINT_COLOR
	cerr << NORMAL_COLORS;
#endif
	return (-3);
      }
      if (mr.Msg.DATA[0] != ms.DATA[0]) {
#ifdef PRINT_COLOR
	cerr << RED_ON_WHITE;
#endif
	cerr << errorMsg << " response: first byte: " 
	     << showbase << hex << (unsigned int)mr.Msg.DATA[0] 
	     << " expected " << (unsigned int)ms.DATA[0] << endl;
	printCANMsg(mr.Msg, "response:");
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
	if (mr.Msg.DATA[1] != 0) {
#ifdef PRINT_COLOR
	  cerr << RED_ON_WHITE;
#endif
	  cerr << errorMsg << " response: second (status) byte: " 
	       << showbase << hex << (unsigned int)mr.Msg.DATA[1] << endl; 
	  printCANMsg(mr.Msg, "response:");
#ifdef PRINT_COLOR
	  cerr << NORMAL_COLORS;
#endif
	  return (-8);
	}
      }

      // Message is good, continue
      
    } // data read
  } // data received
  else if (errno == CAN_ERR_QRCVEMPTY) {	
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

//****************************************************************************
int openCAN_l(WORD devID)
{

#ifdef LOCAL_DEBUG
  printf("open write FIFO\n");
#endif
  if((_sendFifoFp = fopen(FIFO_FILE, "w")) == NULL) {
    perror("fopen");
    exit(1);
  }
  
  /* this blocks until the other side is open for Write */
#ifdef LOCAL_DEBUG
  printf("open FIFO\n");
#endif
  if((_recvFifoFd = open(FIFO_RESPONSE, O_RDONLY)) == -1) {
    perror("open response FIFO");
    exit(1);
  }

  _pfd.fd = _recvFifoFd;
  _pfd.events = POLLIN;
  
  // notify pcanloop that we want response messages
#ifdef LOCAL_DEBUG
  printf("sending w command\n");
#endif
  fputs("w", _sendFifoFp); fflush(_sendFifoFp);
  // read dummy response
  int dummy;
  int numRead = read(_recvFifoFd, &dummy, 4);
  if(numRead < 0) {
    perror("dummy read");
    printf("didn't get connection response\n");
    exit(1);
  }

  h = (HANDLE)1; // some dummy value not null

  return (0);
}

DWORD CAN_Close_l(HANDLE hHandle)
{
#ifdef LOCAL_DEBUG
  printf("sending W command\n");
#endif

  fputs("W", _sendFifoFp); fflush(_sendFifoFp);
  fclose(_sendFifoFp);
  close(_recvFifoFd);

  return(0);
}

DWORD CAN_Write_l(HANDLE hHandle, TPCANMsg* pMsgBuff, int devID) 
{
  string cmdString;
  stringstream ss;

  ss.str("");
  ss << ((pMsgBuff->MSGTYPE & MSGTYPE_RTR) ? "r " : "m ");
  ss << ((pMsgBuff->MSGTYPE & MSGTYPE_EXTENDED) ? "e " : "s ");
  ss << hex << showbase << pMsgBuff->ID << " ";
  ss << dec << (int)(pMsgBuff->LEN) << " " << hex << showbase;
  for (int i = 0; i < pMsgBuff->LEN; i++)
    ss << (unsigned int)(pMsgBuff->DATA[i]) << " ";
  ss << dec << devID;

  cmdString = ss.str();
  
#ifdef LOCAL_DEBUG
  cout << "CAN_Write_l: cmdString = " << cmdString << endl;
#endif
  
  fputs(cmdString.c_str(), _sendFifoFp); fflush(_sendFifoFp);

  return 0;
}

//****************************************************************************
DWORD LINUX_CAN_Read_l(HANDLE hHandle, TPCANRdMsg* pMsgBuff)
{
#ifdef LOCAL_DEBUG
  cout << "LINUX_CAN_Read_l: reading response, sizeof(TPCANMsg) = "
       << dec << sizeof(TPCANMsg)
       << endl;
#endif
  int numRead = read(_recvFifoFd, 
		     &(pMsgBuff->Msg), 
		     sizeof(TPCANMsg));
  if(numRead < 0) perror("read");
    
    
#ifdef LOCAL_DEBUG
  cout << "number of bytes read " << dec << numRead << endl;
#endif
  return 0;
}

//****************************************************************************
DWORD LINUX_CAN_Read_Timeout_l(HANDLE hHandle, 
			     TPCANRdMsg* pMsgBuff, 
			     int nMicroSeconds)
{
  int nMilliSeconds = nMicroSeconds/1000;

#ifdef LOCAL_DEBUG
  cout << "LINUX_CAN_Read_Timeout_l: reading response, sizeof(TPCANMsg) = "
       << dec << sizeof(TPCANMsg)
       << ", timeout = " 
       << nMilliSeconds << endl;
#endif
    errno = poll(&_pfd, 1, nMilliSeconds);
    if (errno != 1) {
#ifdef LOCAL_DEBUG
      cout << "LINUX_CAN_Read_Timeout_l: poll returned " << errno << endl;
#endif
      return (errno-1);
    }
      
    int numRead = read(_recvFifoFd, 
		       &(pMsgBuff->Msg), 
		       sizeof(TPCANMsg));
    if(numRead < 0) perror("read");
    
    
#ifdef LOCAL_DEBUG
    cout << "number of bytes read " << dec << numRead << endl;
#endif

  return 0;
}
//****************************************************************************
int sendCAN_and_Compare_l(int devID, TPCANMsg &ms, const char *errorMsg, 
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
  if ( (errno = CAN_Write_l(h, &ms, devID)) ) {
    perror("CAN_Write()");
    return(errno);
  }
  
  errno = LINUX_CAN_Read_Timeout_l(h, &mr, timeout);
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

int findAllTCPUs(vector<unsigned int> *pTcpuIDs) 
{
#ifdef LOCAL_DEBUG
  char txt[255]; // temporary string storage
#endif
  TPCANMsg m;
  TPCANRdMsg mr;
  __u32 status;
  unsigned int tcpuID;
  int numTCPUs;
  
  if (h != NULL) {
    // swallow all existing messages first
    while (errno == 0)
      errno = LINUX_CAN_Read_Timeout(h, &mr, 1000); // timeout = 1ms
  }
  else {
#ifdef LOCAL_DEBUG
    cerr << "CAN handle not valid. Exiting...\n";
#endif
    return -1;
  }
    
  // send a broadcast message to all TCPUs
  m.MSGTYPE = MSGTYPE_STANDARD;
  m.ID = 0x7f4;
  m.LEN = 1;
  m.DATA[0] = 0xb4;
#ifdef LOCAL_DEBUG
  printCANMsg(m, "checkCan: message assembled:");
#endif
  if ( (errno = CAN_Write(h, &m)) ) {
#ifdef LOCAL_DEBUG
    perror("pcanloop: CAN_Write()");
#endif
    return -2;
  }

  numTCPUs = 0;
  // now read all received messages
  while (errno == 0) {
    errno = LINUX_CAN_Read_Timeout(h, &mr, 10000); // timeout = 10ms
    
    if (errno == 0) { // data received
      
      // check if a CAN status is pending	     
      if (mr.Msg.MSGTYPE & MSGTYPE_STATUS) {
	status = CAN_Status(h);
#ifdef LOCAL_DEBUG
	cout << "received status " << hex << showbase << status << endl;
#endif
	if ((int)status < 0) {
	  errno = nGetLastError();
#ifdef LOCAL_DEBUG
	  perror("pcanloop: CAN_Status()");
#endif
	  return -3;
	}
#ifdef LOCAL_DEBUG
	else {
	  check_err(status, txt);
	  cerr << txt << endl;
	}
	cerr << "Status message received, breaking out of loop\n";
#endif	
	break;
      } 
      else if ((mr.Msg.MSGTYPE == MSGTYPE_STANDARD) || (mr.Msg.MSGTYPE == MSGTYPE_EXTENDED)) {
#ifdef LOCAL_DEBUG
	printCANMsg(mr.Msg, "checkCAN: message received: ");
#endif
      
	if ((mr.Msg.ID & 0x600) == 0x200) {
	  numTCPUs++;
	  tcpuID = (mr.Msg.ID >> 4) & 0x3f;
#ifdef LOCAL_DEBUG
	  cout << "TCPU ID = " << showbase << tcpuID << endl;
#endif
	  pTcpuIDs->push_back(tcpuID);
	}
      } // else if ((mr.Msg.MSGTYPE
    } // end "data received"
  } // while (errno = 0)

  return numTCPUs;
}
