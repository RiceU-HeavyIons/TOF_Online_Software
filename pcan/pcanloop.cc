/* File name     : pcanloop.cxx
 * Creation date : 9/10/03
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: pcanloop.cc,v 1.21 2008-01-22 20:47:50 jschamba Exp $";
#endif /* lint */


//****************************************************************************
// INCLUDES
#include <iostream>
//#include <fstream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <libpcan.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>

//****************************************************************************
// DEFINES
#define FIFO_FILE "/tmp/pcanfifo"
#define FIFO_RESPONSE "/tmp/pcanfifoRsp"

//****************************************************************************
// GLOBALS
HANDLE h = NULL;
HANDLE hI[256] = {NULL};
FILE *fifofp = (FILE *)NULL;
int respFifoFd = -1;
int dummyFd;

#define LOCAL_STRING_LEN 64       // length of internal used strings
typedef struct
{
  char szVersionString[LOCAL_STRING_LEN];
  char szDevicePath[LOCAL_STRING_LEN];
  int  nFileNo;
} PCAN_DESCRIPTOR;

//****************************************************************************
// LOCALS

//****************************************************************************
// CODE 

void check_err(__u32  err,  char *txtbuff)
{
#define CAN_ERR_HWINUSE   0x0400  // Hardware ist von Netz belegt
#define CAN_ERR_NETINUSE  0x0800  // an Netz ist Client angeschlossen
#define CAN_ERR_ILLHW     0x1400  // Hardwarehandle war ungueltig
#define CAN_ERR_ILLNET    0x1800  // Netzhandle war ungueltig
#define CAN_ERR_ILLCLIENT 0x1C00  // Clienthandle war ungueltig

  strcpy(txtbuff, "Error: ") ;
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

// centralized entry point for all exits
static void my_private_exit(int error)
{
//   if (h)
//   {
//     printf("Closing pcan\n");
//     CAN_Close(h); 
//   }
  for (int i=0; i<256; i++) {
    if (hI[i] != NULL) {
      printf("Closing pcan %d\n", i);
      CAN_Close(hI[i]);
    }
  }
  printf("pcanloop: finished (%d).\n\n", error);fflush(stdout);
  if (fifofp != (FILE *)NULL)
    fclose(fifofp);
  unlink(FIFO_FILE);

  close(respFifoFd);
  close(dummyFd);
  unlink(FIFO_RESPONSE);
  exit(error);
}

// handles CTRL-C user interrupt
static void signal_handler(int signal)
{
  my_private_exit(0);
}

//**********************************************
// here all is done
int main(int argc, char *argv[])
{
  //int nType = HW_USB;
  //__u32 dwPort = 0;
  //__u16 wIrq = 0;
  //__u16 wBTR0BTR1 = CAN_BAUD_250K;
  __u16 wBTR0BTR1 = CAN_BAUD_1M;
  // __u16 wBTR0BTR1 = CAN_BAUD_500K;
  //int   nExtended = CAN_INIT_TYPE_ST;
  int  nExtended = CAN_INIT_TYPE_EX; // open for "extended" message IDs
  int  iteration = 0;
  char txt[255]; // temporary string storage
  bool saveit = false;
  bool doWriteId = false;
  bool doTimedSave = false;
  bool filterit = false;
  bool printReceived = true;
  bool writeResponse = false;
  bool addTime = false;
  FILE *fp = NULL;
  unsigned int buffer[2];
  unsigned char *uc_ptr =  (unsigned char *)buffer;
  int fifofd;
  TPDIAG my_PDiag;
  WORD devID;
  char devName[255];
  // timing variables
  int total_time = 0;
  int start_time = 0;
  int curr_time = 0;
  int elapsed_time = 0;
  time_t *theTime = new time_t;
  int numEvents = 0;

  int openHandles[256] = {0};
  int currentIndex = 0;

  errno = 0;
  devID = 255;

  if (argc >1) {
    devID = atoi(argv[1]);
    if (devID > 255) {
      printf("Invalid Device ID 0x%x. Use a device ID between 0 and 255\n", devID);fflush(stdout);
      return 1;
    }
  }

  // give some information back
  if (wBTR0BTR1)
    printf("Trying to open PCAN devices with BTR0BTR1=0x%04x\n", wBTR0BTR1);
  else
    printf("Trying to open PCAN devices with 500 kbit/sec.\n");
  fflush(stdout);
  
  // install signal handler for manual break
  signal(SIGINT, signal_handler);
  
  // open the CAN port
  for (int i=0; i<8; i++) {
    sprintf(devName, "/dev/pcan%d", 32+i);
    h = LINUX_CAN_Open(devName, O_RDWR|O_NONBLOCK);
    if (h == NULL) {
      continue;
    }
    // get the hardware ID from the diag structure:
    LINUX_CAN_Statistics(h,&my_PDiag);
    printf("\tDevice at %s: Hardware ID = 0x%x\n", devName, 
	   my_PDiag.wIrqLevel); fflush(stdout);

    hI[my_PDiag.wIrqLevel] = h;
    openHandles[currentIndex++] = my_PDiag.wIrqLevel;

    // init to a user defined bit rate
    if (wBTR0BTR1) {
      errno = CAN_Init(h, wBTR0BTR1, nExtended);
      if (errno) {
	perror("pcanloop: CAN_Init()");
	my_private_exit(errno);
      }
    }
    //if (my_PDiag.wIrqLevel == devID) break;
    //CAN_Close(h);
  }

  if (currentIndex == 0) {
    printf("No devices found, exiting\n");
    errno = nGetLastError();
    perror("pcanloop: CAN_Open()");
    fflush(stdout);
    my_private_exit(errno);
  }

  currentIndex = 0;

  // get version info
  errno = CAN_VersionInfo(hI[openHandles[0]], txt);
  if (!errno)
    printf("pcanloop: driver version = %s\n", txt);
  else {
    perror("pcanloop: CAN_VersionInfo()");
    my_private_exit(errno);
  }
  

  // create or open control FIFO
  umask(0);
  mkfifo(FIFO_FILE, 0666);
  mkfifo(FIFO_RESPONSE, 0666);
  fifofd = open(FIFO_FILE, O_RDONLY | O_NONBLOCK);
  fifofp = fdopen(fifofd, "r");
  

  dummyFd = open(FIFO_RESPONSE, O_RDONLY | O_NONBLOCK); 
  respFifoFd = open(FIFO_RESPONSE, O_WRONLY); 
  if(respFifoFd == -1) {
    perror("open response FIFO");
    my_private_exit(errno);
  }
  
  // read in endless loop until Ctrl-C
  while (1) {	//**************** big while loop here ************************
    TPCANMsg m;
    TPCANRdMsg mr;
    __u32 status;
    int i;
    int parse_input_message(char *, TPCANMsg *, WORD *);
    
    iteration++;
    //printf("Iteration %d\r", iteration);
    //fflush(stdout);

    if (doTimedSave && (elapsed_time > total_time)) {
      cout << "Saved " << numEvents <<" events in "<< elapsed_time << " seconds!" << endl;
      saveit = false;
      doWriteId = false;
      doTimedSave = false;
      total_time = 0;
      fprintf(fp, "Event %d time %d \n", numEvents, elapsed_time);
      if (fp != (FILE *)NULL) {
	fclose(fp);
	fp = (FILE *)NULL;
      }
    }     

    char *result = fgets(txt, 80, fifofp);
    if (result != NULL) {
      if (strncmp(txt, "e", 1) == 0) {
	printf("End command received\n");
	if (fp != (FILE *)NULL) {
	  fclose(fp);
	  fp = (FILE *)NULL;
	}
	my_private_exit(0);
      }

      if (strncmp(txt, "s", 1) == 0) {
	char *ptr = txt;
	void skip_blanks(char **);
	printf("Save command received. ");
	if (strlen(txt) != 1) {
	  ptr++;
	  skip_blanks(&ptr);
	  printf("filename %s\n", ptr);
	  saveit = true;
	  if (fp != (FILE *)NULL)
	    printf("File already open, ignoring...\n");
	  else
	    fp = fopen (ptr, "w");
	}
	else
	  printf("No filename specified. No file opened\n");
	fflush(stdout);
      }
      else if (strncmp(txt, "S", 1) == 0) {
	char *ptr = txt;
	void skip_blanks(char **);
	printf("Save with Id command received. ");
	if (strlen(txt) != 1) {
	  ptr++;
	  skip_blanks(&ptr);
	  printf("filename %s\n", ptr);
	  saveit = true;
	  doWriteId = true;
	  if (fp != (FILE *)NULL)
	    printf("File already open, ignoring...\n");
	  else
	    fp = fopen (ptr, "w");
	}
	else
	  printf("No filename specified. No file opened\n");
	fflush(stdout);
      }
      else if (strncmp(txt, "t", 1) == 0) {
	char *ptr = txt;
	void skip_blanks(char **);
	int scan_unsigned_number(char **, __u32 *);
	printf("DoTimedSave command received, ");
	if (strlen(txt) != 1) {
	  ptr++;
	  skip_blanks(&ptr);
	  if(scan_unsigned_number(&ptr, (__u32 *)&total_time) != 0) {
	    total_time = 0;
	    printf("but no proper time format specified. Ignoring...\n");
	  }
	  else {
	    doTimedSave = true;
	    start_time = 0;
	    curr_time = 0;
	    elapsed_time = 0;
	    numEvents = 0;
	    printf("total_time = %d\n", total_time);
	  }
	  
	}
	else
	  printf("but no time specified. Ignoring...\n");
	
	fflush(stdout);
      }
      else if (strncmp(txt, "c", 1) == 0) {
	printf("Close command received\n");fflush(stdout);
	saveit = false;
	doWriteId = false;
	if (fp != (FILE *)NULL) {
	  fclose(fp);
	  fp = (FILE *)NULL;
	}
      }	
      else if (strncmp(txt, "f", 1) == 0) {
	printf("Filter command received\n");fflush(stdout);
	filterit = true;
      }	
      else if (strncmp(txt, "n", 1) == 0) {
	printf("NoFilter command received\n");fflush(stdout);
	filterit = false;
      }	
      else if (strncmp(txt, "q", 1) == 0) {
	printf("quietReceive command received\n");fflush(stdout);
	printReceived = false;
      }	
      else if (strncmp(txt, "d", 1) == 0) {
	printf("printReceived packets command received\n");fflush(stdout);
	printReceived = true;
      }	
      else if (strncmp(txt, "w", 1) == 0) {
	printf("Write Reponse command received\n");fflush(stdout);
	// the following call blocks until the other side is open as well
	//respFifoFd = open(FIFO_RESPONSE, O_WRONLY | O_NONBLOCK);
	writeResponse = true;
	// write a dummy message to indicate we are ready
	int dummy = 1;
	write(respFifoFd, &dummy, 4);
      }	
      else if (strncmp(txt, "r", 1) == 0) {
	printf("No Write Reponse command received\n");fflush(stdout);
	writeResponse = false;
	//if (respFifoFd != -1) {
	//close(respFifoFd);
	//respFifoFd = -1;
	//}
      }	
      else if (strncmp(txt, "a", 1) == 0) {
	printf("addTime command received\n");fflush(stdout);
	addTime = true;
      }	
      else if (strncmp(txt, "A", 1) == 0) {
	printf("don't addTime command received\n");fflush(stdout);
	addTime = false;
      }	

      
      else {
	errno = parse_input_message(txt, &m, &devID);
	if (errno == 0) {
	  printf("pcanloop: message assembled: %d %c %c 0x%08x %1d  ", 
		 devID,
		 (m.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
		 (m.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
		 m.ID, 
		 m.LEN); 
	  for (i = 0; i < m.LEN; i++)
	    printf("0x%02x ", m.DATA[i]);
	  printf("\n");fflush(stdout);
	  
	  // new: send the message even with extended message IDs
	  // send the message
	  if (hI[devID] != NULL) {
	    if ( (errno = CAN_Write(hI[devID], &m)) ) {
	      perror("pcanloop: CAN_Write()");
	      my_private_exit(errno);
	    }
	  }
	}
	else {
	  printf("Parse error %d\n", errno);
	}
      }
      
    }


    currentIndex++;
    if (openHandles[currentIndex] == 0) currentIndex = 0;
    errno = LINUX_CAN_Read_Timeout(hI[openHandles[currentIndex]], &mr, 1000); // timeout = 1ms

    if (errno == 0) { // data received

      
      // check if a CAN status is pending	     
      if (mr.Msg.MSGTYPE & MSGTYPE_STATUS) {
	// status = CAN_Status(h);
	status = CAN_Status(hI[openHandles[currentIndex]]);
	printf("received status %d from device %d\n", status, openHandles[currentIndex]);
	if ((int)status < 0) {
	  errno = nGetLastError();
	  perror("pcanloop: CAN_Status()");
	  my_private_exit(errno);
	}
	else {
	  check_err(status, txt);
	  printf("%s\n", txt);
	  //printf("pcanloop: pending CAN status 0x%04x read.\n", (__u16)status);
	}
	fflush(stdout);

	if ((status & CAN_ERR_ANYBUSERR) != 0) {
	  // in case of any BUSERR re-initialize to see if it goes away
	  errno = CAN_Init(hI[openHandles[currentIndex]], wBTR0BTR1, nExtended);
	  if (errno) {
	    perror("pcanloop: CAN_Init()");
	    my_private_exit(errno);
	  }
	}

      } 
      else if ((mr.Msg.MSGTYPE == MSGTYPE_STANDARD) || (mr.Msg.MSGTYPE == MSGTYPE_EXTENDED)) {
	if (printReceived) {
	  printf("pcanloop: message received : %d %c %c 0x%08x %1d  ", 
		 openHandles[currentIndex],
		 (mr.Msg.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
		 (mr.Msg.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
		 mr.Msg.ID, 
		 mr.Msg.LEN); 
	  
	  for (i = 0; i < mr.Msg.LEN; i++)
	    printf("0x%02x ", mr.Msg.DATA[i]);
	  printf("\n");fflush(stdout);
	}
	
	if (writeResponse) {
	  write(respFifoFd, &(mr.Msg), sizeof(m));
	}
	if (((mr.Msg.ID &  0x0000000F) == 0x1) && saveit) { // a DATA_TO_PC packet
	  numEvents++;
	  // save start time
	  if (numEvents == 1) {
	    start_time = time(theTime);
	  }
	  curr_time = time(theTime);
	  elapsed_time = curr_time - start_time;
	  
	  for (i=0; i<4; i++)
	    uc_ptr[i] = mr.Msg.DATA[i];
	  if(filterit) {
	    if ( (buffer[0]&0xf0000000) != 0xe0000000)
	      if (doWriteId) fprintf(fp, "0x%08x ", mr.Msg.ID);
	      fprintf(fp, "0x%08x\n", buffer[0]);
	  }
	  else {
	    if (addTime)
	      if ( (buffer[0]&0xff000000) == 0xea000000)
		buffer[0] |= (curr_time & 0x00ffffff);
	    if (doWriteId) fprintf(fp, "0x%08x ", mr.Msg.ID);
	    fprintf(fp, "0x%08x\n", buffer[0]);
	  }
	  if (mr.Msg.LEN == 8) {
	    for (i=4; i<8; i++)
	      uc_ptr[i] = mr.Msg.DATA[i];
	    if(filterit) {
	      if ( (buffer[1]&0xf0000000) != 0xe0000000)
		if (doWriteId) fprintf(fp, "0x%08x ", mr.Msg.ID);
		fprintf(fp, "0x%08x\n", buffer[1]);
	    }
	    else {
	      if (addTime)
		if ( (buffer[1]&0xff000000) == 0xea000000)
		  buffer[1] |= (curr_time & 0x00ffffff);
	      if (doWriteId) fprintf(fp, "0x%08x ", mr.Msg.ID);
	      fprintf(fp, "0x%08x\n", buffer[1]);
	    }
	  }
	}
      }
      
    } // data received
    else {
      status = CAN_Status(hI[openHandles[currentIndex]]);
      if ((int)status < 0) {
	errno = nGetLastError();
	perror("pcanloop: CAN_Status()");
	my_private_exit(errno);
      }
      else {
	if ((status != 0) && (status != CAN_ERR_QRCVEMPTY)) {
	  printf("pcanloop: pending CAN status 0x%04x read.\n", (__u16)status);
	  check_err(status, txt);
	  printf("%s\n", txt);
	  fflush(stdout);

	  if ((status & CAN_ERR_ANYBUSERR) != 0) {
	    // in case of any BUSERR re-initialize to see if it goes away
	    errno = CAN_Init(hI[openHandles[currentIndex]], wBTR0BTR1, nExtended);
	    if (errno) {
	      perror("pcanloop: CAN_Init()");
	      my_private_exit(errno);
	    }
	  }

	}
      }
    }
  } // while (1)
  
  return errno;
}


/********************* 
	PARSER STUFF 
**********************/

//----------------------------------------------------------------------------
// skip blanks and tabs
void skip_blanks(char **ptr)
{
  // remove blanks or tabs
  while ((**ptr == ' ') || (**ptr == '\t')) 
    (*ptr)++; 
}

//----------------------------------------------------------------------------
// skip blanks, return 0 if the 1st non-blank char is not '\n'
int skip_blanks_and_test_for_CR(char **ptr)
{
  // remove blanks or tabs
  skip_blanks(ptr);
  
  if (**ptr == '\n')
    return -1;
  else
    return 0;
}

//----------------------------------------------------------------------------
// extract a number, either hex or decimal from a string
int scan_unsigned_number(char **ptr, __u32 *dwResult)
{
  char *p = *ptr;
  
  *dwResult = strtoul(p, ptr, 0); 
  
  if (p != *ptr)
    return 0;
    
  return ERANGE;
}

//----------------------------------------------------------------------------
// extract a char from a string
char scan_char(char **ptr)
{
  return *(*ptr)++;
}
  
//----------------------------------------------------------------------------
// parses a message command
int parse_input_message(char *buffer, TPCANMsg *Message, WORD *devID)
{
  char *ptr = buffer;
  __u32 dwLen;
  __u32 dwDat;
  int i;
  unsigned int parseUInt;
  int err = EINVAL;
  
  *devID = 255;
 
  // remove leading blanks
  skip_blanks(&ptr);
  
  // search for 'm' or 'r' to distinguish between message or remote transmit requests
  Message->MSGTYPE  = 0; 
  switch (scan_char(&ptr))
  {
    case 'm':  break; // normal message
    case 'r':  Message->MSGTYPE |= MSGTYPE_RTR; break; // rtr message

    default:   goto reject;
  }
  if (skip_blanks_and_test_for_CR(&ptr)) // no CR allowed here
    goto reject;
   
  // read message type
  switch (scan_char(&ptr))
  {
    case 's': break;
    case 'e': Message->MSGTYPE |= MSGTYPE_EXTENDED; break;
    
    default:  goto reject;
  }
  if (skip_blanks_and_test_for_CR(&ptr))
    goto reject;
  
  // read CAN-ID
  if ((err = scan_unsigned_number(&ptr, &Message->ID)))
    goto reject;
  if (Message->MSGTYPE & MSGTYPE_EXTENDED)
  {
    if (Message->ID > 0x3fffffff)
      goto reject;
  }
  else
  {
    if (Message->ID > 2047)
      goto reject;
  }
  if (skip_blanks_and_test_for_CR(&ptr))
    goto reject;
  
  //read datalength
  if ((err = scan_unsigned_number(&ptr, &dwLen)))
    goto reject;
  if (dwLen > 8)
    goto reject;  
  Message->LEN = (__u8)dwLen;  
    
  // read data elements up to message len
  i = 0;
  while (i < (int)dwLen)
  {
    if (skip_blanks_and_test_for_CR(&ptr))
    goto reject;
    
    if ((err = scan_unsigned_number(&ptr, &dwDat)))
      goto reject;
    if (dwDat > 255)
      goto reject;    
	  Message->DATA[i] = (__u8)dwDat;
    	  
	  i++; 
  }   
  if (*ptr == 0) return 0;

  err = scan_unsigned_number(&ptr, &parseUInt);
  if (parseUInt > 255) *devID = 255;
  else *devID = (WORD) parseUInt;
  
  reject:
  return err;
}
