/* File name     : can_utils.cpp
 * Creation date : 6/26/2012
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id$";
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
#include <errno.h>


#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>

#include "locallibpcan.h"

//****************************************************************************
// GLOBALS

//****************************************************************************
// CODE 

//****************************************************************************
int LINUX_CAN_Read_Timeout(int cansock, struct can_frame *frame, int nMicroSeconds)
{
  struct timeval t;
  fd_set fdRead;
  int nbytes;
  int err;

  // JSJSJSJSJSJS: temporarily make timeout small
  //nMicroSeconds = 50000;

#ifdef VCAN
  nMicroSeconds = 5000;
#endif

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
  
#ifdef VCAN
  sprintf(ifr.ifr_name, "vcan%d", devID);
#else
  sprintf(ifr.ifr_name, "can%d", devID);
#endif

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
int CAN_Close(HANDLE hHandle)
{
  return(close(hHandle));
}

int CAN_Write(HANDLE hHandle, struct can_frame* pMsgBuff)
{
  int ret;
  ret = write(hHandle, pMsgBuff, sizeof(struct can_frame));
  if (ret < 0) perror("CAN_Write");
  return ret;
}

int LINUX_CAN_Write_Timeout(HANDLE hHandle, struct can_frame* pMsgBuff, int nMicroSeconds)
{
  int ret;
  // need to do something with timeout, but for now:
  ret = CAN_Write(hHandle, pMsgBuff);
  return ret;
}



