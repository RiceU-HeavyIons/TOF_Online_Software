#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: p_config.cc,v 1.1 2003-10-13 22:53:22 jschamba Exp $";
#endif /* lint */

#include <iostream>
#include <fstream>
#include <string>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <libpcan.h>

using namespace std;

#define LOCAL_STRING_LEN 64       // length of internal used strings
typedef struct
{
  char szVersionString[LOCAL_STRING_LEN];
  char szDevicePath[LOCAL_STRING_LEN];
  int  nFileNo;
} PCAN_DESCRIPTOR;

int p_config(const char *filename, HANDLE h, unsigned int nodeID, int TDC)
{
  ifstream conf;
  unsigned char confByte[81];
  unsigned int tempval;
  PCAN_DESCRIPTOR *pcd = (PCAN_DESCRIPTOR *)h;
  TPCANMsg mSend, mRec;
  
  cerr << "Configuring NodeID 0x" << hex << nodeID << ", TDC " <<
    dec << TDC << ", with filename " << filename << endl;
  //conf.open(filename,ifstream::in);
  conf.open(filename); // "in" is default 
  if ( !conf.good() ) {
    cerr << filename << ": file error\n";
    return -1;
  }

  conf.seekg(0,ios::beg);    //move file pointer to begin
  conf >> hex;


  for (int i=0; (i<81 && conf.good()); i++) { 
    conf >> tempval;
    if ( !conf.good() ) {
      cerr << filename << ": error reading value for config byte " << dec << i << endl;
      conf.close();
      return -1;
    }
    
    confByte[i] = (unsigned char)tempval;
    //cerr << "read value 0x" << hex << tempval;
    //cerr << dec << " confByte[" << i <<"] = 0x" << hex << (unsigned int)confByte[i] << endl;

  }

  conf.close();

  // read all the configuration bytes, now send them over CAN
  struct pollfd pfd;
  pfd.fd = pcd->nFileNo;
  pfd.events = POLLIN;

  mSend.MSGTYPE	= CAN_INIT_TYPE_ST;
  mSend.ID 	= 0x200 | nodeID; // CONFIGURE_TDC message
  mSend.LEN	= 1;


  // "CONFIGURE_TDC:Start"
  mSend.DATA[0]	= 0x00 | (TDC == 1) ? 0 : 0x10;
  CAN_Write(h, &mSend);
  // --- Wait on poll return without timeout (1000ms timeout)
  if (poll(&pfd, 1, 1000) == 0) { //"poll" returned a time-out //
    cerr << "Sent CONFIGURE_TDC:Start command, but did not receive response within 1 sec" << endl;
    return(-1);
  }
  else { //-- read the CAN message
    if (CAN_Read(h, &mRec) != 0) {
      perror("CAN_Read: ");
      return(-1);
    }
    if (mRec.ID != (0x180 | nodeID)) { // make sure correct response came back
      cerr << "COINFIGURE_TDC:Start response: wrong ID 0x" << hex << mRec.ID << endl;
      return(-1);
    }
      
    if (mRec.DATA[0] != mSend.DATA[0]) { // make sure no error bit set
      cerr << "CONFIGURE_TDC:Start response: first byte: 0x" << hex << mRec.DATA[0] <<
	" expected 0x" << mSend.DATA[0] << endl;
      return(-1);
    }
  }

  // "CONFIGURE_TDC:Data", 11 messages with 7 bytes each ...
  mSend.LEN	= 8;
  for (int i=1; i<12; i++) {
    mSend.DATA[0]	= 0x00 | (TDC == 1) ? 0 : 0x10 | i;
    for (int j=0; j<7; j++) mSend.DATA[j+1] = confByte[i*7+j];
    CAN_Write(h, &mSend);
    // --- Wait on poll return without timeout (1000ms timeout)
    if (poll(&pfd, 1, 1000) == 0) { //"poll" returned a time-out //
      cerr << "Sent CONFIGURE_TDC:Data command, but did not receive response within 1 sec" << endl;
      return(-1);
    }
    else { //-- read the CAN message
      if (CAN_Read(h, &mRec) != 0) {
	perror("CAN_Read: ");
	return(-1);
      }
      if (mRec.ID != (0x180 | nodeID)) { // make sure correct response came back
	cerr << "CONFIGURE_TDC:Data response: wrong ID 0x" << hex << mRec.ID << endl;
	return(-1);
      }
      
      if (mRec.DATA[0] != mSend.DATA[0]) { // make sure no error bit set
	cerr << "CONFIGURE_TDC:Data response: first byte: 0x" << hex << mRec.DATA[0] <<
	  " expected 0x" << mSend.DATA[0] << endl;
	return(-1);
      }
    }
  }

  // "CONFIGURE_TDC:Data", ... and 1 last message with 4 bytes
  mSend.LEN	= 5;
  mSend.DATA[0]	= 0x0c | (TDC == 1) ? 0 : 0x10;
  for (int j=0; j<7; j++) mSend.DATA[j+1] = confByte[74+j];
  CAN_Write(h, &mSend);
  // --- Wait on poll return without timeout (1000ms timeout)
  if (poll(&pfd, 1, 1000) == 0) { //"poll" returned a time-out //
    cerr << "Sent CONFIGURE_TDC:Data command, but did not receive response within 1 sec" << endl;
    return(-1);
  }
  else { //-- read the CAN message
    if (CAN_Read(h, &mRec) != 0) {
      perror("CAN_Read: ");
      return(-1);
    }
    if (mRec.ID != (0x180 | nodeID)) { // make sure correct response came back
      cerr << "CONFIGURE_TDC:Data response: wrong ID 0x" << hex << mRec.ID << endl;
      return(-1);
    }
    
    if (mRec.DATA[0] != mSend.DATA[0]) { // make sure no error bit set
      cerr << "CONFIGURE_TDC:Data response: first byte: 0x" << hex << mRec.DATA[0] <<
	" expected 0x" << mSend.DATA[0] << endl;
      return(-1);
    }
  }

  // "CONFIGURE_TDC:Config_end"
  mSend.LEN	= 1;
  mSend.DATA[0]	= 0x80 | (TDC == 1) ? 0 : 0x10;
  CAN_Write(h, &mSend);
  // --- Wait on poll return without timeout (1000ms timeout)
  if (poll(&pfd, 1, 1000) == 0) { //"poll" returned a time-out //
    cerr << "Sent CONFIGURE_TDC:Config_end command, but did not receive response within 1 sec" << endl;
    return(-1);
  }
  else { //-- read the CAN message
    if (CAN_Read(h, &mRec) != 0) {
      perror("CAN_Read: ");
      return(-1);
    }
    if (mRec.ID != (0x180 | nodeID)) { // make sure correct response came back
      cerr << "CONFIGURE_TDC:Config_end response: wrong ID 0x" << hex << mRec.ID << endl;
      return(-1);
    }
      
    if (mRec.DATA[0] != mSend.DATA[0]) { // make sure no error bit set
      cerr << "CONFIGURE_TDC:Config_end response: first byte: 0x" << hex << mRec.DATA[0] <<
	" expected 0x" << mSend.DATA[0] << endl;
      return(-1);
    }
  }

  // "CONFIGURE_TDC:Program"
  mSend.DATA[0]	= 0xc0 | (TDC == 1) ? 0 : 0x10;
  CAN_Write(h, &mSend);
  // --- Wait on poll return without timeout (1000ms timeout)
  if (poll(&pfd, 1, 1000) == 0) { //"poll" returned a time-out //
    cerr << "Sent CONFIGURE_TDC:Program command, but did not receive response within 1 sec" << endl;
    return(-1);
  }
  else { //-- read the CAN message
    if (CAN_Read(h, &mRec) != 0) {
      perror("CAN_Read: ");
      return(-1);
    }
    if (mRec.ID != (0x180 | nodeID)) { // make sure correct response came back
      cerr << "CONFIGURE_TDC:Program response: wrong ID 0x" << hex << mRec.ID << endl;
      return(-1);
    }
      
    if (mRec.DATA[0] != mSend.DATA[0]) { // make sure no error bit set
      cerr << "CONFIGURE_TDC:Program response: first byte: 0x" << hex << mRec.DATA[0] <<
	" expected 0x" << mSend.DATA[0] << endl;
      return(-1);
    }
  }

  return 0;
}


// A simple driver for now:
// "main" function to call the above
int main(int argc, char *argv[])
{
  HANDLE h = NULL;

  cerr << "p_config version 1.0 (JS). This program assumes nodeID = 0x01." << endl;

  if ( argc != 3 ) {
    printf("USAGE: %s <TDC_Number> <filename> \n", argv[0]);
    exit(1);
  }

  int TDC = atoi(argv[1]);
  if ((TDC != 1) && (TDC != 2)) { 
    cerr << "TDC = " << TDC << " invalid entry. Use 1 or 2 instead." << endl;
    exit(0);
  }

  if(!(h = LINUX_CAN_Open("/dev/pcan32", O_RDWR|O_NONBLOCK))) {
    cerr << "LINUX_CAN_Open failed\n";
    exit(0);
  }
    
  if(CAN_Init(h, CAN_BAUD_250K, CAN_INIT_TYPE_ST) != 0) {
    cerr << "CAN_Init failed\n";
    CAN_Close(h);
    exit(0);
  }

  p_config(argv[2], h, 0x01, TDC);

  CAN_Close(h); 
}

