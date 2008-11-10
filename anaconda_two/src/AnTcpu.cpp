/*
 * AnTcpu.cpp
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */
#include "AnRoot.h"
#include "AnSock.h"
#include "AnTcpu.h"

AnTcpu::AnTcpu(const AnLAddress &laddr, const AnHAddress &haddr,
    AnCanObject *parent) :
  AnBoard(laddr, haddr, parent)
{
  setObjectName(QString("TCPU ") + lAddress().toString());

  AnLAddress lad = lAddress();
  AnHAddress had = hAddress();

  for(int i = 0; i < 8; ++i) {
    lad.set(1, i+1);
    had.set(2, 0x10+i);
    m_tdig[i] = new AnTdig(lad, had, this);
  }
}

AnTcpu::~AnTcpu()
{
  for(int i = 0; i < 8; ++i)
    delete m_tdig[i];
}



void AnTcpu::sync(int level)
{
  if (enable && level >= 0) {
    quint8 ctcpu = hAddress().at(1);

    TPCANMsg    msg;
    TPCANRdMsg  rmsg;
    quint64 rdata;
    AnSock *sock = static_cast<AnRoot*>(parent())->sock(0);

    AnSock::set_msg(msg, ctcpu << 4 | 0x4, MSGTYPE_STANDARD, 1, 0xb0);
    rdata = sock->write_read(msg, rmsg, 8);
    setEcsr(rmsg.Msg.DATA[3]);
    setTemp((double)rmsg.Msg.DATA[2] + (double)(rmsg.Msg.DATA[1])/100.0);

    AnSock::set_msg(msg, ctcpu << 4 | 0x4, MSGTYPE_STANDARD, 1, 0xb1);
    rdata = sock->write_read(msg, rmsg, 4);
    setFirmwareId(0xFFFFFF & rdata);

    if (--level >= 0)
      for(quint8 i = 0; i < 8; ++i) m_tdig[i]->sync(level);
  }
}

//
//std::string AnBoard:firmware_id() {
//  char buff[32];
//  sprintf(buff, "%x%c/%x", (fw_id >> 8) & 0xff, fw_id & 0xff, (fw_id >> 16) & 0xff);
//  return std::string(buff);
//}

void AnTcpu::dump() const {
  printf("Tcpu:#0x%llx\n", reinterpret_cast<quint64>(this));
//  printf("  ID                 : %d\n",     fid);
//  printf("  UT ID              : %d\n",     futid);
  printf("  Hardware Address   : %s\n", hAddress().toString().toStdString().c_str());
  printf("  Logical Address    : %s\n", lAddress().toString().toStdString().c_str());
  printf("  Enable             : %d\n",     enable);
  printf("  Firmware ID        : %s\n",     firmwareId());
  printf("  Temperature        : %fC\n",    temp());
  printf("  ESCR               : 0x%02x\n", ecsr());
}
