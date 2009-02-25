/*
 * $Id$
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */
#include "AnRoot.h"
#include "AnTcpu.h"
#include "AnExceptions.h"
//-----------------------------------------------------------------------------
AnTcpu::AnTcpu(
	const AnAddress &laddr,
	const AnAddress &haddr,
	AnCanObject *parent) : AnBoard(laddr, haddr, parent),
	m_tray_id(0)
{
	setObjectName(QString("TCPU ") + lAddress().toString());
	setName(QString("Tray %1").arg(lAddress().at(1)));

	AnAddress lad = lAddress();
	AnAddress had = hAddress();

	for (int i = 0; i < 8; ++i) {
		lad.set(2, i + 1);
		had.set(2, 0x10 + i);
		m_tdig[i] = new AnTdig(lad, had, this);
	}
	m_tray_sn = "";
	m_chipid = 0;
	
	m_pld02    = 0;
	m_pld02Set = 0;
	m_pld03    = 1;
	m_pld03Set = 1;
	m_pld0e    = 0;
	m_pld0eSet = 0;
}

//-----------------------------------------------------------------------------
AnTcpu::~AnTcpu()
{
  for(int i = 0; i < 8; ++i)
    delete m_tdig[i];
}

AnCanObject *AnTcpu::at(int i)
{
	if(i >= 1 && i <= 9)
		return m_tdig[i - 1];
	else
		return this;
}

AnCanObject *AnTcpu::hat(int i)
{
	if(i >= 0x10 && i <= 0x17)
		return m_tdig[i - 0x10];
	else
		return this;
}

//-----------------------------------------------------------------------------
void AnTcpu::config(int level)
{
//	qDebug() << "AnTcpu::config: " << laddr();
//	qDebug() << "AnTcpu::config: level = " << level;
//	qDebug() << "AnTcpu::config: commError = " << commError();
	
	if (active() && level >= 1 && commError() == 0) {
		if(--level >= 1) {
			for (int i = 0; i < 8; ++i) {
				m_tdig[i]->config(level);
			}
		}
		// catch final messages
		// for (int i = 0; i < 8; ++i)
		// agent()->read(rmsg);
		// write to PLD REG[2]
		try {
			TPCANMsg    msg;
			TPCANRdMsg  rmsg;
			AnAgent::set_msg(msg, canidw(), MSGTYPE_STANDARD, 3, 0xe, 0x02, m_pld02Set);
			agent()->write_read(msg, rmsg, 2);
		} catch (AnExCanError ex) {
			qDebug() << "CAN error occurred: " << ex.status();
			incCommError();
		}

	}
}

//-----------------------------------------------------------------------------
void AnTcpu::init(int level)
{
	// don't want to init TCPUs for upVPD
	if (haddr().at(1) == 0x20) return;

	if (active() && level >= 1 && commError() == 0) {

		TPCANMsg    msg;
		TPCANRdMsg  rmsg;

		try {
			AnAgent::set_msg(msg, canidw(),
			                 MSGTYPE_STANDARD, 5, 0x8a, 0x69, 0x96, 0xa5, 0x5a);
//			                 MSGTYPE_STANDARD, 5, 0x7f, 0x69, 0x96, 0xa5, 0x5a);
			agent()->write_read(msg, rmsg, 3);

			if (--level >= 1)
				for (int i = 0; i < 8; ++i) m_tdig[i]->init(level);
		} catch (AnExCanError ex) {
			qDebug() << "CAN error occurred: " << ex.status();
			incCommError();
		}
	}
}

//-----------------------------------------------------------------------------
void AnTcpu::reset(int level)
{

	if (active() && level >= 1) {

	    TPCANMsg    msg;
	    TPCANRdMsg  rmsg;

		try {
			// this may not implemented yet
			AnAgent::set_msg(msg, canidw(),
		                 	MSGTYPE_STANDARD, 5, 0xe, 0x1, 0x3, 0x1, 0x0);
			agent()->write_read(msg, rmsg, 2);

			if (--level >= 1)
				for (int i = 0; i < 8; ++i) m_tdig[i]->reset(level);

			clearCommError();
		} catch (AnExCanError ex) {
			qDebug() << "CAN error occurred: " << ex.status();
			incCommError();
		}
	}
}

//-----------------------------------------------------------------------------
void AnTcpu::sync(int level)
{
	if (active() && level >= 1 && commError() == 0) {

		TPCANMsg    msg;
		TPCANRdMsg  rmsg;
		quint64     rdata;

		try {
			// get temperature and ecsr
			// HLP 3f says "ECSR Temp Temp AD1L AD1H AD2L AD2H"...
			AnAgent::set_msg(msg, canidr(), MSGTYPE_STANDARD, 1, 0xb0);
			agent()->write_read(msg, rmsg, 8);
			setEcsr(rmsg.Msg.DATA[3]);
			setTemp((double)rmsg.Msg.DATA[2] + (double)(rmsg.Msg.DATA[1])/100.0);

			if (level >= 3) {
				// get firmware versions
				AnAgent::set_msg(msg, canidr(), MSGTYPE_STANDARD, 1, 0xb1);
				rdata = agent()->write_read(msg, rmsg, 4);
				setFirmwareId(0xFFFFFF & rdata);

				// get chip id
				AnAgent::set_msg(msg, canidr(), MSGTYPE_STANDARD, 1, 0xb2);
				rdata = agent()->write_read(msg, rmsg, 8);
				setChipId(0xFFFFFFFFFFFFFFULL & (rdata >> 8));
			}

			// get PLD 0x02 and 0x0e
			// AnAgent::set_msg(msg, ctcpu << 4 | 0x4, MSGTYPE_STANDARD, 3, 0xe, 0x2, 0xe);
			// sock->write_read(msg, rmsg, 5);
			// m_pld02 = rmsg.Msg.DATA[2];
			// m_pld0e = rmsg.Msg.DATA[4];
			AnAgent::set_msg(msg, canidr(), MSGTYPE_STANDARD, 2, 0xe, 0x2);
			agent()->write_read(msg, rmsg, 3);
			m_pld02 = rmsg.Msg.DATA[2];

			AnAgent::set_msg(msg, canidr(), MSGTYPE_STANDARD, 2, 0xe, 0x3);
			agent()->write_read(msg, rmsg, 3);
			m_pld03 = rmsg.Msg.DATA[2];

			if (--level >= 1)
				for(quint8 i = 0; i < 8; ++i) m_tdig[i]->sync(level);

			setSynced();
		} catch (AnExCanError ex) {
			qDebug() << "CAN error occurred: " << ex.status();
			incCommError();
		}
	}
}

//-----------------------------------------------------------------------------
void AnTcpu::resync(int level)
{
	
	if (active() && level >= 1 && commError() == 0) {
		try {
			TPCANMsg    msg;
			TPCANRdMsg  rmsg;
			AnAgent::set_msg(msg, canidw(), MSGTYPE_STANDARD, 3, 0xe, 0x2, 0x0);
			agent()->write_read(msg, rmsg, 2);
			AnAgent::set_msg(msg, canidw(), MSGTYPE_STANDARD, 3, 0xe, 0x2, m_pld02Set);
			agent()->write_read(msg, rmsg, 2);
		} catch (AnExCanError ex) {
			qDebug() << "CAN error occurred: " << ex.status();
			incCommError();
		}
	}
}


/**
 * Return CANBus id for read
**/
quint32 AnTcpu::canidr() const
{
	return haddr().at(1) << 4 | 0x4;
}

quint32 AnTcpu::canidw() const
{
	return haddr().at(1) << 4 | 0x2;
}

AnAgent *AnTcpu::agent() const
{
	return dynamic_cast<AnRoot*>(parent())->agent(hAddress().at(0));
}

//-----------------------------------------------------------------------------
bool AnTcpu::setInstalled(bool b) {

	AnCanObject::setInstalled(b);
	for (int i = 0; i < 8; ++i)
		m_tdig[i]->setInstalled(installed());
		
	return installed();
}

bool AnTcpu::setActive(bool b) {

	AnCanObject::setActive(b);
	for (int i = 0; i < 8; ++i)
		m_tdig[i]->setActive(active());

	return active();
}

QString AnTcpu::dump() const
{
	QStringList sl;

	sl << QString().sprintf("AnTcpu(%p):", this);
	sl << QString("  Name              : ") + name();
	sl << QString("  Hardware Address  : ") + haddr().toString().toStdString().c_str();
	sl << QString("  Logical Address   : ") + laddr().toString().toStdString().c_str();
	sl << QString("  Installed         : ") + (installed() ? "yes" : "no");
	sl << QString("  Active            : ") + (active() ? "yes" : "no");
	sl << QString("  Synchronized      : ") + synced().toString();
	sl << QString("  Tray ID           : ") + trayIdString();
	sl << QString("  Tray SN           : ") + traySn();
	sl << QString("  Firmware ID       : ") + firmwareString();
	sl << QString("  Chip ID           : ") + chipIdString();
	sl << QString("  Temperature       : ") + tempString();
	sl << QString("  Temperature Alarm : ") + tempAlarmString();
	sl << QString("  ECSR              : 0x") + QString::number(ecsr(), 16);
	sl << QString("  PLD Reg[02]       : 0x") + QString::number(m_pld02, 16);
	sl << QString("  PLD Reg[02] Set   : 0x") + QString::number(m_pld02Set, 16);
	sl << QString("  PLD Reg[03]       : 0x") + QString::number(m_pld03, 16);
	sl << QString("  PLD Reg[03] Set   : 0x") + QString::number(m_pld03Set, 16);
	sl << QString("  PLD Reg[0E]       : 0x") + QString::number(m_pld0e, 16);
	sl << QString("  PLD Reg[0E] Set   : 0x") + QString::number(m_pld0eSet, 16);
	sl << QString("  Status            : ") + QString::number(status());
	sl << QString("  East / West       : ") + (isEast()? "East" : "West");
	sl << QString("  LV / HV           : ") + lvHvString();

	return sl.join("\n");
}

//-----------------------------------------------------------------------------
QString AnTcpu::ecsrString() const
{
  static const char* msg_list[] = {
      "PLD_CONFIG_DONE",
      "PLD_INIT_DONE",
      "PLD_CRC_ERROR",
      "PLD_nSTATUS",
      "Pushbutton",
      "JU2 Jumper 5-6",
      "JU2 Jumper 3-4",
      "JU2 Jumper 1-2", NULL };

  quint8 bts = ecsr();
  QString ret = "<h4>ECSR BITS for TCPU</h4>\n";

  ret += "<table>\n";
  for (int i = 0; i < 8; ++i)
   ret += QString("<tr><td>[%1]</td><td>%2</td><td>= %3</td></tr>\n").
         arg(i).arg(msg_list[i]).arg((bts >> i) & 0x1);
  ret += "</table>\n";

  return ret;
}

//-----------------------------------------------------------------------------
double AnTcpu::maxTemp() const
{
	double t = temp();
	for (int i = 0; i < 8; ++i)
		if (t < m_tdig[i]->temp()) t = m_tdig[i]->temp();

	return t;
}

int AnTcpu::status() const
{
	int stat, err = 0;

	if (temp() > tempAlarm()) ++err;
	if (ecsr() & 0x4) ++err; // PLD_CRC_ERROR
	
	if (m_pld02 != m_pld02Set) ++err;
	if (m_pld03 != m_pld03Set) ++err;
	
	for (int i = 0; i < 8; ++i)
		if (m_tdig[i]->status() == STATUS_ERROR) ++err;

	if (err)
		stat = STATUS_ERROR;
	else if (commError() != 0)
		stat = STATUS_COMM_ERR;
	else
		stat = (m_pld02 & 0x1) ? STATUS_ON : STATUS_STANBY;

	if (!active()) stat = STATUS_UNKNOWN;

	return stat;
}

//-----------------------------------------------------------------------------
QString AnTcpu::pldReg02String(bool hlite) const
{
	// QString ret = "0x" + QString::number(m_pld02, 16) + ", "
	//             + "0x" + QString::number(m_pld0e, 16);
	QString ret = "0x" + QString::number(m_pld02, 16);
	ret += " (0x" + QString::number(m_pld02Set, 16) + ")";

	if (hlite && m_pld02 != m_pld02Set)
		ret = QString("<font color='red'>%1</font>").arg(ret);

	return ret;
}

//-----------------------------------------------------------------------------
QString AnTcpu::pldReg03String(bool hlite) const
{
	QString ret = "0x" + QString::number(m_pld03, 16);
	ret += " (0x" + QString::number(m_pld03Set, 16) + ")";
	
	if (hlite && m_pld03 != m_pld03Set)
		ret = QString("<font color='red'>%1</font>").arg(ret);

	return ret;
}

//-----------------------------------------------------------------------------
QString AnTcpu::pldReg0eString(bool hlite) const
{
	QString ret = "0x" + QString::number(m_pld0e, 16);
	ret += " (0x" + QString::number(m_pld0eSet, 16) + ")";

	if (hlite && m_pld0e != m_pld0eSet)
		ret = QString("<font color='red'>%1</font>").arg(ret);
	
	return ret;
}

//-----------------------------------------------------------------------------
void AnTcpu::setLvHv(int lb, int lc, int hb, int hc)
{
	m_lv_box = lb;
	m_lv_ch  = lc;
	m_hv_box = hb;
	m_hv_ch  = hc;
}

/**
 * Return LV / HV String
**/
QString AnTcpu::lvHvString() const
{
	QString ret;
	if(active())
		ret = QString("PS%1-U%2 / BOX%3-%4")
		 .arg(m_lv_box).arg(m_lv_ch).arg(m_hv_box).arg(m_hv_ch);
	return ret;
}

/**
 * Return LV tring
**/
QString AnTcpu::lvString() const
{
	QString ret;
	if(active())
		ret = QString("PS%1-U%2").arg(m_lv_box).arg(m_lv_ch);
	return ret;
}
/**
 * Return HV String
**/
QString AnTcpu::hvString() const
{
	QString ret;
	if(active())
		ret = QString("BOX%3-%4").arg(m_hv_box).arg(m_hv_ch);
	return ret;
}

bool AnTcpu::fibermode() const
{
	return active() && (m_pld02Set & 0x8);
}
