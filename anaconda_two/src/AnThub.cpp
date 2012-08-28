/*
 * $Id$
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#include "AnRoot.h"
#include "AnThub.h"
#include "AnExceptions.h"

//-----------------------------------------------------------------------------
AnThub::AnThub(const AnAddress &laddr, const AnAddress &haddr, AnCanObject *parent)
 : AnBoard(laddr, haddr, parent)
{
	setObjectName(QString("THUB ") + lAddress().toString());
	setName(QString("THUB %1").arg(lAddress().at(1)));
	AnAddress lad = lAddress();
	AnAddress had = hAddress();

	for(int i = 0; i < 8; ++i) {
		lad.set(2, i+1);
		had.set(2, i+1);
		m_serdes[i] = new AnSerdes(lad, had, this);
	}
}

//-----------------------------------------------------------------------------
AnThub::~AnThub()
{
	// do nothing
}

//-----------------------------------------------------------------------------
AnCanObject *AnThub::at(int i)
{
	if (i >= 1 && i <= 8)
		return m_serdes[i - 1];
	return
		this;
}

//-----------------------------------------------------------------------------
AnCanObject *AnThub::hat(int i)
{
	if (i >= 1 && i <= 8)
		return m_serdes[i - 1];
	return
		this;
}

//-----------------------------------------------------------------------------
QString AnThub::firmwareString() const
{
  char buf[32];
  sprintf(buf, "%x/%x", m_thubMCUFirmware, m_thubFPGAFirmware);
  return QString(buf);
}

//-----------------------------------------------------------------------------
QString AnThub::ecsrString(bool hilit) const
{
	QString ret = "0x" + QString::number(ecsr(), 16);
	if (hilit && (ecsr() != 0))
		ret = QString("<font color='red'>%1</font>").arg(ret);

	return ret;
}

//-----------------------------------------------------------------------------
void AnThub::config(int level)
{
  if (active() && level >= 1) {
    // make sure no communication errors are present
    clearCommError();
    
    // no other config needed currently
  }	
}

//-----------------------------------------------------------------------------
QString AnThub::dump() const
{
	QStringList sl;

	sl << QString().sprintf("AnThub(%p):", this);
	sl << QString("  Name              : ") + name();
	sl << QString("  Hardware Address  : ") + haddr().toString().toStdString().c_str();
	sl << QString("  Logical Address   : ") + laddr().toString().toStdString().c_str();
	sl << QString("  Installed         : ") + (installed() ? "yes" : "no");
	sl << QString("  Active            : ") + (active() ? "yes" : "no");
	sl << QString("  Synchronized      : ") + synced().toString();
	sl << QString("  Firmware ID       : ") + firmwareString();
	sl << QString("  Temperature 1     : ") + tempString(0);
	sl << QString("  Temperature 2     : ") + tempString(1);
	sl << QString("  Temperature Alarm : ") + tempAlarmString();
	sl << QString("  CRC               : 0x") + QString::number(ecsr(), 16);
	sl << QString("  Status            : ") + QString::number(status());
	sl << QString("  East / West       : ") + (isEast()? "East" : "West");

	return sl.join("\n");
}

//-----------------------------------------------------------------------------
/**
 * Reset THUB
 */
void AnThub::reset(int level)
{
  if (active() && level >= 1) {
    struct can_frame msg;
    struct can_frame rmsg;
    
    try {
      clearCommError();
      
      AnAgent::set_msg(msg, canidw(), MSGTYPE_STANDARD, 4, 0x81, 0x3, 0x81, 0x0);
      agent()->write_read(msg, rmsg, 1);
      
      if (--level >= 1)
	for(int i = 0; i < 8; ++i) m_serdes[i]->reset(level);
      
    } catch (AnExCanError ex) {
      log(QString("reset: CAN error occcured: 0x%1").arg(ex.status(),0,16));
      incCommError();
    }
  } else {
    log(QString("reset: wasn't issued, active=%1, level=%2, commError=%3")
	.arg(active()).arg(level).arg(commError()));
  }
}

//-----------------------------------------------------------------------------
/**
 * Quick Reset THUB
 */
void AnThub::qreset(int level)
{
  if (active() && level >= 1) {
    
    clearCommError();
    
    struct can_frame msg;
    // struct can_frame rmsg;
    
    try {
      AnAgent::set_msg(msg, canidw(), MSGTYPE_STANDARD, 4, 0x81, 0x3, 0x81, 0x0);
      //			agent()->write_read(msg, rmsg, 1);
      
      // if (--level >= 1)
      // 	for(int i = 0; i < 8; ++i) m_serdes[i]->reset(level);
      
    } catch (AnExCanError ex) {
      log(QString("qreset: CAN error occcured: 0x%1").arg(ex.status(),0,16));
      incCommError();
    }
  } else {
    log(QString("qreset: wasn't issued, active=%1, level=%2, commError=%3")
	.arg(active()).arg(level).arg(commError()));
  }
}

//-----------------------------------------------------------------------------
/**
 * Sync THUB
 */
void AnThub::sync(int level)
{
  if (active()) {
    if (level >= 1 && commError() < 10) { // only fail after 10 attempts
      struct can_frame msg;
      struct can_frame rmsg;
      quint64 rdata;
      
      try {
	// readout master firmware id
	if (level >= 3) {
	  m_thubFPGAFirmware = 0;
	  m_thubMCUFirmware = 0;
	  AnAgent::set_msg(msg, canidr(), MSGTYPE_STANDARD, 1, 0x01);
	  agent()->write_read(msg, rmsg, 8);
	  m_thubMCUFirmware = static_cast<quint16>(rmsg.data[1]) << 8 |
	    static_cast<quint16>(rmsg.data[0]);
	  AnAgent::set_msg(msg, canidr(), MSGTYPE_STANDARD, 2, 0x02, 0x00);
	  agent()->write_read(msg, rmsg, 4);
	  for(int j = 0; j < rmsg.can_dlc; ++j)
	    m_thubFPGAFirmware |= static_cast<quint32>(rmsg.data[j]) << 8 * j;
	  //setFpgaFirmwareId(rmsg.Msg.DATA[2]);
	}
	// readout temperature
	for (int i = 0; i < 2; ++i) {
	  AnAgent::set_msg(msg, canidr(), MSGTYPE_STANDARD, 2, 0x03, i + 1);
	  rdata = agent()->write_read(msg, rmsg, 2);
	  setTemp(((double)rmsg.data[1] + (double)(rmsg.data[0])/256.0)*2.0, i);
	  
	}
	// since communication succeeded, make sure commError is cleared
	clearCommError();
	for(int j = 0; j < 2; ++j)
	  agent()->root()->tlog(QString("THUB %1 %2: %3").arg(laddr().at(1)).arg(j+1).arg(temp(j)));
	
	// readout crc error bits
	AnAgent::set_msg(msg, canidr(), MSGTYPE_STANDARD, 1, 0x05);
	rdata = agent()->write_read(msg, rmsg, 2);
	rdata = (rdata << 8) + rmsg.data[0];
	setEcsr(rdata);
	
	// readout serdes infomation
	if (--level >= 1)
	  for(quint8 i = 0; i < 8; ++i) m_serdes[i]->sync(level);
	
	setSynced();
      } catch (AnExCanError ex) {
	QStringList btrace;
	//if (ex.status() == CAN_ERR_QRCVEMPTY) {
	if (ex.status() == 0) {
	  // probably harmless, only print log message
	  log(QString("sync: CAN timeout error occurred"));
	  incCommError();
	  //log(btrace.join("\n"));
	} 
	else {
	  log(QString("sync: CAN error occurred: 0x%1").arg(ex.status(),0,16));
	  log(btrace.join("\n"));
	  incCommError();
	}
	log(QString("sync: latest msg " + AnRdMsg(haddr().at(0), msg).toString() + "\n"));
      }
    } 
    else {
      log(QString("sync: wasn't issued, active=%1, level=%2, commError=%3")
	  .arg(active()).arg(level).arg(commError()));
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * THUB Bunch Reset
 */
void AnThub::bunchReset(int level)
{
  log(QString("bunchReset: level=%1").arg(level));
  
  if (active() && level >= 1 && commError() == 0) {
    struct can_frame msg;
    struct can_frame rmsg;
    
    try {
      AnAgent::set_msg(msg, canidw(), MSGTYPE_STANDARD, 2, 0x99, 0x1);
      agent()->write_read(msg, rmsg, 2);
      
    } catch (AnExCanError ex) {
      log(QString("bunchReset: CAN error occcured: 0x%1").arg(ex.status(),0,16));
      incCommError();
    }
  } else {
    log(QString("bunchReset: wasn't issued, active=%1, level=%2, commError=%3")
	.arg(active()).arg(level).arg(commError()));
  }
}

//-----------------------------------------------------------------------------
/**
 * THUB Reload FPGAs
 */
void AnThub::reloadFPGAs(int level)
{
  log(QString("reloadFPGAs: level=%1").arg(level));
  
  clearCommError();
  if (active() && level >= 1) {
    struct can_frame msg;
    struct can_frame rmsg;
    
    try {
      AnAgent::set_msg(msg, canidw(), MSGTYPE_STANDARD, 1, 0xd);
      agent()->write_read(msg, rmsg, 2);
      
    } catch (AnExCanError ex) {
      log(QString("reloadFPGAs: CAN error occcured: 0x%1").arg(ex.status(),0,16));
      incCommError();
    }
  } else {
    log(QString("reloadFPGAs: wasn't issued, active=%1, level=%2, commError=%3")
	.arg(active()).arg(level).arg(commError()));
  }
}

//-----------------------------------------------------------------------------
/**
 * THUB Turn on (= 0) or off (= 1) the recover alert message
 */
void AnThub::recoverAlertMsg(int val)
{
  log(QString("recoverAlertMsg: val=%1").arg(val));
  
  if (active() && commError() == 0) {
    struct can_frame msg;
    struct can_frame rmsg;
    
    try {
      AnAgent::set_msg(msg, canidw(), MSGTYPE_STANDARD, 2, 0xc, val);
      agent()->write_read(msg, rmsg, 2);
      
    } catch (AnExCanError ex) {
      log(QString("recoverAlertMsg: CAN error occcured: 0x%1").arg(ex.status(),0,16));
      incCommError();
    }
  } else {
    log(QString("recoverAlertMsg: wasn't issued, active=%1, val=%2, commError=%3")
	.arg(active()).arg(val).arg(commError()));
  }
}

//-----------------------------------------------------------------------------
quint32 AnThub::canidr() const
{
	return haddr().at(1) << 4 | 0x4;
}

//-----------------------------------------------------------------------------
quint32 AnThub::canidw() const
{
	return haddr().at(1) << 4 | 0x2;
}

//-----------------------------------------------------------------------------
AnAgent *AnThub::agent() const
{
	return dynamic_cast<AnRoot*>(parent())->agent(hAddress().at(0));
}

//-----------------------------------------------------------------------------
bool AnThub::setInstalled(bool b) {

	AnCanObject::setInstalled(b);
	for (int i = 0; i < 8; ++i)
		m_serdes[i]->setInstalled(installed());

	return installed();
}

//-----------------------------------------------------------------------------
bool AnThub::setActive(bool b) {

	AnCanObject::setActive(b);
	for (int i = 0; i < 8; ++i)
		m_serdes[i]->setActive(active());

	return active();
}

//-----------------------------------------------------------------------------
int AnThub::status() const
{
// TO-DO implement real logic
	if (active()) {
		if (agent()->commError() || commError()) return STATUS_COMM_ERR;

		int err = 0;
		if (maxTemp() > tempAlarm()) ++err;

		int warn = 0;
		if (ecsr() != 0x0) ++warn;

		for (int i = 0; i < 8; ++i)
			if (m_serdes[i]->status() == STATUS_ERROR) ++err;

		if (err) return STATUS_ERROR;
		if (warn) return STATUS_WARNING;

		return STATUS_ON;
	} else {
		return STATUS_UNKNOWN;
	}
}

//-----------------------------------------------------------------------------
void AnThub::log(QString str)
{
	foreach(QString s, str.split("\n")) {
		agent()->root()->log(QString("AnThub[%1]: " + s).arg(laddr().at(1)));
	}
}
