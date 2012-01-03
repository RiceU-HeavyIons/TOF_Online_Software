// INCLUDES
#include <vxWorks.h>
#include <logLib.h>
#include <taskLib.h>

int printsequence(unsigned int addrVal, int numVals)
{
  unsigned int dsmval;
  volatile unsigned int *dsmAddr = (unsigned int *)addrVal;

  int currVal[6];
  int i;

  for (i=0; i<numVals; i++) {
    dsmval = *dsmAddr++;
    
    currVal[5] = (int)((dsmval & 0x7c000000)>>26);
    currVal[4] = (int)((dsmval & 0x03e00000)>>21);
    currVal[3] = (int)((dsmval & 0x001f0000)>>16);
    
    currVal[2] = (int)((dsmval & 0x7c00)>>10);
    currVal[1] = (int)((dsmval & 0x03e0)>>5);
    currVal[0] = (int)(dsmval & 0x001f);
    logMsg("converted: %2d %2d %2d %2d %2d %2d\n",
	   currVal[5],
	   currVal[4],
	   currVal[3],
	   currVal[2],
	   currVal[1],
	   currVal[0]);
    
  }
  
  // delay a little, so the print statements finish
  taskDelay(500);
  return 0;
}
