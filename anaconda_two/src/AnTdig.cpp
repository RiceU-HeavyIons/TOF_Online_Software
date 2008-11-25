/*
 * AnTdig.cpp
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#include "AnRoot.h"
#include "AnTdig.h"

AnTdig::AnTdig(const AnAddress& laddr, const AnAddress& haddr, AnCanObject *parent)
  : AnBoard(laddr, haddr, parent), m_threshold(0)
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
    quint8  ctcpu = hAddress().at(1);
    quint8  ctdig = hAddress().at(2);
    quint32 cid = ((ctdig << 4 | 0x4) << 18 ) | ctcpu;
    AnSock *sock = dynamic_cast<AnRoot*>(root())->sock(hAddress().at(0));

    TPCANMsg    msg;
    TPCANRdMsg  rmsg;
    quint64     rdata;


    AnSock::set_msg(msg, cid, MSGTYPE_EXTENDED, 1, 0xb0);
    rdata = sock->write_read(msg, rmsg, 8, 800000);
    setTemp((double)rmsg.Msg.DATA[2] + (double)(rmsg.Msg.DATA[1])/100.0);
    setEcsr(rmsg.Msg.DATA[3]);

    AnSock::set_msg(msg, cid, MSGTYPE_EXTENDED, 1, 0xb1);
    rdata = sock->write_read(msg, rmsg, 4, 800000);
    setFirmwareId(0xFFFFFF & rdata);

    if (--level >= 0)
      for(quint8 i = 1; i < 4; ++i) m_tdc[i]->sync(level);
  }
}

void AnTdig::reset()
{
	
	if (active()) {
		quint8  devid = hAddress().at(0);
		quint8  ctcpu = hAddress().at(1);
	    quint8  ctdig = hAddress().at(2);
	    quint32 cid = ((ctdig << 4 | 0x2) << 18 ) | ctcpu;

	    TPCANMsg    msg;
	    TPCANRdMsg  rmsg;
	    AnSock *sock = dynamic_cast<AnRoot*>(root())->sock(devid);

		// this may not implemented yet
	    AnSock::set_msg(msg, cid, MSGTYPE_EXTENDED, 5, 0x7f, 0x69, 0x96, 0xa5, 0x5a);
	    sock->write_read(msg, rmsg, 2);

		m_tdc[0]->reset();
//		for(int i = 1; i < 4; ++i) m_tdc[i]->reset();
	}
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
	else
		stat = (ecsr() & 0x10) ? STATUS_ON : STATUS_STANBY;

	return stat;	
}