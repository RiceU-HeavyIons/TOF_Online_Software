/*
 * $Id$
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */
#include "AnRoot.h"
#include "AnTcpu.h"

//-----------------------------------------------------------------------------
AnTcpu::AnTcpu(
	const AnAddress &laddr,
	const AnAddress &haddr,
    AnCanObject *parent) : AnBoard(laddr, haddr, parent)
{
  setObjectName(QString("TCPU ") + lAddress().toString());

  AnAddress lad = lAddress();
  AnAddress had = hAddress();

  for(int i = 0; i < 8; ++i) {
    lad.set(2, i + 1);
    had.set(2, 0x10 + i);
    m_tdig[i] = new AnTdig(lad, had, this);
  }
}

//-----------------------------------------------------------------------------
AnTcpu::~AnTcpu()
{
  for(int i = 0; i < 8; ++i)
    delete m_tdig[i];
}

//-----------------------------------------------------------------------------
void AnTcpu::sync(int level)
{
  if (active() && level >= 0) {

    TPCANMsg    msg;
    TPCANRdMsg  rmsg;
    quint64     rdata;

	// get temperature and ecsr
	// HLP 3f says "ESCSR Temp Temp AD1L AD1H AD2L AD2H"...
    AnAgent::set_msg(msg, canidr(), MSGTYPE_STANDARD, 1, 0xb0);
    agent()->write_read(msg, rmsg, 8);
    setEcsr(rmsg.Msg.DATA[3]);
    setTemp((double)rmsg.Msg.DATA[2] + (double)(rmsg.Msg.DATA[1])/100.0);

	if (level >= 1) {
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
	//     AnAgent::set_msg(msg, ctcpu << 4 | 0x4, MSGTYPE_STANDARD, 3, 0xe, 0x2, 0xe);
	//     sock->write_read(msg, rmsg, 5);
	// m_pld02 = rmsg.Msg.DATA[2];
	// m_pld0e = rmsg.Msg.DATA[3];
    AnAgent::set_msg(msg, canidr(), MSGTYPE_STANDARD, 2, 0xe, 0x2);
    agent()->write_read(msg, rmsg, 3);
	m_pld02 = rmsg.Msg.DATA[2];

    if (--level >= 0)
      for(quint8 i = 0; i < 8; ++i) m_tdig[i]->sync(level);
  }
}


//-----------------------------------------------------------------------------
void AnTcpu::reset()
{

	if (active()) {

	    TPCANMsg    msg;
	    TPCANRdMsg  rmsg;

		// this may not implemented yet
	    AnAgent::set_msg(msg, canidw(), MSGTYPE_STANDARD,
											5, 0x7f, 0x69, 0x96, 0xa5, 0x5a);
	    agent()->write_read(msg, rmsg, 2);

		for (int i = 0; i < 8; ++i) m_tdig[i]->reset();
	}
}

//-----------------------------------------------------------------------------
void AnTcpu::config()
{
	if (active()) {
		for (int i = 0; i < 8; ++i) {
			m_tdig[i]->config();
		}
		// catch final messages
//		for (int i = 0; i < 8; ++i)
//			agent()->read(rmsg);
	}
}

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
void AnTcpu::dump() const
{
  printf("Tcpu:#0x%p\n", this);
//  printf("  ID                 : %d\n",     fid);
//  printf("  UT ID              : %d\n",     futid);
  printf("  Hardware Address   : %s\n", hAddress().toString().toStdString().c_str());
  printf("  Logical Address    : %s\n", lAddress().toString().toStdString().c_str());
  printf("  Active             : %d\n",     active());
  printf("  Firmware ID        : %x\n",     firmwareId());
  printf("  Temperature        : %fC\n",    temp());
  printf("  ESCR               : 0x%02x\n", ecsr());
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

	if (maxTemp() > 40.0) ++err;
	if (ecsr() & 0x4) ++err; // PLD_CRC_ERROR
	
	if (err)
		stat = STATUS_ERROR;
	else
		stat = (m_pld02 & 0x1) ? STATUS_ON : STATUS_STANBY;

	return stat;
}


//-----------------------------------------------------------------------------
QString AnTcpu::pldRegString() const
{
	// QString ret = "0x" + QString::number(m_pld02, 16) + ", "
	//             + "0x" + QString::number(m_pld0e, 16);
	QString ret = "0x" + QString::number(m_pld02, 16);
	return ret;
}