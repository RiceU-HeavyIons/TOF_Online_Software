/*
 * $Id$
 *
 *  Created on: Nov 29, 2008
 *      Author: koheik
 */
#include <QtCore/QMutex>

#include <sys/ioctl.h>

#include "AnAgent.h"
#include "AnRoot.h"
#include "AnExceptions.h"

int AnAgent::TCAN_DEBUG = 0;
#ifndef FAKEPCAN
const char * AnAgent::PCAN_DEVICE_PATTERN = "/dev/pcanusb*";
#else
const char * AnAgent::PCAN_DEVICE_PATTERN = "./dev/pcan*";
#endif
//-----------------------------------------------------------------------------
AnAgent::AnAgent(QObject *parent) : QThread(parent), m_handle(0), m_comm_err(0)
{
}

//-----------------------------------------------------------------------------
AnAgent::~AnAgent()
{
	if (m_handle) CAN_Close(m_handle);
}

//-----------------------------------------------------------------------------
void AnAgent::set_msg(TPCANMsg &msg, ...)
{
	va_list argptr;
	va_start(argptr, msg);

	msg.ID      = va_arg(argptr, int);
	msg.MSGTYPE = va_arg(argptr, int);
	msg.LEN     = va_arg(argptr, int);
	for (int i = 0; i < msg.LEN && i < 8; ++i)
		msg.DATA[i] = va_arg(argptr, int);

	va_end(argptr);
}

//-----------------------------------------------------------------------------
void AnAgent::print(const TPCANMsg &msg)
{
	int i;

	printf("CANMsg ID: 0x%x TYPE: 0x%02x DATA[%d]: ",
	                   msg.ID, msg.MSGTYPE, msg.LEN);
	for (i = 0; i < msg.LEN && i < 8; i++)
		printf("0x%02x ", msg.DATA[i]);
	puts("");
	fflush(stdout);
}

//-----------------------------------------------------------------------------
void AnAgent::print(const TPCANRdMsg &rmsg)
{
  print(rmsg.Msg);
}

// member functions

QMutex mtex;
void AnAgent::print_send(const TPCANMsg &msg)
{
	mtex.lock();
	printf("[%d] >> ", m_id);
	print(msg);
	mtex.unlock();
}

void AnAgent::print_recv(const TPCANMsg &msg)
{
	mtex.lock();
	printf("[%d] << ", m_id);
	print(msg);
	mtex.unlock();
}

//-----------------------------------------------------------------------------
quint64 AnAgent::read(TPCANRdMsg &rmsg,
    int return_length, unsigned int time_out)
{
//	pre_check();

	quint64 data = 0;
	unsigned int length = 0;
	unsigned int niter = return_length / 8 + ((return_length % 8)? 1 : 0);
	int er = 0;

	for (unsigned int i = 0; i < niter && !er; ++i) {
		er = LINUX_CAN_Read_Timeout(m_handle, &rmsg, time_out);
		error_handle(er, rmsg.Msg);

		if (TCAN_DEBUG) {
			print_recv(rmsg.Msg);
			emit debug_recv(AnRdMsg(devid(), rmsg));
		}

		length += rmsg.Msg.LEN;
		for (int j = 1; j < rmsg.Msg.LEN; ++j)
			data |= static_cast<quint64>(rmsg.Msg.DATA[j]) << 8 * (7*i + j-1);
	}

	if (return_length >= 0 && return_length != (int)length) {
		log( QString("Return length doesn't match: expected %1, received %2")
		      .arg(return_length).arg(length) );
		throw AnExCanError(0);
	}

	return data;
}

//-----------------------------------------------------------------------------
void AnAgent::raw_write(TPCANMsg &msg, int time_out)
{
	pre_check();

	int er;

	if (TCAN_DEBUG) {
		print_send(msg);
		emit debug_send(AnRdMsg(devid(), msg));
	}

	er = LINUX_CAN_Write_Timeout(m_handle, &msg, time_out);
	error_handle(er, msg);
}

//-----------------------------------------------------------------------------
quint64 AnAgent::write_read(TPCANMsg &msg, TPCANRdMsg &rmsg,
    unsigned int return_length, unsigned int time_out)
{
	pre_check();

	quint64 data = 0;
	unsigned int length = 0;
	unsigned int niter = return_length / 8 + ((return_length % 8)? 1 : 0);
	int er;

	if (TCAN_DEBUG) {
		print_send(msg);
		emit debug_send(AnRdMsg(devid(), msg));
	}

	er = CAN_Write(m_handle, &msg);
	error_handle(er, msg);

	for (unsigned int i = 0; i < niter && !er; ++i) {
		int ntry = 10; // tra 10 times
		for (; ntry > 0; --ntry) {
			er = LINUX_CAN_Read_Timeout(m_handle, &rmsg, time_out);
			error_handle(er, rmsg.Msg);

			if (TCAN_DEBUG) {
				print_recv(rmsg.Msg);
				emit debug_recv(AnRdMsg(devid(), rmsg));
			}

			if (match(msg, rmsg.Msg)) {
				break;
			} else {
//				emit received(AnRdMsg(devid(), rmsg));
			}
		}
		if (ntry == 0) {
			log( QString("Didn't receive reply message for %1").arg(msg.ID) );
//			incCommError();
			throw AnExCanError(0);
		}  else {
			if (rmsg.Msg.DATA[0] != msg.DATA[0] && ((rmsg.Msg.ID >> 4) != 0x40)) {
				log( QString("Return payload doesn't match: expected %1, received %2")
			         .arg(msg.DATA[0]).arg(rmsg.Msg.DATA[0]) );
//				incCommError();
				throw AnExCanError(0);
			}
			length += rmsg.Msg.LEN;
			for(int j = 1; j < rmsg.Msg.LEN; ++j)
				data |= static_cast<quint64>(rmsg.Msg.DATA[j]) << 8 * (7*i + j-1);
		}
	}

	if (return_length != length) {
		log( QString("Return length doesn't match: expected %1, received %2")
		      .arg(return_length).arg(length) );
//		incCommError();
		throw AnExCanError(0);
	}

	return data;
}
//-----------------------------------------------------------------------------
QMap<int, AnAgent*> AnAgent::open(QMap<int, int>& devid_map) {

  QMap<int, AnAgent*> sock_map;
  foreach(int id, devid_map) sock_map[id] = NULL;
  
  char *dev_path;
  
  int nFileHandle;
  TPEXTRAPARAMS params;
  char txt_buff[VERSIONSTRING_LEN];
  unsigned int i;
  
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
      fprintf(stderr, "glob[%d] %s\n", i, globb.gl_pathv[i]);
    }
    dev_path = globb.gl_pathv[i];
    h = LINUX_CAN_Open(dev_path, O_RDWR | O_NONBLOCK);
    if (h == NULL) {
      if (TCAN_DEBUG) {
	fprintf(stderr, "cannot open: %s\n", dev_path);
	//				log( QString("cannot open: %1").arg(dev_path) );
      }
      continue;
    }
    nFileHandle = LINUX_CAN_FileHandle(h);
    params.nSubFunction = SF_GET_HCDEVICENO;
    ioctl(nFileHandle, PCAN_EXTRA_PARAMS, &params);
    int dev_id = params.func.ucHCDeviceNo;
    
    if (devid_map.contains(dev_id)) {
      CAN_Init(h, AGENT_PCAN_INIT_BAUD, AGENT_PCAN_INIT_TYPE);
      AnAgent *sock = new AnAgent();
      sock->m_handle = h;
      sock->addr = dev_id;
      sock->dev_path = dev_path;
      sock_map[devid_map[dev_id]] = sock;
      
      if (TCAN_DEBUG) {
	CAN_VersionInfo(h, txt_buff);
	printf("handle:   %p\n", h);
	printf("dev_id:   %x\n", dev_id);
	printf("dev_name: %s\n", dev_path);
	printf("version_info: %s\n", txt_buff);
      }
    } else { // this dev_id is not requested
      CAN_Close(h);
    }
  }
  globfree(&globb);
  
  foreach(int id, devid_map) {
    if (sock_map[id] == NULL)
      qFatal("Device %d is not found.", id);
    sock_map[id]->setId(id);
  }
  return sock_map;
}

//-----------------------------------------------------------------------------
int AnAgent::open(quint8 dev_id) {
  char *dev_path;

  int nFileHandle;
  TPEXTRAPARAMS params;
  char txt_buff[VERSIONSTRING_LEN];
  unsigned int i;

  //WORD wBTR0BTR1 = CAN_BAUD_1M;         /* 250K, 500K */
  WORD wBTR0BTR1 = CAN_BAUD_500K;         /* 250K, 500K */
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
    nFileHandle = LINUX_CAN_FileHandle(h);
    params.nSubFunction = SF_GET_HCDEVICENO;
    ioctl(nFileHandle, PCAN_EXTRA_PARAMS, &params);

    if (dev_id != params.func.ucHCDeviceNo) {
      CAN_Close(h);
      h = NULL;
      continue;
    }

    if (TCAN_DEBUG) {
      CAN_Init(h, wBTR0BTR1, nExtended);
      CAN_VersionInfo(h, txt_buff);
      printf("handle:   %p\n", h);
      printf("dev_id:   %x\n", dev_id);
      printf("dev_name: %s\n", dev_path);
      printf("version_info: %s\n", txt_buff);
    }
    this->m_handle = h;
    this->addr = dev_id;
    this->dev_path = dev_path;
    break;
  }

  globfree(&globb);
  m_handle = h;

  if (m_handle == NULL)
    return -1;
  else
    return 0;
}

//-----------------------------------------------------------------------------
void AnAgent::init(int mode, int level, QList<AnBoard*> list)
{
	m_mode  = mode;
	m_level = level;
	m_list  = list;
}

//-----------------------------------------------------------------------------
void AnAgent::run()
{
  m_root->disableWatch(m_id);
  m_cancel = false;
  
  int total = m_list.count();
  int step  = 0;
  
  emit init(m_id);
  
  switch (m_mode) {

  case AnRoot::TASK_INIT :
    log( QString("TASK_INIT: level=%1").arg(m_level));
    foreach(AnBoard *brd, m_list) {
      if (m_cancel) return;
      brd->init(m_level);
      emit progress(m_id, 100*(++step)/total);
    }
    break;
  
  case AnRoot::TASK_CONFIG :
    log( QString("TASK_CONFIG: level=%1").arg(m_level));
    foreach(AnBoard *brd, m_list) {
      if (m_cancel) return;
      brd->config(m_level);
      emit progress(m_id, 100*(++step)/total);
    }
    break;
    
  case  AnRoot::TASK_RESET :
    log( QString("TASK_RESET: level=%1").arg(m_level));
    foreach(AnBoard *brd, m_list) {
      if (m_cancel) return;
      brd->reset(m_level);
      emit progress(m_id, 100*(++step)/total);
    }
    break;
  
  case AnRoot::TASK_QRESET :  // Quick Reset
    log( QString("TASK_QRESET: level=%1").arg(m_level));
    foreach(AnBoard *brd, m_list) {
      if (m_cancel) return;
      brd->qreset(m_level);
      emit progress(m_id, 100*(++step)/total);
    }
    break;
  
  case AnRoot::TASK_SYNC :
    log( QString("TASK_SYNC: level=%1").arg(m_level));
    foreach(AnBoard *brd, m_list) {
      if (m_cancel) return;
      brd->sync(m_level);
      emit progress(m_id, 100*(++step)/total);
    }
    break;
  
  case AnRoot::TASK_RELINK :
    log( QString("TASK_RELINK: level=%1").arg(m_level));
    foreach(AnBoard *brd, m_list) {
      if (m_cancel) return;
      AnTcpu *tcpu = dynamic_cast<AnTcpu*>( brd );
      if (tcpu) {
	log( QString("TASK_RELINK: %1").arg(tcpu->name()) );
	tcpu->relink(m_level);
      }
      emit progress(m_id, 100*(++step)/total);
    }
    break;
  
  case AnRoot::TASK_TCPURECOVERY :
    log( QString("TASK_TCPURECOVERY: level=%1").arg(m_level));
    foreach(AnBoard *brd, m_list) {
      if (m_cancel) return;
      AnTcpu *tcpu = dynamic_cast<AnTcpu*>( brd );
      if (tcpu) {
	bool cond = false;
	cond |= (m_level >= 1 && tcpu->status() == AnBoard::STATUS_ERROR);
	cond |= (m_level >= 2 && tcpu->status() == AnBoard::STATUS_WARNING);
	cond |= (m_level >= 3);
	if (cond) {
	  log( QString("TASK_TCPURECOVERY: %1").arg(tcpu->name()) );
	  tcpu->sync(3);
	  log( tcpu->errorDump() );
	  tcpu->init(2);
	  // this just resets the TDCs on all TDIGs, not necessary when doing bunch reset
	  // tcpu->qreset(2);

	  //tcpu->config(1);
	  // try using relink instead:
	  tcpu->relink(1);

	  tcpu->sync(3);
	}
      }
      emit progress(m_id, 100*(++step)/total);
    }
    // after all is done, sleep a little to make sure serdes sync happened properly
    // usleep(700000); // done now via database action item "USLEEP" (106)
    break;
  
  case AnRoot::TASK_THUBRECOVERY :
    log( QString("TASK_THUBRECOVERY: level=%1").arg(m_level));
    foreach(AnBoard *brd, m_list) {
      if (m_cancel) return;
      AnThub *thub = dynamic_cast<AnThub*>( brd );
      if (thub) {
	thub->sync(1);
	// log( QString("TASK_THUBRECOVERY: %1 CRC = %2").arg(thub->name()).arg(thub->ecsrString()) );
	if (thub->ecsr() != 0) {
	  log( QString("TASK_THUBRECOVERY: %1 CRC Error, CRC = %2").arg(thub->name()).arg(thub->ecsrString()) );
	  thub->reloadFPGAs(1);
	  for (int i=1; i<=8; i++)
	    (thub->serdes(i))->config(1);
	  usleep(200000); // sleep a little to make sure the sync happened properly
	  thub->sync(3);
	}
	
      }
      emit progress(m_id, 100*(++step)/total);
    }
    break;

  } // end "switch ("
  
  // make sure send out finish
  emit progress(m_id, 100);
  emit finished(m_id);
  m_root->enableWatch(m_id);
}

//-----------------------------------------------------------------------------
void AnAgent::setTdcConfigs(const QMap<int, AnTdcConfig*>& tcnfs)
{
	m_tcnfs.clear();
	foreach(int k, tcnfs.keys()) { // copy each item
		m_tcnfs[k] = new AnTdcConfig(tcnfs[k], this);
	}
}

// private functions

//-----------------------------------------------------------------------------
bool AnAgent::match(TPCANMsg &snd, TPCANMsg &rcv)
{
	if(snd.MSGTYPE == MSGTYPE_EXTENDED) {
		return ((snd.ID | 0x40000) == rcv.ID);
	} else {
		return ((snd.ID | 0x1) == rcv.ID);
	}
}

//-----------------------------------------------------------------------------
void AnAgent::error_handle(int er, TPCANMsg &msg)
{
  if (er == 0 && (msg.MSGTYPE & MSGTYPE_STATUS)) {
    int status = CAN_Status(m_handle);
    if (status & CAN_ERR_ANYBUSERR) {
      //WORD wBTR0BTR1 = CAN_BAUD_1M;         /* 250K, 500K */
      WORD wBTR0BTR1 = CAN_BAUD_500K;         /* 250K, 500K */
      int nExtended  = CAN_INIT_TYPE_EX;    /* CAN_INIT_TYPE_ST */
      incCommError();
      er = CAN_Init(m_handle, wBTR0BTR1, nExtended);
      throw AnExCanError(status);
    }
  } else if (er != 0) {
    //    incCommError();
    int status = CAN_Status(m_handle);
    if (status == CAN_ERR_QRCVEMPTY) {
      log( QString("error_handle: CANBus[%1] QRCVEMPTY Error: 0x%2")
	   .arg(addr).arg(QString::number(status, 16)) );
      throw AnExCanError(status);
      //			throw AnExCanTimeOut(status);
    } else if (status > 0) {
      incCommError();
      log( QString("error_handle: CANBus[%1] other Error: 0x%2")
	   .arg(addr).arg(QString::number(status, 16)) );
      throw AnExCanError(status);
    } else {
      incCommError();
      log( QString("error_handle: CANBus[%1] System Error: 0x%2")
	   .arg(addr).arg(QString::number(status, 16)) );
      throw AnExCanError(status);
    }
  }
}

void AnAgent::pre_check()
{
//	qDebug() << "AnAgent::pre_check(): commError()" << commError();

	if(m_handle == NULL) {
		qDebug() << "device is not open";
		throw AnExCanError(0);
	}

	TPCANRdMsg rmsg;
	int er = LINUX_CAN_Read_Timeout(m_handle, &rmsg, 0);

	if (er == 0 && (rmsg.Msg.MSGTYPE & MSGTYPE_STATUS))
	{
		int status = CAN_Status(m_handle);
		if (status & CAN_ERR_ANYBUSERR) {
			incCommError();
			er = CAN_Init(m_handle, AGENT_PCAN_INIT_BAUD, AGENT_PCAN_INIT_TYPE);
			
		}
	}

	if (commError() >= AGENT_COMM_ERROR_THRESHOLD) {
		throw AnExCanError(0);
	}
}

void AnAgent::log(QString str)
{
	foreach(QString s, str.split("\n")) {
		m_root->log(QString("AnAgent[%1]: " + s).arg(m_id));
	}
}
