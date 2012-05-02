/* $Id$ */
#ifndef __CAN_UTILS_H__
#define __CAN_UTILS_H__

#include <vector>
using namespace std;

extern void printCANMsg(const TPCANMsg &, const char *);
extern int openCAN(WORD);
extern int openCAN_br(WORD, WORD);
extern int sendCAN_and_Compare(TPCANMsg &, const char *, const int, 
			       unsigned int = 0xffffffff, bool = false);

/* These functions communicate  with the pcanloop program over FIFOs: */
extern DWORD CAN_Write_l(HANDLE hHandle, TPCANMsg* pMsgBuff, int devID);
extern int openCAN_l(WORD devID);
extern DWORD CAN_Close_l(HANDLE hHandle);
extern DWORD LINUX_CAN_Read_l(HANDLE hHandle, TPCANRdMsg* pMsgBuff);
extern DWORD LINUX_CAN_Read_Timeout_l(HANDLE hHandle, TPCANRdMsg* pMsgBuff, 
				      int nMicroSeconds);
extern int sendCAN_and_Compare_l(int, TPCANMsg &, const char *, const int, 
				 unsigned int = 0xffffffff, bool = false);
extern void check_err(__u32,  char *);
extern int findAllTCPUs(vector<unsigned int> *); 

#endif // __CAN_UTILS_H__
