/*
 * $Id$
 *
 *  Created on: Nov 29, 2008
 *      Author: koheik
 */
#include <iostream>
#include <cerrno>
using namespace std;

#include <QtCore/QMutex>

#include "AnAgent.h"
#include "AnRoot.h"
#include "AnExceptions.h"

int AnAgent::TCAN_DEBUG = 0;

//-----------------------------------------------------------------------------
AnAgent::AnAgent(QObject *parent) : QThread(parent), m_handle(0), m_comm_err(0)
{}

//-----------------------------------------------------------------------------
AnAgent::~AnAgent()
{
  if (m_handle) CAN_Close(m_handle);
}

//-----------------------------------------------------------------------------
void AnAgent::set_msg(struct can_frame &msg, ...)
{
  va_list argptr;
  va_start(argptr, msg);

  msg.can_id = va_arg(argptr, int) | va_arg(argptr, int);
  //msg.MSGTYPE = va_arg(argptr, int);
  msg.can_dlc     = va_arg(argptr, int);
  for (int i = 0; i < msg.can_dlc && i < 8; ++i)
    msg.data[i] = va_arg(argptr, int);

  va_end(argptr);
}

//-----------------------------------------------------------------------------
void AnAgent::print(const struct can_frame &msg)
{
  int i;

  printf("CANMsg ID: 0x%x DATA[%d]: ",
	 msg.can_id, msg.can_dlc);
  for (i = 0; i < msg.can_dlc && i < 8; i++)
    printf("0x%02x ", msg.data[i]);
  puts("");
  fflush(stdout);
}

//-----------------------------------------------------------------------------
// void AnAgent::print(const TPCANRdMsg &rmsg)
// {
//   print(rmsg.Msg);
// }

// member functions

QMutex mtex;
void AnAgent::print_send(const struct can_frame &msg)
{
  mtex.lock();
  printf("[%d] >> ", m_id);
  print(msg);
  mtex.unlock();
}

//-----------------------------------------------------------------------------
void AnAgent::print_recv(const struct can_frame &msg)
{
  mtex.lock();
  printf("[%d] << ", m_id);
  print(msg);
  mtex.unlock();
}

//-----------------------------------------------------------------------------
quint64 AnAgent::read(struct can_frame &rmsg,
		      int return_length, unsigned int time_out)
{
  //	pre_check();

  quint64 data = 0;
  unsigned int length = 0;
  unsigned int niter = return_length / 8 + ((return_length % 8)? 1 : 0);
  int er = 0;

  for (unsigned int i = 0; i < niter && !er; ++i) {
    er = LINUX_CAN_Read_Timeout(m_handle, &rmsg, time_out);
    error_handle(er, rmsg);

    if (TCAN_DEBUG) {
      print_recv(rmsg);
      emit debug_recv(AnRdMsg(devid(), rmsg));
    }

    length += rmsg.can_dlc;
    for (int j = 1; j < rmsg.can_dlc; ++j)
      data |= static_cast<quint64>(rmsg.data[j]) << 8 * (7*i + j-1);
  }

  if (return_length >= 0 && return_length != (int)length) {
    log( QString("read return length doesn't match: expected %1, received %2")
	 .arg(return_length).arg(length) );
    throw AnExCanError(0);
  }

  return data;
}

//-----------------------------------------------------------------------------
void AnAgent::raw_write(struct can_frame &msg, int time_out)
{
  //pre_check();

  int er;

  if (TCAN_DEBUG) {
    print_send(msg);
    emit debug_send(AnRdMsg(devid(), msg));
  }

  er = LINUX_CAN_Write_Timeout(m_handle, &msg, time_out);
  error_handle(er, msg);
}

//-----------------------------------------------------------------------------
quint64 AnAgent::write_read(struct can_frame &msg, struct can_frame &rmsg,
			    unsigned int return_length, unsigned int time_out)
{
  //pre_check();

  quint64 data = 0;
  unsigned int length = 0;
  unsigned int niter;
  int er;
  int ntry;

  niter = return_length / 8 + ((return_length % 8)? 1 : 0);
  if (TCAN_DEBUG) {
    print_send(msg);
    emit debug_send(AnRdMsg(devid(), msg));
  }

  if (commError() >= AGENT_COMM_ERROR_THRESHOLD) {
    throw AnExCanError(-1);
  }
  
  er = CAN_Write(m_handle, &msg);
  error_handle(er, msg);
  clearCommError(); // since it succeeded

  er = 1;
  for (unsigned int i = 0; i < niter && (er > 0); ++i) {
    // try 3 times
    for (ntry=3; ntry > 0; ntry--) {
      er = LINUX_CAN_Read_Timeout(m_handle, &rmsg, time_out);
      //if (er == CAN_ERR_QRCVEMPTY) {
      if (er == 0) {
	// timeout
	log(QString("write_read: read timeout: latest msg " + AnRdMsg(addr, msg).toString()));
	//er = 0;
	// try sending message again:
	er = CAN_Write(m_handle, &msg);
	// retry reading
      }
      else {
	error_handle(er, rmsg);

	if (TCAN_DEBUG) {
	  print_recv(rmsg);
	  emit debug_recv(AnRdMsg(devid(), rmsg));
	}

	if (match(msg, rmsg)) {
	  break;
	}
	else {
	  // emit received(AnRdMsg(devid(), rmsg));
	}
      }
    }

    if (ntry == 0) {
      log( QString("Didn't receive reply message after 3 retries for msg with ID 0x%1")
	   .arg(QString::number(msg.can_id,16)) );
      // since this is most likely a board specific communication error,
      // don't increase the agent communication error count
      //incCommError(); 
      throw AnExCanError(0);
    }
    else {
      if (rmsg.data[0] != msg.data[0] && ((rmsg.can_id >> 4) != 0x40)) { // not a THUB
	log( QString("Return payload doesn't match: expected 0x%1, received 0x%2")
	     .arg(QString::number(msg.data[0],16)).arg(QString::number(rmsg.data[0],16)) );
	//incCommError();
	throw AnExCanError(0);
      }
      length += rmsg.can_dlc;
      for(int j = 1; j < rmsg.can_dlc; ++j)
	data |= static_cast<quint64>(rmsg.data[j]) << 8 * (7*i + j-1);
    }
  }

  
  if ( (er > 0) && (return_length != length)) {
    log( QString("write_read error %3 return length doesn't match: expected %1, received %2")
	 .arg(return_length).arg(length).arg(er) );
    //incCommError();
    throw AnExCanError(0);
  }

  return data;
}
//-----------------------------------------------------------------------------
QMap<int, AnAgent*> AnAgent::open(QMap<int, int>& devid_map) {
  int h = -1;
  QMap<int, AnAgent*> sock_map;

  foreach(int id, devid_map) sock_map[id] = NULL;
  
  QMapIterator<int, int> i(devid_map);
  while (i.hasNext()) {
    i.next();
    int id = i.value();
    int dev_id = i.key();
    
    if((h = CAN_Open(dev_id)) < 0) {
      qFatal("Device %d is not found.", i.value());
    }
    else {
      char dev_path[7];
      sprintf(dev_path, "can%d", dev_id);

      AnAgent *sock = new AnAgent();
      sock->m_handle = h;
      sock->addr = dev_id;
      sock->dev_path = dev_path;
      sock_map[id] = sock;
      sock_map[id]->setId(id);
    }
  }

  return sock_map;
}

//-----------------------------------------------------------------------------
int AnAgent::open(quint8 dev_id) {
  char dev_path[7];
  int h = -1;

  sprintf(dev_path, "can%d", dev_id);

  if((h = CAN_Open(dev_id)) < 0) {
    qFatal("Device %d is not found.", dev_id);
  }

  this->m_handle = h;
  this->addr = dev_id;
  this->dev_path = dev_path;

  m_handle = h;
  
  if (m_handle <= 0)
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
	    // tcpu->sync(3); // JS: don't need another syn here (?)
	    log( tcpu->errorDump() );
	    tcpu->init(2);
	    // this just resets the TDCs on all TDIGs, not necessary when doing bunch reset
	    // tcpu->qreset(2);

	    if ((tcpu->lAddress().at(1) == 121) || (tcpu->lAddress().at(1) == 122)) 
	      // so that this doesn't interfere with the other TCPUs on agent 1 or 2
	      // just do the config (just turns on link on TCPU)
	      tcpu->config(1);
	    else
	      // try using relink for all others, which also resets serdes on THUB:
	      tcpu->relink(1);

	    tcpu->sync(2);
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
  bool AnAgent::match(struct can_frame &snd, struct can_frame &rcv)
  {
    if((snd.can_id & MSGTYPE_EXTENDED) == MSGTYPE_EXTENDED) {
      return ((snd.can_id | 0x40000) == rcv.can_id);
    }
    else {
      return ((snd.can_id | 0x1) == rcv.can_id);
    }
  }

//-----------------------------------------------------------------------------
void AnAgent::error_handle(int er, struct can_frame &msg)
{
  if (er == 0) {
    // timeout
    log( QString("error_handle: CANBus[%1] Timeout Error")
	 .arg(addr));
    log(QString("error_handle: latest msg " + AnRdMsg(addr, msg).toString()));
  }
  else if (er < 0) {
    incCommError();
    // log( QString("error_handle: CANBus[%1] other Error: 0x%2")
    // 	 .arg(addr).arg(QString::number(er, 16)) );
    log( QString("error_handle: CANBus[%1]  error: %2")
	 .arg(addr).arg(QString(strerror(errno))));
    throw AnExCanError(errno);
    
  }

#ifdef NOTNOW
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
    } 
    else if (er != 0) {
      //    incCommError();
      int status = CAN_Status(m_handle);
      if (status == CAN_ERR_QRCVEMPTY) {
	log( QString("error_handle: CANBus[%1] QRCVEMPTY Error: 0x%2")
	     .arg(addr).arg(QString::number(status, 16)) );
	log(QString("error_handle: latest msg " + AnRdMsg(addr, msg).toString()));

	// 3/30/2012: try to just log this error, but let agent retry?
	// i.e. don't throw AnExCanError
	// throw AnExCanError(status); 
	// throw AnExCanTimeOut(status);
      }
      else if (status > 0) {
	incCommError();
	log( QString("error_handle: CANBus[%1] error: 0x%2")
	     .arg(addr).arg(QString::number(status, 16)) );
	throw AnExCanError(status);
      }
      else {
	incCommError();
	log( QString("error_handle: CANBus[%1] System Error: 0x%2")
	     .arg(addr).arg(QString::number(status, 16)) );
	throw AnExCanError(status);
      }
    }
#endif
}

void AnAgent::pre_check()
{
  //	qDebug() << "AnAgent::pre_check(): commError()" << commError();
  
  if(m_handle == NULL) {
    qDebug() << "device is not open";
    throw AnExCanError(0);
  }
  
  struct can_frame rmsg;
  int er = LINUX_CAN_Read_Timeout(m_handle, &rmsg, 0);
  
  if (er <= 0) {
    incCommError();
  }
  
#ifdef NOTNOW
  if (er == 0 && (rmsg.Msg.MSGTYPE & MSGTYPE_STATUS)) {
    int status = CAN_Status(m_handle);
    if (status & CAN_ERR_ANYBUSERR) {
      incCommError();
      er = CAN_Init(m_handle, AGENT_PCAN_INIT_BAUD, AGENT_PCAN_INIT_TYPE);
    }
  }
#endif
  
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
