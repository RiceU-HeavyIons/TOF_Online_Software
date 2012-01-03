// INCLUDES
#include <vxWorks.h>
#include <logLib.h>
#include <taskLib.h>

int checkDSM(unsigned int addrVal, int numVals)
{
  unsigned int dsmval;
  volatile unsigned int *dsmAddr = (unsigned int *)addrVal;

  int prevVal[6];
  int currVal[6];
  int tmpval = 0;
  int i,j;

  dsmval = *dsmAddr++;
  
  prevVal[5] = (int)((dsmval & 0x7c000000)>>26);
  prevVal[4] = (int)((dsmval & 0x03e00000)>>21);
  prevVal[3] = (int)((dsmval & 0x001f0000)>>16);

  prevVal[2] = (int)((dsmval & 0x7c00)>>10);
  prevVal[1] = (int)((dsmval & 0x03e0)>>5);
  prevVal[0] = (int)(dsmval & 0x001f);
  logMsg("converted: %2d %2d %2d %2d %2d %2d\n",
	 prevVal[5],
	 prevVal[4],
	 prevVal[3],
	 prevVal[2],
	 prevVal[1],
	 prevVal[0]);


  for (i=1; i<numVals; i++) {
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
    
    for (j=0; j<6; j++) {
      if (currVal[j] != ((prevVal[j] + 1)%32)) {
	tmpval |= 1<<j;
	logMsg("%d out of sequence\n", j, 0,0,0,0,0);
      }
      prevVal[j] = currVal[j];
    }
  }
  
  logMsg("\n\nError Mask: 0x%x\n", tmpval, 0,0,0,0,0);

  // delay a little, so log messages all print first
  taskDelay(1000);


  return 0;
}
