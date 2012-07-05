/* $Id$ */
#ifndef __CAN_UTILS_H__
#define __CAN_UTILS_H__

#include <vector>
using namespace std;

extern int CAN_Read_Timeout(int, struct can_frame *, int);
extern int CAN_Open(int);
extern void printCANMsg(const struct can_frame &, const char *);
extern int sendCAN_and_Compare(int, struct can_frame &, const char *, const int, 
			       unsigned int = 0xffffffff, bool = false);
extern int findAllTCPUs(int h, vector<unsigned int> *); 

#endif // __CAN_UTILS_H__
