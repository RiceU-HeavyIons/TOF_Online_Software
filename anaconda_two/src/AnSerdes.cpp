/*
 * $Id$
 *
 *  Created on: Nov 22, 2008
 *      Author: koheik
 */

#include "AnRoot.h"
#include "AnSerdes.h"

AnSerdes::AnSerdes(const AnAddress& laddr, const AnAddress& haddr, AnCanObject *parent)
 : AnBoard(laddr, haddr, parent)
{
	setObjectName(QString("SERDES ") + lAddress().toString());
	for(int i = 1; i <= NPORT; ++i) setTcpu(i, NULL);
}

AnSerdes::~AnSerdes()
{
}

QString AnSerdes::firmwareString() const
{
  char buf[32];
  sprintf(buf, "%x", fpgaFirmwareId());
  return QString(buf);
}

QString AnSerdes::dump() const
{
	QStringList sl;

	sl << QString().sprintf("AnSerdes(%p):", this);
	sl << QString("  Name             : ") + name();
	sl << QString("  Hardware Address : ") + haddr().toString().toStdString().c_str();
	sl << QString("  Logical Address  : ") + laddr().toString().toStdString().c_str();
	sl << QString("  Active           : ") + (active() ? "yes" : "no");
	sl << QString("  Synchronized     : ") + synced().toString();
	sl << QString("  Firmware ID      : ") + firmwareString();
	sl << QString("  ECSR             : 0x") + QString::number(ecsr(), 16);
	sl << QString("  PLD REG9x Set    : 0x") + QString::number(pld9xSet(), 16);
		sl << QString("  Status           : ") + QString::number(status());
	sl << QString("  East / West      : ") + (isEast()? "East" : "West");
	for(int i = 1; i <= NPORT; ++i)
		sl << QString().sprintf("  Port[%i]          : Tcpu(%p)", i, tcpu(i));

	return sl.join("\n");
}

void AnSerdes::sync(int level)
{
  if (active() && level >= 0) {
	quint8  srdid = hAddress().at(2);

    TPCANMsg    msg;
    TPCANRdMsg  rmsg;
    quint64     rdata;

    AnAgent::set_msg(msg, canidr(), MSGTYPE_STANDARD, 2, 0x02, srdid);
    rdata = agent()->write_read(msg, rmsg, 3);
    setFpgaFirmwareId(rmsg.Msg.DATA[2]);

    AnAgent::set_msg(msg, canidr(), MSGTYPE_STANDARD, 1, 0x90 + srdid);
    rdata = agent()->write_read(msg, rmsg, 2);
    setEcsr(rmsg.Msg.DATA[1]);

    setSynced();
  }
}

QString AnSerdes::ecsrString() const
{
  static const char* msg_list[] = {
      "Channel 0 is On",
      "Channel 1 is On",
      "Channel 2 is On",
      "Channel 3 is On",
      "Test Data(0) / Real Data(1)",
      "NOT USED",
      "NOT USED",
      "NOT USED", NULL };

  quint8 bts = ecsr();
  QString ret = "<h4>SERDES Register</h4>\n";

  ret += "<table>\n";
  for (int i = 0; i < 8; ++i)
   ret += QString("<tr><td>[%1]</td><td>%2</td><td>= %3</td></tr>\n").
         arg(i).arg(msg_list[i]).arg((bts >> i) & 0x1);
  ret += "</table>";

  return ret;
}


//-----------------------------------------------------------------------------
quint32 AnSerdes::canidr() const
{
	return haddr().at(1) << 4 | 0x4;
}

quint32 AnSerdes::canidw() const
{
	return haddr().at(1) << 4 | 0x2;
}

quint32 AnSerdes::cantyp() const
{
	return MSGTYPE_STANDARD;
}

AnAgent *AnSerdes::agent() const
{
	return dynamic_cast<AnRoot*>(parent()->parent())->agent(hAddress().at(0));
}

int AnSerdes::status() const
{
// TO-DO implement real logic
	int err = 0;
	if (ecsr() != pld9xSet()) err++;

	if (active()) {
		if (err) return STATUS_ERROR;
		return STATUS_ON;
	} else
		return STATUS_UNKNOWN;
}

quint8 AnSerdes::pld9xSet() const {
	quint8 bts = m_pld9xBase;
	for(int i = 0; i < 4; ++i)
		if(m_tcpu[i] && m_tcpu[i]->active()) bts |= (1 << i);

	return bts;
}
