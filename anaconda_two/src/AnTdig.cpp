/*
 * $Id$
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#include "AnRoot.h"
#include "AnTdig.h"
#include "AnExceptions.h"

AnTdig::AnTdig(const AnAddress& laddr, const AnAddress& haddr, AnCanObject *parent)
  : AnBoard(laddr, haddr, parent), m_threshold(0), m_chipid(0), m_pld03(0), m_pld03Set(0)
{
	setObjectName(QString("TDIG ") + lAddress().toString());

	AnAddress lad = lAddress();
	AnAddress had = hAddress();

// broad cast address
	lad.set(3, 255);
	had.set(3, 0);
	m_tdc[0] = new AnTdc(lad, had, this);

	for(int i = 1; i < 4; ++i) {
		lad.set(3, i);
		had.set(3, i);
		m_tdc[i] = new AnTdc(lad, had, this);
	}
}

AnTdig::~AnTdig()
{
  for(int i = 0; i < 4; ++i)
    delete m_tdc[i];
}

AnCanObject *AnTdig::hat(int i)
{
	if(i >= 1 && i <= 3)
		return m_tdc[i];
	else
		return this;
}

void AnTdig::sync(int level)
{
	if (active() && level >= 1 && commError() == 0) {

		TPCANMsg    msg;
		TPCANRdMsg  rmsg;
		quint64     rdata;

		try {
			AnAgent::set_msg(msg, canidr(), MSGTYPE_EXTENDED, 1, 0xb0);
			rdata = agent()->write_read(msg, rmsg, 8);
			setTemp((double)rmsg.Msg.DATA[2] + (double)(rmsg.Msg.DATA[1])/100.0);
			setEcsr(rmsg.Msg.DATA[3]);

			AnAgent::set_msg(msg, canidr(), MSGTYPE_EXTENDED, 2, 0xe, 0x3);
			agent()->write_read(msg, rmsg, 3);
			m_pld03 = rmsg.Msg.DATA[2];

			if (level >= 3) {
			// get firmware version
				AnAgent::set_msg(msg, canidr(), MSGTYPE_EXTENDED, 1, 0xb1);
				rdata = agent()->write_read(msg, rmsg, 4);
				setFirmwareId(0xFFFFFF & rdata);

			// get chip id
				AnAgent::set_msg(msg, canidr(), MSGTYPE_EXTENDED, 1, 0xb2);
				rdata = agent()->write_read(msg, rmsg, 8);
				setChipId(0xFFFFFFFFFFFFFFULL & (rdata >> 8));
			}

			if (--level >= 1) {
				for (quint8 i = 1; i < 4; ++i) m_tdc[i]->sync(level);
			}
			setSynced();

		} catch (AnExCanError ex) {
			qDebug() << "Can Communication Error Occurred: " << ex.status();
			incCommError();
		}
	}
}

/**
 * Initialized TDIG
**/

void AnTdig::init(int level)
{
	if (active() && level >= 1) {
		TPCANMsg    msg;
		TPCANRdMsg  rmsg;
		try {
			// this might not be implemented yet
//			AnAgent::set_msg(msg, canidw(), MSGTYPE_EXTENDED, 5, 0x7f, 0x69, 0x96, 0xa5, 0x5a);
			AnAgent::set_msg(msg, canidw(), MSGTYPE_EXTENDED, 5, 0x89, 0x69, 0x96, 0xa5, 0x5a);
			agent()->write_read(msg, rmsg, 3);

			if(--level >= 1) m_tdc[0]->init(level);
		// for(int i = 1; i < 4; ++i) m_tdc[i]->reset();
		} catch (AnExCanError ex) {
			incCommError();
			qDebug() << "Communication Error Occurred: " << ex.status();
		}
	}
}

/**
 * Configure TDIG and TDCs
**/
void AnTdig::config(int level)
{
	if (active() && level >= 1 && commError() == 0) {
		try {
			if (--level >= 1) { /* configure tdcs */
				if( tdc(1)->configId() == tdc(2)->configId() &&
				tdc(2)->configId() == tdc(3)->configId() )
				{
					tdc(0)->setConfigId(tdc(1)->configId());
					tdc(0)->config(level);
				} else {
					for(int i = 1; i < 4; i++) m_tdc[i]->config(level);
				}
			}

		// write threshold
			TPCANMsg msg;
			TPCANRdMsg rmsg;
			quint16 val = (quint16)(threshold() * 4095.0 / 3300.0 + 0.5);
			quint8  vl  = (val & 0xff);
			quint8  vh  = ((val >> 8) & 0x0f);
			AnAgent::set_msg(msg, canidw(), MSGTYPE_EXTENDED, 3, 0x08, vl, vh);
			agent()->write_read(msg, rmsg, 2);
		} catch (AnExCanError ex) {
			incCommError();
			qDebug() << "Communication Error Occurred: " << ex.status();
		}
	}
}
/**
 * Rest TDIG
**/
void AnTdig::reset(int level)
{
//	qDebug() << "AnTdig::reset" << laddr();
	if (active() && level >= 1) {
		clearCommError();
		try {
		// do nothing here
		// TPCANMsg    msg;
		// TPCANRdMsg  rmsg;

		// AnAgent::set_msg(msg, canidw(), MSGTYPE_EXTENDED, 1, 0x90);
		// agent()->write_read(msg, rmsg, 2);

			if (--level >= 1) {
				m_tdc[0]->reset(level);
			// for(int i = 1; i < 4; ++i) m_tdc[i]->reset();
			}

		} catch (AnExCanError ex) {
			incCommError();
			qDebug() << "Communication Error Occurred: " << ex.status();
		}
	}
}
/**
 * Quick Rest TDIG
**/
void AnTdig::qreset(int level)
{
//	qDebug() << "AnTdig::reset" << laddr();
	if (active() && level >= 1) {
		try {
			// do nothing here
			TPCANMsg    msg;
			TPCANRdMsg  rmsg;

			AnAgent::set_msg(msg, canidw(), MSGTYPE_EXTENDED, 5, 0xe, 0x2, 0x1, 0x2, 0x0);
			agent()->write_read(msg, rmsg, 2);

			// if (--level >= 1) {
			// 	m_tdc[0]->reset(level);
			// for(int i = 1; i < 4; ++i) m_tdc[i]->reset();
			// }

			clearCommError();
		} catch (AnExCanError ex) {
			incCommError();
			qDebug() << "Communication Error Occurred: " << ex.status();
		}
	}
}


//-----------------------------------------------------------------------------
quint32 AnTdig::canidr() const
{
	return (haddr().at(2) << 4 | 0x4) << 18 | haddr().at(1);
}

quint32 AnTdig::canidw() const
{
	return (haddr().at(2) << 4 | 0x2) << 18 | haddr().at(1);
}

quint32 AnTdig::cantyp() const
{
	return MSGTYPE_EXTENDED;
}

AnAgent *AnTdig::agent() const
{
	return dynamic_cast<AnRoot*>(parent()->parent())->agent(hAddress().at(0));
}

//-----------------------------------------------------------------------------
bool AnTdig::setInstalled(bool b) {

	AnCanObject::setInstalled(b);
	for (int i = 0; i < 4; ++i)
		m_tdc[i]->setInstalled(installed());

	return installed();
}

//-----------------------------------------------------------------------------
bool AnTdig::setActive(bool b) {

	AnCanObject::setActive(b);
	for (int i = 0; i < 4; ++i)
		m_tdc[i]->setActive(active());

	return active();
}

//-----------------------------------------------------------------------------
QString AnTdig::dump() const
{
	QStringList sl;

	sl << QString().sprintf("AnTdig(%p):", this);
	sl << QString("  Name              : ") + name();
	sl << QString("  Hardware Address  : ") + haddr().toString().toStdString().c_str();
	sl << QString("  Logical Address   : ") + laddr().toString().toStdString().c_str();
	sl << QString("  Installed         : ") + (installed() ? "yes" : "no");
	sl << QString("  Active            : ") + (active() ? "yes" : "no");
	sl << QString("  Synchronized      : ") + synced().toString();
	sl << QString("  Firmware ID       : ") + firmwareString();
	sl << QString("  Chip ID           : ") + chipIdString();
	sl << QString("  Temperature       : ") + tempString();
	sl << QString("  Temperature Alarm : ") + tempAlarmString();
	sl << QString("  ECSR              : 0x") + QString::number(ecsr(), 16);
	sl << QString("  PLD Reg[03]       : 0x") + QString::number(m_pld03, 16);
	sl << QString("  PLD Reg[03] Set   : 0x") + QString::number(m_pld03Set, 16);
	sl << QString("  Threshold         : ") + thresholdString();
	sl << QString("  Status            : ") + QString::number(status());
	sl << QString("  East / West       : ") + (isEast()? "East" : "West");


	return sl.join("\n");
}


QString AnTdig::errorDump() const
{
	QStringList sl;

	sl << QString().sprintf("AnTdig(%p):", this);
	sl << QString("  Name               : ") + name();
	sl << QString("  Temperature        : ") + tempString();
	sl << QString("  ECSR               : 0x") + QString::number(ecsr(), 16);
	sl << QString("  TDIG PLD Reg[03]   : 0x") + QString::number(m_pld03, 16);
	sl << QString("  Status             : ") + QString::number(status());
	for (int i = 1; i <= 3; ++i) {
		sl << QString("  TDIG %1 Status Word: %2").arg(i).arg(m_tdc[i]->statusString());
	}

	return sl.join("\n");
}

QString AnTdig::ecsrToolTipString() const
{
  static const char* msg_list[] = {
      "PLD_CONFIG_DONE",
      "PLD_INIT_DONE",
      "PLD_CRC_ERROR",
      "PLD_nSTATUS",
      "TDC_POWER_ERROR_B",
      "ENABLE_TDC_POWER",
      "TINO_TEST_MCU",
      "SPARE_PLD", NULL };

  quint8 bts = ecsr();
  QString ret = "<h4>ECSR BITS for TDIG</h4>\n";

  ret += "<table>\n";
  for (int i = 0; i < 8; ++i)
   ret += QString("<tr><td>[%1]</td><td>%2</td><td>= %3</td></tr>\n").
         arg(i).arg(msg_list[i]).arg((bts >> i) & 0x1);
  ret += "</table>\n";

  return ret;	
}

QString AnTdig::ecsrString(bool hilit) const
{
	QString ret = "0x" + QString::number(ecsr(), 16);

	if (hilit && (ecsr() & 0x4))
		ret = QString("<font color='red'>%1</font>").arg(ret);

	return ret;
}

//-----------------------------------------------------------------------------
QString AnTdig::pldReg03String(bool hlite) const
{
	QString ret = "0x" + QString::number(m_pld03, 16);
//	ret += " (0x" + QString::number(m_pld03Set, 16) + ")";
	
	if (hlite && m_pld03 != m_pld03Set)
		ret = QString("<font color='red'>%1</font>").arg(ret);

	return ret;
}
//-----------------------------------------------------------------------------
QString AnTdig::pldReg03ToolTipString() const
{
  static const char* msg_list[] = {
      "TDC 1 Error",
      "TDC 2 Error",
      "TDC 3 Error",
       NULL
  };

  quint8 bts = pldReg03();
  QString ret = "<h4>TDC ERROR BITS</h4>\n";

  ret += "<table>\n";
  for (int i = 0; i < 3; ++i)
   ret += QString("<tr><td>[%1]</td><td>%2</td><td>= %3</td></tr>\n").
         arg(i).arg(msg_list[i]).arg((bts >> i) & 0x1);
  ret += "</table>\n";

  return ret;
}

//-----------------------------------------------------------------------------
int AnTdig::status() const
{
	int stat;
	int err = 0;
	int warn = 0;

	if (temp() > tempAlarm()) ++err;
	if (ecsr() & 0x4) ++warn; // PLD_CRC_ERROR

	if (m_pld03 != m_pld03Set) ++err; // TDC 1-3 Error

	if (agent()->commError() != 0 || commError() != 0)
		stat = STATUS_COMM_ERR;
	else if (err)
		stat = STATUS_ERROR;
	else if (warn)
		stat = STATUS_WARNING;
	else if (!active())
		stat = STATUS_UNKNOWN;
	else if (ecsr() & 0x10)
		stat = STATUS_ON;
	else
		stat = STATUS_STANDBY;

	return stat;
}
