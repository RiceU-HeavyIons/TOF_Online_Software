#ifndef __LIBPCAN_H__
#define __LIBPCAN_H__

#define WORD unsigned short int
#define DWORD unsigned int


//****************************************************************************
// INCLUDES

#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#define MSGTYPE_STANDARD 0x0
#define MSGTYPE_EXTENDED CAN_EFF_FLAG

typedef int HANDLE;


//****************************************************************************
// PROTOTYPES

int CAN_Open(int devID);

// int CAN_Init(HANDLE hHandle, short int wBTR0BTR1, int nCANMsgType);

int CAN_Close(HANDLE hHandle);

//DWORD CAN_Status(HANDLE hHandle);

int CAN_Write(HANDLE hHandle, struct can_frame* pMsgBuff);

//****************************************************************************
//  LINUX_CAN_Write_Timeout()
//  writes a message to the CAN bus. If the (software) message buffer is full
//  the current write request blocks until a write slot gets empty
//  or a timeout or a error occures.
//  nMicroSeconds  > 0 -> Timeout in microseconds
//  nMicroSeconds == 0 -> polling
//  nMicroSeconds  < 0 -> blocking, same as CAN_Write()
int LINUX_CAN_Write_Timeout(int hHandle, struct can_frame* pMsgBuff, int nMicroSeconds);


//****************************************************************************
//  LINUX_CAN_Read_Timeout()
//  reads a message WITH TIMESTAMP from the CAN bus. If there is no message
//  to read the current request blocks until either a new message arrives
//  or a timeout or a error occures.
//  nMicroSeconds  > 0 -> Timeout in microseconds
//  nMicroSeconds == 0 -> polling
//  nMicroSeconds  < 0 -> blocking, same as LINUX_CAN_Read()
int LINUX_CAN_Read_Timeout(int hHandle, struct can_frame* pMsgBuff, int nMicroSeconds);

//***************************************************************************
// CAN_ResetFilter() - removes all current Message Filters
// Caution! Currently this operation influences all read paths
//
DWORD CAN_ResetFilter(HANDLE hHandle);

//***************************************************************************
// CAN_MsgFilter() - reduce received data in to FromID <= ID <= ToID
// Type may be MSGTYPE_STANDARD or MSGTYPE_EXTENDED
// This function can be called multiple to add more ranges.
// Caution! Currently this operation influences all read paths
//
DWORD CAN_MsgFilter(HANDLE hHandle, DWORD FromID, DWORD ToID, int nCANMsgType);

//***************************************************************************
// LINUX_CAN_FileHandle() - return PCAN driver file handle for select(2)
//
int LINUX_CAN_FileHandle(HANDLE hHandle);

//****************************************************************************
//  LINUX_CAN_Extended_Status()
//  get the same as CAN_Status() with additional informaton about pending reads or writes
//
//  There is a uncertainty of 1 message for "nPendingWrites" for a small amount
//  of time between the messages is put into the CAN sender and the telegram is
//  successfuly sent or an error is thrown.
DWORD LINUX_CAN_Extended_Status(HANDLE hHandle, int *nPendingReads, int *nPendingWrites);


//****************************************************************************
//  nGetLastError()
//  returns the last stored error (errno of the shared library). The returend
//  error is independend of any path.
//
int nGetLastError(void);

//****************************************************************************
//  LINUX_CAN_Open() - another open, LINUX like
//  creates a path to a CAN port
//
//  input: the path to the device node (e.g. /dev/pcan0)
//  returns NULL when open failes
//
HANDLE LINUX_CAN_Open(const char *szDeviceName, int nFlag);

//****************************************************************************
//  LINUX_CAN_BTR0BTR1() - get the BTR0 and BTR1 from bitrate, LINUX like
//
//  input:  the handle to the device node
//          the bitrate in bits / second, e.g. 500000 bits/sec
//
//  returns 0 if not possible
//          BTR0BTR1 for the interface
//
WORD LINUX_CAN_BTR0BTR1(HANDLE hHandle, DWORD dwBitRate);

#endif // __LIBPCAN_H__
