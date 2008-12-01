/*
 * $Id$
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#include "AnRoot.h"
#include "AnTdig.h"

AnTdig::AnTdig(const AnAddress& laddr, const AnAddress& haddr, AnCanObject *parent)
  : AnBoard(laddr, haddr, parent), m_threshold(0), m_chipid(0)
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

void AnTdig::sync(int level)
{
  if (active() && level >= 0) {

    TPCANMsg    msg;
    TPCANRdMsg  rmsg;
    quint64     rdata;

    AnAgent::set_msg(msg, canidr(), MSGTYPE_EXTENDED, 1, 0xb0);
    rdata = agent()->write_read(msg, rmsg, 8);
    setTemp((double)rmsg.Msg.DATA[2] + (double)(rmsg.Msg.DATA[1])/100.0);
    setEcsr(rmsg.Msg.DATA[3]);

    if (level >= 1) {
		// get firmware version
		AnAgent::set_msg(msg, canidr(), MSGTYPE_EXTENDED, 1, 0xb1);
	    rdata = agent()->write_read(msg, rmsg, 4);
	    setFirmwareId(0xFFFFFF & rdata);

		// get chip id
	    AnAgent::set_msg(msg, canidr(), MSGTYPE_EXTENDED, 1, 0xb2);
	    rdata = agent()->write_read(msg, rmsg, 8);
	    setChipId(0xFFFFFFFFFFFFFFULL & (rdata >> 8));
	}

    if (--level >= 0)
      for(quint8 i = 1; i < 4; ++i) m_tdc[i]->sync(level);
  }
}

void AnTdig::reset()
{

	if (active()) {
	    TPCANMsg    msg;
	    TPCANRdMsg  rmsg;

		// this may not implemented yet
	    AnAgent::set_msg(msg, canidw(), MSGTYPE_EXTENDED, 5, 0x7f, 0x69, 0x96, 0xa5, 0x5a);
	    agent()->write_read(msg, rmsg, 2);

		m_tdc[0]->reset();
//		for(int i = 1; i < 4; ++i) m_tdc[i]->reset();
	}
}
void AnTdig::config()
{
	if (active()) {
		if( tdc(1)->configId() == tdc(2)->configId() &&
		    tdc(2)->configId() == tdc(3)->configId() )
		{
			tdc(0)->setConfigId(tdc(1)->configId());
			tdc(0)->config();
		} else {
			for(int i = 1; i < 4; i++) m_tdc[i]->config();
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
bool AnTdig::setActive(bool act) {

	AnCanObject::setActive(act);

	if(!active()) {
		for (int i = 0; i < 4; ++i)
			m_tdc[i]->setActive(false);
	}
}

//-----------------------------------------------------------------------------
QString AnTdig::dump() const
{
	QStringList sl;

	sl << QString().sprintf("AnTdig(%p):", this);
	sl << QString("  Name             : ") + name();
	sl << QString("  Hardware Address : ") + haddr().toString().toStdString().c_str();
	sl << QString("  Logical Address  : ") + laddr().toString().toStdString().c_str();
	sl << QString("  Active           : ") + (active() ? "yes" : "no");
	sl << QString("  Firmware ID      : ") + firmwareString();
	sl << QString("  Chip ID          : ") + chipIdString();
	sl << QString("  Temperature      : ") + tempString();
	sl << QString("  ECSR             : 0x") + QString::number(ecsr(), 16);
	sl << QString("  Threshold        : ") + thresholdString();
	sl << QString("  Status           : ") + QString::number(status());
	sl << QString("  East / West      : ") + (isEast()? "East" : "West");

	return sl.join("\n");
}

QString AnTdig::ecsrString() const
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

//-----------------------------------------------------------------------------
int AnTdig::status() const
{
	int stat, err = 0;

	if (temp() > 40.0) ++err;
	if (ecsr() & 0x4) ++err; // PLD_CRC_ERROR

	if (err)
		stat = STATUS_ERROR;
	else if (!active())
		stat = STATUS_UNKNOWN;
	else if (ecsr() & 0x10)
		stat = STATUS_ON;
	else
		stat = STATUS_STANBY;

	return stat;
}
