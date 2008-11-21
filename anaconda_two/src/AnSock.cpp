/*
 * AnSock.cpp
 *
 *  Created on: Nov 10, 2008
 *      Author: koheik
 */

#include <fcntl.h>
#include <glob.h>
#include <QtCore/QDebug>
#include "AnSock.h"

int AnSock::TCAN_DEBUG = 1;
const char * AnSock::PCAN_DEVICE_PATTERN = "./dev/pcan*";

AnSock::AnSock() : handle(NULL) {
  // TODO Auto-generated constructor stub

}

AnSock::~AnSock() {
  if (handle != NULL) CAN_Close(handle);
}

void AnSock::set_msg(TPCANMsg &msg, ...)
{
  va_list argptr;
  va_start(argptr, msg);

  msg.ID      = va_arg(argptr, int);
  msg.MSGTYPE = va_arg(argptr, int);
  msg.LEN     = va_arg(argptr, int);
  for(int i = 0; i < msg.LEN && i < 8; i++)
    msg.DATA[i] = va_arg(argptr, int);

  va_end(argptr);
}


QMap<int, AnSock*> AnSock::open(QList<int> &dev_id_list) {

	QMap<int, AnSock*> sock_map;
	foreach(int dev_id, dev_id_list) sock_map[dev_id] = NULL;

	char *dev_path;

	TPDIAG tpdiag;
	char txt_buff[VERSIONSTRING_LEN];
	unsigned int i;

	WORD wBTR0BTR1 = CAN_BAUD_1M;         /* 250K, 500K */
	int nExtended  = CAN_INIT_TYPE_EX;    /* CAN_INIT_TYPE_ST */

	glob_t globb;

	globb.gl_offs = 0;
	glob(PCAN_DEVICE_PATTERN, GLOB_DOOFFS, NULL, &globb);

	if (globb.gl_pathc == 0)
	{
		qDebug() << "device files were not found";
	}

	HANDLE h = NULL;
	for(i = 0; i < globb.gl_pathc; i++) {
		if (TCAN_DEBUG) {
			printf("glob[%d] %s\n", i, globb.gl_pathv[i]);
		}
		dev_path = globb.gl_pathv[i];
		h = LINUX_CAN_Open(dev_path, O_RDWR | O_NONBLOCK);
		if (h == NULL) {
			if (TCAN_DEBUG) fprintf(stderr, "cannot open: %s\n", dev_path);
			continue;
		}
		LINUX_CAN_Statistics(h, &tpdiag);
		int dev_id = tpdiag.wIrqLevel;
	
		if (sock_map.contains(dev_id)) {
			CAN_Init(h, wBTR0BTR1, nExtended);
			AnSock *sock = new AnSock();
			sock->handle = h;
			sock->addr = dev_id;
			sock->dev_path = dev_path;
			sock_map[dev_id] = sock;

			if (TCAN_DEBUG) {
				CAN_VersionInfo(h, txt_buff);
				printf("handle:   %llx\n", reinterpret_cast<unsigned long long>(h));
				printf("dev_id:   %x\n", dev_id);
				printf("dev_name: %s\n", dev_path);
				printf("version_info: %s\n", txt_buff);
			}
		} else { // this dev_id is not requested
			CAN_Close(h);
		}
	}
	globfree(&globb);
	
	foreach(int dev_id, sock_map.keys()) {
		if (sock_map[dev_id] == NULL)
			qFatal("Device %d is not found.", dev_id);
	}
	return sock_map;
}

int AnSock::open(quint8 dev_id) {
  char *dev_path;

  TPDIAG tpdiag;
  char txt_buff[VERSIONSTRING_LEN];
  unsigned int i;

  WORD wBTR0BTR1 = CAN_BAUD_1M;         /* 250K, 500K */
  int nExtended  = CAN_INIT_TYPE_EX;    /* CAN_INIT_TYPE_ST */

  glob_t globb;

  globb.gl_offs = 0;
  glob(PCAN_DEVICE_PATTERN, GLOB_DOOFFS, NULL, &globb);

//  dl->dl_irq = (WORD*)malloc(globb.gl_pathc*sizeof(WORD));
//  dl->dl_path = (char**)malloc(globb.gl_pathc*sizeof(char *));


  if (globb.gl_pathc == 0)
  {
    qDebug() << "device files were not found";
  }

  HANDLE h = NULL;
  for(i = 0; i < globb.gl_pathc; i++) {
    if (TCAN_DEBUG) {
      printf("glob[%d] %s\n", i, globb.gl_pathv[i]);
    }
    dev_path = globb.gl_pathv[i];
    h = LINUX_CAN_Open(dev_path, O_RDWR | O_NONBLOCK);
    if (h == NULL) {
      if (TCAN_DEBUG) fprintf(stderr, "cannot open: %s\n", dev_path);
      continue;
    }
    LINUX_CAN_Statistics(h, &tpdiag);

    if (dev_id != tpdiag.wIrqLevel) {
      CAN_Close(h);
      h = NULL;
      continue;
    }

    if (TCAN_DEBUG) {
      CAN_Init(h, wBTR0BTR1, nExtended);
      CAN_VersionInfo(h, txt_buff);
      printf("handle:   %llx\n", reinterpret_cast<unsigned long long>(h));
      printf("dev_id:   %x\n", dev_id);
      printf("dev_name: %s\n", dev_path);
      printf("version_info: %s\n", txt_buff);
    }
    this->handle = h;
    this->addr = dev_id;
    this->dev_path = dev_path;
    break;
  }

  globfree(&globb);
  handle = h;

  if (handle == NULL)
    return -1;
  else
    return 0;
}

/*********************************************************************/
void AnSock::print(const TPCANMsg &msg)
{
  int i;

  printf("CANMsg ID: 0x%x TYPE: 0x%02x DATA[%d]: ",
                                       msg.ID, msg.MSGTYPE, msg.LEN);
  for(i = 0; i < msg.LEN && i < 8; i++)
    printf("0x%02x ", msg.DATA[i]);
  puts("");
}

/*********************************************************************/
void AnSock::print(const TPCANRdMsg &rmsg)
{
  print(rmsg.Msg);
}

quint64 AnSock::write_read(TPCANMsg &msg, TPCANRdMsg &rmsg,
    unsigned int return_length, unsigned int time_out)
{

  if(handle == NULL) {
    qDebug() << "device is not open";
    return -1;
  }

  quint64 data = 0;
  unsigned int length = 0;
  unsigned int niter = return_length / 8 + ((return_length % 8)? 1 : 0);
  int er;

  if (TCAN_DEBUG) {
    printf(">> ");
    print(msg);
  }

  er = CAN_Write(handle, &msg);
  if (er != 0) {
    int status = CAN_Status(handle);
    if (status > 0)
      fprintf(stderr, "CANbus error: 0x%x\n", status);
    else
      fprintf(stderr, "System error: 0x%x\n", status);
  }

  for(unsigned int i = 0; i < niter && !er; ++i) {
    er = LINUX_CAN_Read_Timeout(handle, &rmsg, time_out);
    if (er != 0) {
      int status = CAN_Status(handle);
      if (status > 0)
        fprintf(stderr, "CANbus error: 0x%x\n", status);
      else
        fprintf(stderr, "System error: 0x%x\n", status);
    }
    if (TCAN_DEBUG) {
      printf("<< ");
      print(rmsg);
    }
    if (rmsg.Msg.DATA[0] != msg.DATA[0]) {
      fprintf(stderr, "Return payload doesn't match.\n");
      er = -1;
    }
    length += rmsg.Msg.LEN;
    for(int j = 1; j < rmsg.Msg.LEN; ++j)
      data |= static_cast<quint64>(rmsg.Msg.DATA[j]) << 8 * (7*i + j-1);
  }

  if (return_length != length) {
    fprintf(stderr, "Return length doesn't match.\n");
  }

  return data;
}
