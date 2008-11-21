/*
 * AnTdig.cpp
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#include "AnRoot.h"
#include "AnTdig.h"

AnTdig::AnTdig(const AnLAddress& laddr, const AnHAddress& haddr, AnCanObject *parent)
  : AnBoard(laddr, haddr, parent)
{
  setObjectName(QString("TDIG ") + lAddress().toString());

  AnLAddress lad = lAddress();
  AnHAddress had = hAddress();

  for(int i = 0; i < 3; ++i) {
    lad.set(2, i+1);
    had.set(3, i+1);
    m_tdc[i] = new AnTdc(lad, had, this);
  }
}

AnTdig::~AnTdig()
{
  for(int i = 0; i < 3; ++i)
    delete m_tdc[i];
}

void AnTdig::sync(int level)
{
  if (enable && level >= 0) {
    quint8  ctcpu = hAddress().at(1);
    quint8  ctdig = hAddress().at(2);
    quint32 cid = ((ctdig << 4 | 0x4) << 18 ) | ctcpu;
    AnSock *sock = static_cast<AnRoot*>(parent()->parent())->sock(hAddress().at(0));

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
      for(quint8 i = 0; i < 3; ++i) m_tdc[i]->sync(level);
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
