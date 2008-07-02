/* File name     : xgetStatus.cc
 * Creation date : 10/05/2007
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: xgetStatus.cc,v 1.6 2008-07-02 15:01:18 jschamba Exp $";
#endif /* lint */

//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

// other headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <libpcan.h>
#include <sys/types.h>
#include <sys/stat.h>


//****************************************************************************
// DEFINES
// #define LOCAL_DEBUG

#define FIFO_FILE       "/tmp/pcanfifo"
#define FIFO_RESPONSE "/tmp/pcanfifoRsp"


//****************************************************************************
// CODE 


//****************************************************************************
// Method to convert a decimal value to a binary value
void decToBin(int number) {
  int remainder;
  
  if(number <= 1) {
    cout << number;
    return;
  }
  
  remainder = number%2;
  decToBin(number >> 1);
  cout << remainder;
}


//****************************************************************************
/* Method to convert bits in an array to decimal.
 *
 * param bitArray - pointer to the array of bits
 * param numBits - and how many bits are in the value to be converted
 */
int bitArrayToDec(int *bitArray, int numBits) {
  int decimalVal = 0;

  for (int i=0; i<numBits; i++) {
    if(*bitArray==1){
      decimalVal += (0x1<<i);
    }
    bitArray++;
  }
  return decimalVal;
}


//****************************************************************************
// here is where all is done
int getStatus(int tdcNum, int tdigNodeID, int tcpuNodeID, int devID)
{
  FILE *fp;
  int fifofd;
  int numRead; 
  TPCANMsg m;
  string cmdString;
  stringstream ss;


  // create the CANbus message ID and data
  int msgid = tdigNodeID & 0x3f;
  msgid = (msgid<<4) | 0x004;
  // now add extended msg ID from TCPU nodeID
  msgid = (msgid<<18) | tcpuNodeID;
#ifdef LOCAL_DEBUG
  cout << "Message ID = 0x" << hex << msgid << endl;
#endif

  unsigned short DATA0 = 0x4 | (tdcNum & 0x3);

#ifdef LOCAL_DEBUG
  printf("open write FIFO\n");
#endif
  if((fp = fopen(FIFO_FILE, "w")) == NULL) {
    perror("fopen");
    exit(1);
  }

  /* this blocks until the other side is open for Write */
#ifdef LOCAL_DEBUG
  printf("open FIFO\n");
 #endif
 fifofd = open(FIFO_RESPONSE, O_RDONLY);
 if (fifofd == -1) {
   perror("open response FIFO");
   exit(1);
 }


  // notify pcanloop that we want response messages
#ifdef LOCAL_DEBUG
  printf("sending w command\n");
#endif
  fputs("w", fp); fflush(fp);
  // read dummy response
  int dummy;
  numRead = read(fifofd, &dummy, 4);
  if(numRead < 0) {
    perror("dummy read");
    printf("didn't get connection response\n");
    exit(1);
  }

 // send the "GET_STATUS" CANbus HLP message
#ifdef LOCAL_DEBUG
  printf("sending command\n");
#endif

  ss << "m e " << showbase << hex << msgid
     << " 1 " << DATA0
     << " " << dec << devID;
  cmdString = ss.str();

#ifdef LOCAL_DEBUG
  cout << "cmdString = " << cmdString << endl;
#endif

  fputs(cmdString.c_str(), fp); fflush(fp);
  

  // now we should receive two messages back;
  // one should be 8 bytes long,
  // the second should be 2 bytes long

  int bitNum = 0;
  int status[64];
  for(int i=0; i<64; i++) status[i] = 0;

  // first message:
#ifdef LOCAL_DEBUG
  printf("reading response, sizeof(m) = %d\n", sizeof(m));
#endif
  numRead = read(fifofd, &m, sizeof(m));
  if(numRead < 0) perror("read");


#ifdef LOCAL_DEBUG
  printf("number of bytes read in first read %d\n", numRead);
#endif

  if (numRead > 0) {
#ifdef LOCAL_DEBUG
    printf("testit: message received : %c %c 0x%08x %1d  ", 
	   (m.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
	   (m.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
	   m.ID, 
	   m.LEN); 
    
    for (int i=0; i<m.LEN; i++)
      printf("0x%02x ", m.DATA[i]);
    printf("\n");
#endif

    // convert to binary and put in array for processing
    int dat; // temp variable to hold DATA byte
    for (int i=1; i<m.LEN; i++) {
      /* For TDIG-D and beyond we are sending LSByte first */
      dat = (int)m.DATA[i];
      for (int k=0; k<8; k++) {
	if ((dat & 0x1)==1) status[bitNum] = 1;
	dat = dat >> 1;
	bitNum++;
      } // end loop over bits in byte
    } // end loop over bytes in message payload
    
  }


  // second message:
#ifdef LOCAL_DEBUG
  printf("trying to read second response\n");
#endif
  numRead = read(fifofd, &m, sizeof(m));
  if(numRead < 0) perror("read");

  // finished with read FIFO
  close(fifofd);

#ifdef LOCAL_DEBUG
  printf("number of bytes read in second read %d\n", numRead);
#endif

  if (numRead > 0) {
#ifdef LOCAL_DEBUG
    printf("testit: message received : %c %c 0x%08x %1d  ", 
	   (m.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
	   (m.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
	   m.ID, 
	   m.LEN); 
    
    for (int i=0; i<m.LEN; i++)
      printf("0x%02x ", m.DATA[i]);
    printf("\n");
#endif

    // convert to binary and put in array for processing
    int dat; // temp variable to hold DATA byte
    for (int i=1; i<m.LEN; i++) {
      /* For TDIG-D and beyond we are sending LSByte first */
      dat = (int)m.DATA[i];
      for (int k=0; k<8; k++) {
	if ((dat & 0x1)==1) status[bitNum] = 1;
	dat = dat >> 1;
	bitNum++;
      } // end loop over bits in byte
    } // end loop over bytes in message payload


  }

  // tell pcanloop that we no longer want response messages
#ifdef LOCAL_DEBUG
  printf("sending W command\n");
#endif
  fputs("W", fp); fflush(fp);
  fclose(fp);


  // now decode the bits received according to the HPTCD status definition:
  if (bitNum > 60) {
    // format the output
    fprintf(stdout,  "Status reply for TDIG Board (node) %d, TDC # %d\n", tdigNodeID, tdcNum);
    fprintf(stdout,  "Parity   [61] = %d\n", status[61]);
    fprintf(stdout,  "DLL lock [60] = %d\n", status[60]);
    fprintf(stdout,  "Trigger_fifo_empty [59]= %d\n", status[59]);
    fprintf(stdout,  "Trigger_fifo_full  [58]= %d\n", status[58]);
    int trigger_fifo_occupancy=bitArrayToDec(&status[54],4);
    fprintf(stdout,  "Trigger_fifo_occupancy [57..54]= %d%d%d%d (%d words)\n",
	    status[57], status[56], status[55], status[54], trigger_fifo_occupancy);
    int group0_L1_occupancy=bitArrayToDec(&status[46],8);
    fprintf(stdout,  "Group0_L1_occupancy [53..46]= %d%d%d%d%d%d%d%d (%d words)\n",
	    status[53], status[52], status[51], status[50],
	    status[49], status[48], status[47], status[46], group0_L1_occupancy);
    int group1_L1_occupancy=bitArrayToDec(&status[38],8);
    fprintf(stdout,  "Group1_L1_occupancy [45..38]= %d%d%d%d%d%d%d%d (%d words)\n",
	    status[45], status[44], status[43], status[42],
	    status[41], status[40], status[39], status[38], group1_L1_occupancy);
    int group2_L1_occupancy=bitArrayToDec(&status[30],8);
    fprintf(stdout,  "Group2_L1_occupancy [37..30]= %d%d%d%d%d%d%d%d (%d words)\n",
	    status[37], status[36], status[35], status[34],
	    status[33], status[32], status[31], status[30], group2_L1_occupancy);
    int group3_L1_occupancy=bitArrayToDec(&status[22],8);
    fprintf(stdout,  "Group3_L1_occupancy [29..22]= %d%d%d%d%d%d%d%d (%d words)\n",
	    status[29], status[28], status[27], status[26],
	    status[25], status[24], status[23], status[22], group3_L1_occupancy);
    fprintf(stdout,  "Readout_fifo_empty [21]= %d\n", status[21]);
    fprintf(stdout,  "Readout_fifo_full  [20]= %d\n", status[20]);
    int readout_fifo_occupancy=bitArrayToDec(&status[12],8);
    fprintf(stdout,  "Readout_fifo_occupancy [19..11]= %d%d%d%d%d%d%d%d (%d words)\n",
	    status[19], status[18], status[17], status[16],
	    status[15], status[14], status[13], status[12], readout_fifo_occupancy);
    fprintf(stdout,  "have_token = %d\n", status[11]);
    fprintf(stdout,  "Status (Error) [10..0] = %d%d%d%d%d%d%d%d%d%d%d\n",
	    status[10], status[9], status[8],
	    status[7], status[6], status[5],
	    status[4], status[3], status[2],
	    status[1], status[0]);
    if(status[0]){cout << "VERNIER ERROR [0]\n";}
    if(status[1]){cout << "COURSE ERROR [1]\n";}
    if(status[2]){cout << "CHANNEL SELECT ERROR [2]\n";}
    if(status[3]){cout << "L1 BUFFER PARITY ERROR [3]\n";}
    if(status[4]){cout << "TRIGGER FIFO PARITY ERROR [4]\n";}
    if(status[5]){cout << "TRIGGER MATCHING STATE ERROR [5]\n";}
    if(status[6]){cout << "READOUT FIFO PARITY ERROR [6]\n";}
    if(status[7]){cout << "READOUT STATE ERROR [7]\n";}
    if(status[8]){cout << "SETUP PARITY ERROR [8]\n";}
    if(status[9]){cout << "CONTROL PARITY ERROR [9]\n";}
    if(status[10]){cout << "JTAG INSTRUCTION ERROR [10]\n";}
  } // end if got enough in


  return(0);
}



//****************************************************************************
// main entry point
int main(int argc, char *argv[])
{
  int devID;

  cout << vcid << endl;
  cout.flush();
  
  if ( argc < 4 ) {
    cout << "USAGE: " << argv[0] << " <TDC #> <TDIG nodeID> <TCPU nodeID> [<devID>]\n";
    return 1;
  }
  
  
  int tdcNum = atoi(argv[1]);
  if ((tdcNum < 1) || (tdcNum > 3)) {
    cerr << "TDC # = " << tdcNum << " invalid entry. Use 1..3 instead." << endl;
    return -1;
  }

  int tdigNodeID = strtol(argv[2], (char **)NULL, 0);
  if ((tdigNodeID < 1) || (tdigNodeID > 0x3f)) {
    cerr << "tdigNodeID = " << tdigNodeID << " invalid entry. Use 0x1..0x3f (63) instead." << endl;
    return -1;
  }

  int tcpuNodeID = strtol(argv[3], (char **)NULL, 0);
  if ((tcpuNodeID < 1) || (tcpuNodeID > 0x3f)) {
    cerr << "tcpuNodeID = " << tcpuNodeID << " invalid entry. Use 0x1..0x3f (63) instead." << endl;
    return -1;
  }

  if (argc == 5) {
    devID = strtol(argv[4], (char **)NULL, 0);
    if ((devID < 1) || (devID > 255)) {
      cerr << "devID = " << devID << " invalid entry. Use 1..255 instead." << endl;
      return -1;
    }
  }
  else
    devID = 255;

  return getStatus(tdcNum, tdigNodeID, tcpuNodeID, devID);
}
