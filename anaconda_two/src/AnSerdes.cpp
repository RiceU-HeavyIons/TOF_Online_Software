/*
 * AnSerdes.cpp
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

void AnSerdes::sync(int level)
{
  if (active() && level >= 0) {
    quint8  devid = hAddress().at(0);
	quint32 canid = (hAddress().at(1) << 4) | 0x4;
	quint8  srdid = hAddress().at(2); 
    AnSock *sock = static_cast<AnRoot*>(parent()->parent())->sock(devid);

    TPCANMsg    msg;
    TPCANRdMsg  rmsg;
    quint64     rdata;

    AnSock::set_msg(msg, canid, MSGTYPE_STANDARD, 2, 0x02, srdid);
    rdata = sock->write_read(msg, rmsg, 3);
    setFpgaFirmwareId(rmsg.Msg.DATA[2]);

    AnSock::set_msg(msg, canid, MSGTYPE_STANDARD, 1, 0x90 + srdid);
    rdata = sock->write_read(msg, rmsg, 2);
    setEcsr(rmsg.Msg.DATA[1]);
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