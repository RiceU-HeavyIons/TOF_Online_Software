/* File name     : pcanloop.cxx
 * Creation date : 9/10/03
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: pcanloop.cc,v 1.10 2005-11-18 16:45:47 jschamba Exp $";
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
#include <stdlib.h>
#include <fcntl.h>
#include <libpcan.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>

//****************************************************************************
// DEFINES
#define FIFO_FILE "/tmp/pcanfifo"

//****************************************************************************
// GLOBALS
HANDLE h = NULL;
FILE *fifofp = (FILE *)NULL;

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
  if ( err == CAN_ERR_OK )        strcpy(txtbuff, "OK ") ;
  if ( err & CAN_ERR_XMTFULL )    strcat(txtbuff, "XMTFULL ") ;
  if ( err & CAN_ERR_OVERRUN )    strcat(txtbuff, "OVERRUN ") ;
  if ( err & CAN_ERR_BUSLIGHT )   strcat(txtbuff, "BUSLIGHT ") ;
  if ( err & CAN_ERR_BUSHEAVY )   strcat(txtbuff, "BUSHEAVY ") ;
  if ( err & CAN_ERR_BUSOFF )     strcat(txtbuff, "BUSOFF ") ;
  if ( err & CAN_ERR_QRCVEMPTY )  strcat(txtbuff, "QRCVEMPTY ") ;
  if ( err & CAN_ERR_QOVERRUN )   strcat(txtbuff, "QOVERRUN ") ;
  if ( err & CAN_ERR_QXMTFULL )   strcat(txtbuff, "QXMTFULL ") ;
  if ( err & CAN_ERR_REGTEST )    strcat(txtbuff, "REGTEST ") ;
  if ( err & CAN_ERR_NOVXD )      strcat(txtbuff, "NOVXD ") ;
  if ( (err & CAN_ERRMASK_ILLHANDLE) == CAN_ERR_HWINUSE ) strcat(txtbuff, "HWINUSE ") ;
  if ( (err & CAN_ERRMASK_ILLHANDLE) == CAN_ERR_NETINUSE ) strcat(txtbuff, "NETINUSE ") ;
  if ( (err & CAN_ERRMASK_ILLHANDLE) == CAN_ERR_ILLHW )strcat(txtbuff, "ILLHW ") ;
  if ( (err & CAN_ERRMASK_ILLHANDLE) == CAN_ERR_ILLCLIENT )strcat(txtbuff, "ILLCLIENT ") ;
  if ( (err & CAN_ERRMASK_ILLHANDLE) == CAN_ERR_ILLNET ) strcat(txtbuff, "ILLNET ") ;
  if ( err & CAN_ERR_RESOURCE ) strcat(txtbuff, "RESOURCE ") ;
  if ( err & CAN_ERR_ILLPARAMTYPE ) strcat(txtbuff, "ILLPARAMTYPE ") ;
  if ( err & CAN_ERR_ILLPARAMVAL ) strcat(txtbuff, "ILLPARAMVAL ") ;
  return;
}

// centralized entry point for all exits
static void my_private_exit(int error)
{
  char txt[255];

  if (h)
  {
    printf("Closing pcan\n");
    CAN_Close(h); 
  }
  printf("pcanloop: finished (%d).\n\n", error);fflush(stdout);
  if (fifofp != (FILE *)NULL)
    fclose(fifofp);
  sprintf(txt, "%s-%d", FIFO_FILE, getuid());
  unlink(txt);

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
  //int   nExtended = CAN_INIT_TYPE_ST;
  int   nExtended = CAN_INIT_TYPE_EX; // open for "extended" message IDs
  int iteration = 0;
  char txt[255]; // temporary string storage
  bool saveit = false;
  bool doTimedSave = false;
  bool filterit = false;
  bool printReceived = true;
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
    printf("Trying to open PCAN for Device ID 0x%x with BTR0BTR1=0x%04x\n", devID, wBTR0BTR1);
  else
    printf("Trying to open PCAN for Device ID 0x%x with 500 kbit/sec.\n", devID);
  fflush(stdout);
  
  // install signal handler for manual break
  signal(SIGINT, signal_handler);
  
  // open the CAN port
  // please use what is appropriate  
  // HW_DONGLE_SJA 
  // HW_DONGLE_SJA_EPP 
  // HW_ISA_SJA 
  // HW_PCI 
  // HW_USB  -- this is the one we are using

  for (int i=0; i<8; i++) {
    sprintf(devName, "/dev/pcan%d", 32+i);
    //h = CAN_Open(HW_USB, dwPort, wIrq);
    h = LINUX_CAN_Open(devName, O_RDWR|O_NONBLOCK);
    if (h == NULL) {
      //printf("Failed to open device %s\n", devName);
      //my_private_exit(errno);
      continue;
    }
    // get the hardware ID from the diag structure:
    LINUX_CAN_Statistics(h,&my_PDiag);
    printf("\tDevice at %s: Hardware ID = 0x%x\n", devName, 
	   my_PDiag.wIrqLevel); fflush(stdout);
    if (my_PDiag.wIrqLevel == devID) break;
    CAN_Close(h);

  }

  if (!h) {
    printf("Device ID 0x%x not found, exiting\n", devID);
    errno = nGetLastError();
    perror("pcanloop: CAN_Open()");
    fflush(stdout);
    my_private_exit(errno);
  }

  PCAN_DESCRIPTOR *pcd = (PCAN_DESCRIPTOR *)h;
    
  // get version info
  errno = CAN_VersionInfo(h, txt);
  if (!errno)
    printf("pcanloop: driver version = %s\n", txt);
  else {
    perror("pcanloop: CAN_VersionInfo()");
    my_private_exit(errno);
  }
  
  // init to a user defined bit rate
  if (wBTR0BTR1) {
    errno = CAN_Init(h, wBTR0BTR1, nExtended);
    if (errno) {
      perror("pcanloop: CAN_Init()");
      my_private_exit(errno);
    }
  }
  
  // get the hardware ID from the diag structure:
  LINUX_CAN_Statistics(h,&my_PDiag);
  printf("pcanloop: Device Hardware ID = 0x%x\n", my_PDiag.wIrqLevel); fflush(stdout);

  // create or open control FIFO
  sprintf(txt, "%s-%d", FIFO_FILE, getuid());
  umask(0);
  // mknod(txt, S_IFIFO|0666, 0);
  // replace by "mkfifo" call:
  mkfifo(txt, 0666);
  fifofd = open(txt, O_RDONLY | O_NONBLOCK);
  fifofp = fdopen(fifofd, "r");
  
  
  
  // read in endless loop until Ctrl-C
  while (1) {	//**************** big while loop here ************************
    TPCANMsg m;
    __u32 status;
    int i;
    int parse_input_message(char *, TPCANMsg *);
    
    
    struct pollfd pfd;
    pfd.fd = pcd->nFileNo;
    pfd.events = POLLIN;
    
    iteration++;
    //printf("Iteration %d\r", iteration);
    //fflush(stdout);

    if (doTimedSave && (elapsed_time > total_time)) {
      cout << "Saved " << numEvents <<" events in "<< elapsed_time << " seconds!" << endl;
      saveit = false;
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
      else if (strncmp(txt, "t", 1) == 0) {
	char *ptr = txt;
	void skip_blanks(char **);
	int scan_unsigned_number(char **, __u32 *);
	printf("DoTimedSave command received, ");
	if (strlen(txt) != 1) {
	  ptr++;
	  skip_blanks(&ptr);
	  if(scan_unsigned_number(&ptr, (__u32 *)&total_time) != 0)
	    printf("but no proper time format specified. Ignoring...\n");
	  else {
	    doTimedSave = true;
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

      else {
	errno = parse_input_message(txt, &m);
	if (errno == 0) {
	  printf("pcanloop: message assembled: %c %c 0x%08x %1d  ", 
		 (m.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
		 (m.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
		 m.ID, 
		 m.LEN); 
	  for (i = 0; i < m.LEN; i++)
	    printf("0x%02x ", m.DATA[i]);
	  printf("\n");fflush(stdout);
	  
	  // new: send the message even with extended message IDs
	  //if (!(m.MSGTYPE & MSGTYPE_EXTENDED)) {  
	  // send the message
	  if ( (errno = CAN_Write(h, &m)) ) {
	    perror("pcanloop: CAN_Write()");
	    my_private_exit(errno);
	  }
	  //}
	}
      }
      
    }

    errno = poll(&pfd, 1, 1000); // timeout = 1 second
    //printf("poll returned 0x%x\n", errno);
    //printf("revents = 0x%x\n", pfd.revents);
    
    if (errno == 1) { // data received
      if ((errno = CAN_Read(h, &m))) {
	perror("pcanloop: CAN_Read()");
	my_private_exit(errno);
      }
      else { // data read
	if (printReceived) {
	  printf("pcanloop: message received : %c %c 0x%08x %1d  ", 
		 (m.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
		 (m.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
		 m.ID, 
		 m.LEN); 
	  
	  for (i = 0; i < m.LEN; i++)
	    printf("0x%02x ", m.DATA[i]);
	  printf("\n");fflush(stdout);
	}
	
	// check if a CAN status is pending	     
	if (m.MSGTYPE & MSGTYPE_STATUS) {
	  status = CAN_Status(h);
	  if ((int)status < 0) {
	    errno = nGetLastError();
	    perror("pcanloop: CAN_Status()");
	    my_private_exit(errno);
	  }
	  else
	    printf("pcanloop: pending CAN status 0x%04x read.\n", (__u16)status);
	  fflush(stdout);
	} 
	else if ((m.MSGTYPE == MSGTYPE_STANDARD) || (m.MSGTYPE == MSGTYPE_EXTENDED)) {
	  if (((m.ID &  0x1F000780) == 0x0) && saveit) { // a DATA_TO_PC packet
	    numEvents++;
	    // save start time
	    if (numEvents == 1) {
	      start_time = time(theTime);
	    }
	    curr_time = time(theTime);
	    elapsed_time = curr_time - start_time;

	    for (i=0; i<4; i++)
	      uc_ptr[3-i] = m.DATA[i];
	    if(filterit) {
	      if ( (buffer[0]&0xf0000000) != 0xe0000000)
		fprintf(fp, "0x%08x\n", buffer[0]);
	    }
	    else
	      fprintf(fp, "0x%08x\n", buffer[0]);
	    if (m.LEN == 8) {
	      for (i=4; i<8; i++)
		uc_ptr[11-i] = m.DATA[i];
	      if(filterit) {
		if ( (buffer[1]&0xf0000000) != 0xe0000000)
		  fprintf(fp, "0x%08x\n", buffer[1]);
	      }
	      else
		fprintf(fp, "0x%08x\n", buffer[1]);
	    }
	  }
	}

      } // data read
    } // data received
    else {
      status = CAN_Status(h);
      if ((int)status < 0) {
	errno = nGetLastError();
	perror("pcanloop: CAN_Status()");
	my_private_exit(errno);
      }
      else {
	if (status != CAN_ERR_QRCVEMPTY) {
	  //printf("pcanloop: pending CAN status 0x%04x read.\n", (__u16)status);
	  check_err(status, txt);
	  printf("%s\n", txt);
	  fflush(stdout);
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
int parse_input_message(char *buffer, TPCANMsg *Message)
{
  char *ptr = buffer;
  __u32 dwLen;
  __u32 dwDat;
  int i;
  int err = EINVAL;
  
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
  return 0; 
  
  reject:
  return err;
}
