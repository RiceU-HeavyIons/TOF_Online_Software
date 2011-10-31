/* File name     : pcan_find.cxx
 * Creation date : 10/29/04
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: pcan_find.cc,v 1.6 2011-10-31 15:06:04 jschamba Exp $";
#endif /* lint */


//****************************************************************************
// INCLUDES
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
#include <sys/ioctl.h>

//****************************************************************************
// DEFINES

//****************************************************************************
// GLOBALS
HANDLE h = NULL;

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

  strcpy(txtbuff, "Errorflags set: ") ;
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
  if (h)
  {
    printf("Closing pcan\n");
    CAN_Close(h); 
  }
  printf("pcan_find: finished (%d).\n\n", error);
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
  //__u16 wBTR0BTR1 = CAN_BAUD_1M;
  __u16 wBTR0BTR1 = CAN_BAUD_500K;
  int   nExtended = CAN_INIT_TYPE_ST;
  char txt[255]; // temporary string storage
  char devName[255];
  TPDIAG my_PDiag;
  bool firsttime = true;
  int nFileHandle;
  TPEXTRAPARAMS params;
  
  errno = 0;
  
  // give some information back
  if (wBTR0BTR1)
    printf("PCAN init with BTR0BTR1=0x%04x\n", wBTR0BTR1);
  else
    printf("PCAN init with 500 kbit/sec.\n");
  
  
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
    //sprintf(devName, "/dev/pcan%d", 32+i);
    sprintf(devName, "/dev/pcanusb%d", i);
    //h = CAN_Open(HW_USB, dwPort, wIrq);
    h = LINUX_CAN_Open(devName, O_RDWR);
    if (h == NULL) {
      //printf("Failed to open device %s\n", devName);
      //my_private_exit(errno);
      continue;
    }
    //printf("Opened pcan device at %s\n", devName);

    if (firsttime) {
      // get version info
      errno = CAN_VersionInfo(h, txt);
      if (!errno)
	printf("PCAN Driver version = %s\n", txt);
      else {
	perror("pcan_find: CAN_VersionInfo()");
	my_private_exit(errno);
      }
      printf("\nFound the following PCAN device(s):\n");
      firsttime = false;
    }
    
    // init to a user defined bit rate
    if (wBTR0BTR1) {
      errno = CAN_Init(h, wBTR0BTR1, nExtended);
      if (errno) {
	perror("pcan_find: CAN_Init()");
	my_private_exit(errno);
      }
    }
    
    nFileHandle = LINUX_CAN_FileHandle(h);
    params.nSubFunction = SF_GET_HCDEVICENO;

    errno = ioctl(nFileHandle, PCAN_EXTRA_PARAMS, &params);
    if(errno != 0) {
      printf("\tioctl returned %d\n", errno); 
      perror("pcan_find: ioctl()");
      my_private_exit(errno);
    }

     // get the hardware ID from the diag structure:
    LINUX_CAN_Statistics(h,&my_PDiag);
    printf("\tDevice at %s: IRQ Level = 0x%x, Hardware ID = 0x%x, dwBase = 0x%x, wErrorFlag = 0x%x\n", 
	   devName, 
	   my_PDiag.wIrqLevel,
	   params.func.ucHCDeviceNo,
	   my_PDiag.dwBase,
	   my_PDiag.wErrorFlag);
    if (my_PDiag.wErrorFlag != 0) {
	  check_err(my_PDiag.wErrorFlag, txt);
	  printf("\t--%s\n", txt);
    }      
    

    CAN_Close(h);
    
  }

  return errno;
}
