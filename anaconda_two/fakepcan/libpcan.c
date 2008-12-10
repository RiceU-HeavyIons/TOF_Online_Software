#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <errno.h>

#include <pthread.h>

#include <string.h>
#include <stdarg.h>

#include <libpcan.h>
#define VERSION_STRING "Fake_PCAN_Driver_20081119"

// device information
typedef struct {
	int           irq;
	HANDLE        handle;
	DWORD         bitrate;
	int           msgtype;
	TPCANMsg      msg;
	unsigned int  csum;
	unsigned int  cunt;
	char          dname[64];
	int           sock;
} dinfo;

// initialized device information
static dinfo dlist[] = {
 {255, (HANDLE)0xcc01, CAN_BAUD_500K, CAN_INIT_TYPE_ST, {0, 0, 0, {0}}, 0, 0, "pcan32", -1},
 {254, (HANDLE)0xcc02, CAN_BAUD_500K, CAN_INIT_TYPE_ST, {0, 0, 0, {0}}, 0, 0, "pcan31", -1},
 {253, (HANDLE)0xcc03, CAN_BAUD_500K, CAN_INIT_TYPE_ST, {0, 0, 0, {0}}, 0, 0, "pcan30", -1},
 {252, (HANDLE)0xcc04, CAN_BAUD_500K, CAN_INIT_TYPE_ST, {0, 0, 0, {0}}, 0, 0, "pcan29", -1},
 {251, (HANDLE)0xcc05, CAN_BAUD_500K, CAN_INIT_TYPE_ST, {0, 0, 0, {0}}, 0, 0, "pcan28", -1},
 {250, (HANDLE)0xcc06, CAN_BAUD_500K, CAN_INIT_TYPE_ST, {0, 0, 0, {0}}, 0, 0, "pcan27", -1},
 {0,             NULL,             0,                0, {0, 0, 0, {0}}, 0, 0,       "", -1}
};

static pthread_t *thrd = NULL;
//****************************************************************************
//  CAN_Open()
//  creates a path to a CAN port
//
//  for PCAN-Dongle call:             CAN_Open(HW_DONGLE_.., DWORD dwPort, WORD wIrq);
//  for PCAN-ISA or PCAN-PC/104 call: CAN_Open(HW_ISA_SJA, DWORD dwPort, WORD wIrq);
//  for PCAN-PCI call:                CAN_Open(HW_PCI, int nPort); .. enumerate nPort 1..8.
//
//  if ((dwPort == 0) && (wIrq == 0)) CAN_Open() takes the 1st default ISA or DONGLE port.
//  if (nPort == 0) CAN_Open() takes the 1st default PCI port.
//  returns NULL when open failes.
//
//  The first CAN_Open() to a CAN hardware initializes the hardware to default
//  parameter 500 kbit/sec and acceptance of extended frames.
//
HANDLE CAN_Open(WORD wHardwareType, ...)
{
	int nPort;

	va_list vlist;
	va_start(vlist, wHardwareType);

	switch(wHardwareType) {
		case HW_PCI:
		nPort = va_arg(vlist, int);
		break;
	}

	va_end(vlist);
	return NULL;
}

//****************************************************************************
//  CAN_Init()
//  initializes the CAN hardware with the BTR0 + BTR1 constant "CAN_BAUD_...".
//  nCANMsgType must be filled with "CAN_INIT_TYPE_..".
//  The default initialisation, e.g. CAN_Init is not called,
//  is 500 kbit/sec and extended frames.
//
DWORD CAN_Init(HANDLE hHandle, WORD wBTR0BTR1, int nCANMsgType)
{
	int i;
#ifdef DEBUG
	fprintf(stderr, "FAKE::CAN_Init(%p, 0x%04x, %d)\n", hHandle, wBTR0BTR1, nCANMsgType);
#endif
	for(i = 0; dlist[i].irq; i++) {
		if(dlist[i].handle == hHandle) {
			dlist[i].bitrate = wBTR0BTR1;
			dlist[i].msgtype = nCANMsgType;
			return CAN_ERR_OK;
		}
	}
	return -1;
}

//****************************************************************************
//  CAN_Close()
//  closes the path to the CAN hardware.
//  The last close on the hardware put the chip into passive state.
DWORD CAN_Close(HANDLE hHandle)
{
	dinfo *ptr;
	int   nopen = 0;
#ifdef DEBUG
	fprintf(stderr, "CAN_CLose(%p)\n", hHandle);
#endif
	for (ptr = dlist; ptr->irq; ptr++) {
		if (ptr->handle == hHandle) {
			char sname[256];
			close(ptr->sock);
			ptr->sock = -1;
			sprintf(sname, "dev/%s.sock", ptr->dname);
			unlink(sname);
#ifdef DEBUG
			puts(sname);
#endif
		}
		if (ptr->sock >=0 ) nopen++;
	}

	if (nopen == 0 && thrd != NULL) {
		pthread_cancel(*thrd);
		thrd = NULL;
	}

	return CAN_ERR_OK;
}

//****************************************************************************
//  CAN_Status()
//  request the current (stored) status of the CAN hardware. After the read the
//  stored status is reset.
//  If the status is negative a system error is returned (e.g. -EBADF).
DWORD CAN_Status(HANDLE hHandle)
{
	dinfo *ptr;

	for(ptr = dlist; ptr->irq; ptr++) {
		if (ptr->handle == hHandle)
			{ break;}
	}
	return CAN_ERR_OK;
}

//****************************************************************************
//  CAN_Write()
//  writes a message to the CAN bus. If the write queue is full the current
//  write blocks until either a message is sent or a error occured.
//
DWORD CAN_Write(HANDLE hHandle, TPCANMsg* pMsgBuff)
{
	int i;

#ifdef DEBUG
	fprintf(stderr, "FAKE::CAN_Write(hHandle=%p, pMsgBuff=%p)\n", hHandle, pMsgBuff);
#endif
	for(i = 0; dlist[i].irq; i++)
		if(dlist[i].handle == hHandle) {
			memcpy(&dlist[i].msg, pMsgBuff, sizeof(TPCANMsg));
		}
	return CAN_ERR_OK;
}

//****************************************************************************
//  LINUX_CAN_Write_Timeout()
//  writes a message to the CAN bus. If the (software) message buffer is full
//  the current write request blocks until a write slot gets empty
//  or a timeout or a error occures.
//  nMicroSeconds  > 0 -> Timeout in microseconds
//  nMicroSeconds == 0 -> polling
//  nMicroSeconds  < 0 -> blocking, same as CAN_Write()
DWORD LINUX_CAN_Write_Timeout(HANDLE hHandle, TPCANMsg* pMsgBuff, int nMicroSeconds)
{
	return CAN_ERR_OK;
}

DWORD THUB_readHandler(HANDLE hHandle, TPCANMsg *pMsgBuff)
{
	dinfo *ptr;
	double temp;

	for(ptr = dlist; ptr->irq; ptr++) {
		if (ptr->handle == hHandle)
			{ break;}
	}

	if (ptr->irq == 0) return -1;

	memcpy(pMsgBuff, &(ptr->msg), sizeof(TPCANMsg));
	switch(pMsgBuff->DATA[0]) {

	case 0x01: // MCU Firmware ID
		pMsgBuff->LEN = 3;
		pMsgBuff->DATA[1] = 0x54;
		pMsgBuff->DATA[2] = 0x10;
		usleep(2000);
		break;
	case 0x02: // FPGA Firmware ID
		pMsgBuff->LEN = 3;
		if (pMsgBuff->DATA[1] == 0) {
			pMsgBuff->DATA[2] = 0x84;
		} else {
			pMsgBuff->DATA[2] = 0x74;
		}
		usleep(2000);
		break;

	case 0x03: // Get Temperature
	    temp = 30.0 + 10.0*rand()/RAND_MAX;
		pMsgBuff->LEN = 3;
		pMsgBuff->DATA[1] = 0xFF & (int)(temp*100.0); // temperature xx.16
		pMsgBuff->DATA[2] = 0xFF & (int)(temp); // temperature 32.xx
		pMsgBuff->DATA[3] = 0xbb; // ESCR
		pMsgBuff->DATA[4] = 0; // AD 1L
		pMsgBuff->DATA[5] = 0; // AD 1H
		pMsgBuff->DATA[6] = 0; // AD 2L
		pMsgBuff->DATA[7] = 0; // AD 2H
		usleep(2000);
		break;

	case 0x05: // CRC Error bits
		pMsgBuff->LEN = 2;
		pMsgBuff->DATA[1] = 0xab;
		usleep(2000);
		break;
	default:
		pMsgBuff->LEN = 2;
		pMsgBuff->DATA[1] = 0;
	}
	if (pMsgBuff->DATA[0] >= 0x91 && pMsgBuff->DATA[0] <= 0x98) {
		pMsgBuff->LEN = 2;
		pMsgBuff->DATA[1] = 0x1f;
		usleep(2000);
	}

	return CAN_ERR_OK;
}

DWORD readHandler(HANDLE hHandle, TPCANMsg *pMsgBuff) {
	int i;
	dinfo *ptr;
	double temp;

	for (ptr = dlist; ptr->irq; ptr++) {
		if (ptr->handle == hHandle)
			{ break; }
	}

	if (ptr->irq == 0) return -1;

	memcpy(pMsgBuff, &(ptr->msg), sizeof(TPCANMsg));

	if (pMsgBuff->MSGTYPE == MSGTYPE_STANDARD && 0x400 == (pMsgBuff->ID & 0xF00))
		return THUB_readHandler(hHandle, pMsgBuff);

	switch (pMsgBuff->MSGTYPE) {
		case MSGTYPE_STANDARD: /* TCPU */
		switch(pMsgBuff->DATA[0]) {
		case 0x0e: /* PLD read/write, assuming reading from 0x2 */
			pMsgBuff->LEN = 3;
			pMsgBuff->DATA[2] = 0x0f;
			break;

		case 0xb0:
			pMsgBuff->LEN = 8;
			pMsgBuff->DATA[1] = 16; // temperature xx.16
			pMsgBuff->DATA[2] = 32; // temperature 32.xx
			pMsgBuff->DATA[3] = 0x8b; // ESCR
			pMsgBuff->DATA[4] = 0; // AD 1L
			pMsgBuff->DATA[5] = 0; // AD 1H
			pMsgBuff->DATA[6] = 0; // AD 2L
			pMsgBuff->DATA[7] = 0; // AD 2H
			break;
		case 0xb1:
			pMsgBuff->LEN = 4;
			pMsgBuff->DATA[1] = 0x45; // ID.L
			pMsgBuff->DATA[2] = 0x02; // ID.H
			pMsgBuff->DATA[3] = 0x85; // FPGA
			break;
		case 0xb2:
			pMsgBuff->LEN = 8;
			pMsgBuff->DATA[1] = 0x70;
			pMsgBuff->DATA[2] = 0x60;
			pMsgBuff->DATA[3] = 0x50;
			pMsgBuff->DATA[4] = 0x40;
			pMsgBuff->DATA[5] = 0x30;
			pMsgBuff->DATA[6] = 0x20;
			pMsgBuff->DATA[7] = 0x10;
			usleep(2000);
			break;
		}
		break;
	case MSGTYPE_EXTENDED:
		switch(pMsgBuff->DATA[0]) {
		case 0x04: // Control TDC
			pMsgBuff->LEN = 2;
			pMsgBuff->DATA[1] = 0;
			usleep(2000);
			break;
		case 0x05: // Cet Status TDC 1
			if (ptr->msg.LEN == 1) { // first round
				pMsgBuff->LEN = 8;
				pMsgBuff->DATA[1] = 0;
				pMsgBuff->DATA[2] = 0;
				pMsgBuff->DATA[3] = 0;
				pMsgBuff->DATA[4] = 0;
				pMsgBuff->DATA[5] = 0;
				pMsgBuff->DATA[6] = 0;
				pMsgBuff->DATA[7] = 0;
				ptr->msg.LEN = 8;
			} else { /* second round */
				pMsgBuff->LEN = 2;
				pMsgBuff->DATA[7] = 0;
			}
			usleep(2000);
			break;
		case 0x06: /* Cet Status TDC 2 */
			if (ptr->msg.LEN == 1)  { /* first round */
				pMsgBuff->LEN = 8;
				pMsgBuff->DATA[1] = 0x01;
				pMsgBuff->DATA[2] = 0x02;
				pMsgBuff->DATA[3] = 0x03;
				pMsgBuff->DATA[4] = 0x04;
				pMsgBuff->DATA[5] = 0x05;
				pMsgBuff->DATA[6] = 0x06;
				pMsgBuff->DATA[7] = 0x07;
				ptr->msg.LEN = 8;
			} else { /* second round */
				pMsgBuff->LEN = 2;
				pMsgBuff->DATA[1] = 0x08;
			}
			usleep(2000);
			break;
		case 0x07: /* Cet Status TDC 3 */
			if (ptr->msg.LEN == 1) { /* first round */
				pMsgBuff->LEN = 8;
				pMsgBuff->DATA[1] = 0;
				pMsgBuff->DATA[2] = 0;
				pMsgBuff->DATA[3] = 0;
				pMsgBuff->DATA[4] = 0;
				pMsgBuff->DATA[5] = 0;
				pMsgBuff->DATA[6] = 0;
				pMsgBuff->DATA[7] = 0;
				ptr->msg.LEN = 8;
			} else { // second round
				pMsgBuff->LEN = 2;
				pMsgBuff->DATA[1] = 0;
			}
			usleep(2000);
			break;
		case 0x08: // Threshold Set
			pMsgBuff->LEN = 2;
			pMsgBuff->DATA[1] = 0;
			usleep(2000);
			break;
		case 0x10: // Block Start
			pMsgBuff->LEN = 2;
			pMsgBuff->DATA[1] = 0;
			ptr->csum = 0;
			ptr->cunt = 0;
			usleep(2000);
			break;
		case 0x20: // Block Data
		    for (i = 1; i < ptr->msg.LEN; i++) ptr->csum += ptr->msg.DATA[i];
			ptr->cunt += (ptr->msg.LEN - 1);
			pMsgBuff->LEN = 2;
			pMsgBuff->DATA[1] = 0;
			usleep(2000);
			break;
		case 0x30: // Block Data
			pMsgBuff->LEN = 8;
			pMsgBuff->DATA[1] = 0;
			pMsgBuff->DATA[2] = 0xFF & (ptr->cunt);
			pMsgBuff->DATA[3] = 0xFF & ((ptr->cunt) >> 8);
			pMsgBuff->DATA[4] = 0xFF & (ptr->csum);
			pMsgBuff->DATA[5] = 0xFF & ((ptr->csum) >> 8);
			pMsgBuff->DATA[6] = 0xFF & ((ptr->csum) >> 16);
			pMsgBuff->DATA[7] = 0xFF & ((ptr->csum) >> 24);
			usleep(2000);
		break;
		case 0x40: // Write to ALL TDC
			pMsgBuff->LEN = 2;
			pMsgBuff->DATA[1] = 0;
			usleep(1500000);
		break;
		case 0x41: // Write to TDC 1
			pMsgBuff->LEN = 2;
			pMsgBuff->DATA[1] = 0;
			usleep(500000);
		break;
		case 0x42: // Write to TDC 2
			pMsgBuff->LEN = 2;
			pMsgBuff->DATA[1] = 0;
			usleep(500000);
		break;
		case 0x43: // Write to TDC 3
			pMsgBuff->LEN = 2;
			pMsgBuff->DATA[1] = 0;
			usleep(500000);
		break;
		case 0xb0: // Get Board Status
		    temp = 30.0 + 10.0*rand()/RAND_MAX;
			pMsgBuff->LEN = 8;
			pMsgBuff->DATA[1] = 0xFF & (int)(temp*100.0); // temperature xx.16
			pMsgBuff->DATA[2] = 0xFF & (int)(temp); // temperature 32.xx
			pMsgBuff->DATA[3] = 0xbb; // ESCR
			pMsgBuff->DATA[4] = 0; // AD 1L
			pMsgBuff->DATA[5] = 0; // AD 1H
			pMsgBuff->DATA[6] = 0; // AD 2L
			pMsgBuff->DATA[7] = 0; // AD 2H
			usleep(2000);
			break;
		case 0xb1:
			pMsgBuff->LEN = 4;
			pMsgBuff->DATA[1] = 0x53; // ID.L
			pMsgBuff->DATA[2] = 0x11; // ID.H
			pMsgBuff->DATA[3] = 0x74; // FPGA
			usleep(2000);
			break;
		case 0xb2:
			pMsgBuff->LEN = 8;
			pMsgBuff->DATA[1] = 0x77;
			pMsgBuff->DATA[2] = 0x66;
			pMsgBuff->DATA[3] = 0x55;
			pMsgBuff->DATA[4] = 0x44;
			pMsgBuff->DATA[5] = 0x33;
			pMsgBuff->DATA[6] = 0x22;
			pMsgBuff->DATA[7] = 0x11;
			usleep(2000);
			break;
		default:
			pMsgBuff->LEN = 2;
			pMsgBuff->DATA[1] = 0;
			usleep(2000);
		}
		break;
	}

	return CAN_ERR_OK;

}
//****************************************************************************
//  CAN_Read()
//  reads a message from the CAN bus. If there is no message to read the current
//  request blocks until either a new message arrives or a error occures.
DWORD CAN_Read(HANDLE hHandle, TPCANMsg* pMsgBuff)
{

	fprintf(stderr, "FAKE::CAN_Read(hHandle=%p, pMsgBuff=%p)\n", hHandle, pMsgBuff);

	return readHandler(hHandle, pMsgBuff);
}

//****************************************************************************
//  LINUX_CAN_Read()
//  reads a message WITH TIMESTAMP from the CAN bus. If there is no message
//  to read the current request blocks until either a new message arrives
//  or a error occures.
DWORD LINUX_CAN_Read(HANDLE hHandle, TPCANRdMsg* pMsgBuff)
{
	dinfo *ptr;
#ifdef FAKE_DEBUG
	fprintf(stderr, "FAKE::LINUX_CAN_Read(hHandle=%p, pMsgBuff=%p)\n", hHandle, pMsgBuff);
#endif
	for (ptr = dlist; ptr->irq; ptr++) {
		if (ptr->handle == hHandle) {
			int cnt = recv(ptr->sock, pMsgBuff, sizeof(TPCANRdMsg), MSG_DONTWAIT);
			if ( cnt == sizeof(TPCANRdMsg)) {
				return CAN_ERR_OK;
			}
		}
	}
	return readHandler(hHandle, &(pMsgBuff->Msg));
}

//****************************************************************************
//  LINUX_CAN_Read_Timeout()
//  reads a message WITH TIMESTAMP from the CAN bus. If there is no message
//  to read the current request blocks until either a new message arrives
//  or a timeout or a error occures.
//  nMicroSeconds  > 0 -> Timeout in microseconds
//  nMicroSeconds == 0 -> polling
//  nMicroSeconds  < 0 -> blocking, same as LINUX_CAN_Read()
DWORD LINUX_CAN_Read_Timeout(HANDLE hHandle, TPCANRdMsg* pMsgBuff, int nMicroSeconds)
{
	dinfo *ptr;

#ifdef FAKE_DEBUG
	fprintf(stderr,
		"FAKE::LINUX_CAN_Read_Timeout(hHandle=%p, pMsgBuff=%p, nMicroSeconds=%d)\n",
		hHandle, pMsgBuff, nMicroSeconds);
#endif

	for (ptr = dlist; ptr->irq; ptr++) {
		if (ptr->handle == hHandle) {
			int cnt = recv(ptr->sock, pMsgBuff, sizeof(TPCANRdMsg), MSG_DONTWAIT);
			if ( cnt == sizeof(TPCANRdMsg)) {
				return CAN_ERR_OK;
			}
		}
	}

	return readHandler(hHandle, &(pMsgBuff->Msg));
}

//***************************************************************************
// CAN_ResetFilter() - removes all current Message Filters
// Caution! Currently this operation influences all read paths
//
DWORD CAN_ResetFilter(HANDLE hHandle)
{
	return CAN_ERR_OK;
}

//***************************************************************************
// CAN_MsgFilter() - reduce received data in to FromID <= ID <= ToID
// Type may be MSGTYPE_STANDARD or MSGTYPE_EXTENDED
// This function can be called multiple to add more ranges.
// Caution! Currently this operation influences all read paths
//
DWORD CAN_MsgFilter(HANDLE hHandle, DWORD FromID, DWORD ToID, int nCANMsgType)
{
	return CAN_ERR_OK;
}

//***************************************************************************
// LINUX_CAN_FileHandle() - return PCAN driver file handle for select(2)
//
int LINUX_CAN_FileHandle(HANDLE hHandle)
{
	dinfo *ptr;

	for (ptr = dlist; ptr->irq; ptr++) {
		if (ptr->handle == hHandle) return ptr->sock;
	}
	return -1;
}

//****************************************************************************
//  LINUX_CAN_Extended_Status()
//  get the same as CAN_Status() with additional informaton about pending reads or writes
//
//  There is a uncertainty of 1 message for "nPendingWrites" for a small amount
//  of time between the messages is put into the CAN sender and the telegram is
//  successfuly sent or an error is thrown.
DWORD LINUX_CAN_Extended_Status(HANDLE hHandle, int *nPendingReads, int *nPendingWrites)
{
	return CAN_ERR_OK;
}

//****************************************************************************
//  CAN_VersionInfo()
//  returns a text string with driver version info.
//
DWORD CAN_VersionInfo(HANDLE hHandle, LPSTR lpszTextBuff)
{
	fprintf(stderr, "LINUX_CAN_VersionInfo(%p, %p)\n", hHandle, lpszTextBuff);
	strcpy(lpszTextBuff, VERSION_STRING);
	return CAN_ERR_OK;
}

//****************************************************************************
//  nGetLastError()
//  returns the last stored error (errno of the shared library). The returend
//  error is independend of any path.
//
int nGetLastError(void)
{
	return CAN_ERR_OK;
}

void* runner(void *arg)
{
	dinfo *ptr;
	srand(100);

	for (ptr = dlist; ; ptr++) {
		if (ptr->irq == 0) ptr = dlist;

		if (ptr->sock > 0) { /* the device is open */
			TPCANRdMsg rmsg;
			int i, fd;
			struct sockaddr_un addr;
			unsigned short tcpu, tdig;

			fd = socket(PF_UNIX, SOCK_DGRAM, 0);

			addr.sun_family = AF_UNIX;
			sprintf(addr.sun_path, "dev/%s.sock", ptr->dname);
			connect(fd, &addr, sizeof(addr));

			tcpu = (0x21 + (int)(30.0*rand()/RAND_MAX));
			tdig = (0x10 + (int)(8.0*rand()/RAND_MAX));
			switch((int)(2.0*rand()/ RAND_MAX)) {
			case 0:
				rmsg.Msg.MSGTYPE = MSGTYPE_STANDARD;
				rmsg.Msg.ID   = (tcpu << 4) + 0x7;
				break;
			case 1:
				rmsg.Msg.MSGTYPE = MSGTYPE_EXTENDED;
				rmsg.Msg.ID   = (tdig << 4 | 0x7) << 18 | tcpu;
			}
			rmsg.Msg.LEN  = 4;
			rmsg.Msg.DATA[0] = 0xff;
			for(i = 1; i < 4; ++i) rmsg.Msg.DATA[i] = 0;
			send(fd, &rmsg, sizeof(rmsg), 0);

			close(fd);

			sleep(10);
		}
	}
}
//****************************************************************************
//  LINUX_CAN_Open() - another open, LINUX like
//  creates a path to a CAN port
//
//  input: the path to the device node (e.g. /dev/pcan0)
//  returns NULL when open failes
//
HANDLE LINUX_CAN_Open(const char *szDeviceName, int nFlag)
{
	int i;
	HANDLE h = 0;

#ifdef DEBUG
	fprintf(stderr, "FAKE::LINUX_CAN_Open(%s, %x)\n", szDeviceName, nFlag);
#endif
	for (i = 0; dlist[i].irq; i++) {
		char *bn = strdup(szDeviceName);
		if (strcmp(basename(bn), dlist[i].dname) == 0) {
			int sock, er;
			char sname[256];
			struct sockaddr_un addr;

			sprintf(sname, "%s.sock", szDeviceName);
			addr.sun_family = AF_UNIX;
			strcpy(addr.sun_path, sname);
			sock = socket(PF_UNIX, SOCK_DGRAM, 0);

			er = bind(sock, &addr, sizeof(addr.sun_family) + strlen(sname) + 1);
			if (er >=0) {
				h = dlist[i].handle;
				dlist[i].sock = sock;
			} else {
				h = NULL;
				perror("bind");
			}
			break;
		}
		free(bn);
	}

	if(h != NULL && thrd == NULL) {
		thrd = malloc(sizeof(pthread_t));
		if(pthread_create(thrd, NULL, runner, NULL))
			perror("pthread_create");
	}

	return h;
}

//****************************************************************************
//  LINUX_CAN_Statistics() - get statistics about this devices
//
DWORD LINUX_CAN_Statistics(HANDLE hHandle, TPDIAG *diag)
{
	// typedef struct
	// {
	//   WORD  wType;           // the type of interface hardware - see HW_....
	//   DWORD dwBase;          // the base address or port of this device
	//   WORD  wIrqLevel;       // the irq level of this device
	//   DWORD dwReadCounter;   // counts all reads to this device from start
	//   DWORD dwWriteCounter;  // counts all writes
	//   DWORD dwIRQcounter;    // counts all interrupts
	//   DWORD dwErrorCounter;  // counts all errors
	//   WORD  wErrorFlag;      // gathers all errors
	//   int   nLastError;      // the last local error for this device
	//   int   nOpenPaths;      // number of open paths for this device
	//   char  szVersionString[VERSIONSTRING_LEN]; // driver version string
	// } TPDIAG;
	int i;
#ifdef DEBUG
	fprintf(stderr, "FAKE::LINUX_CAN_Statistics(%p, %p)\n", hHandle, diag);
#endif
	for(i = 0; dlist[i].irq; i++) {
		if(dlist[i].handle == hHandle) {
			diag->wType     = HW_USB;
			diag->dwBase    = 100;
			diag->wIrqLevel = dlist[i].irq;
			strcpy(diag->szVersionString, VERSION_STRING);
			return CAN_ERR_OK;
		}
	}
	return -1;
}

//****************************************************************************
//  LINUX_CAN_BTR0BTR1() - get the BTR0 and BTR1 from bitrate, LINUX like
//
//  input:  the handle to the device node
//          the bitrate in bits / second, e.g. 500000 bits/sec
//
//  returns 0 if not possible
//          BTR0BTR1 for the interface
//
WORD LINUX_CAN_BTR0BTR1(HANDLE hHandle, DWORD dwBitRate)
{
	return CAN_ERR_OK;
}

