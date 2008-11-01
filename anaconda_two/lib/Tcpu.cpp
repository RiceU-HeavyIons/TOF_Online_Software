/*
 * Tcpu.cpp
 *
 *  Created on: Oct 31, 2008
 *      Author: koheik
 */

#include "Tcpu.h"

Tcpu::Tcpu() {

  for (uint8 i = 0; i < 8; i++) {
    tdig[i] = new Tdig();
    tdig[i]->set_tcpu(this);
    tdig[i]->set_id(i + 1);
    tdig[i]->set_addr(0x10 + i);
  }
}

Tcpu::~Tcpu() {
  for (uint8 i = 0; i < 8; i++) {
    delete tdig[i];
  }

}

Tcan *Tcpu::set_tcan(Tcan *tc) {
  tcan = tc;
  for (uint8 i = 0; i < 8; i++) {
    tdig[i]->set_tcan(tcan);
  }
  return tcan;
}

void Tcpu::update_status(int level)
{
  if (enable && level >= 0) {
    Tcan *tcan = get_tcan();
    uint8 ctcpu = get_addr();

    TPCANMsg    msg;
    TPCANRdMsg  rmsg;
    uint64 rdata;

    Tcan::set_msg(msg, ctcpu << 4 | 0x4, MSGTYPE_STANDARD, 1, 0xb0);
    rdata = tcan->write_read(msg, rmsg, 8);
    ecsr = rmsg.Msg.DATA[3];
    temp = (double)rmsg.Msg.DATA[2] + (double)(rmsg.Msg.DATA[1])/100.0;

    Tcan::set_msg(msg, ctcpu << 4 | 0x4, MSGTYPE_STANDARD, 1, 0xb1);
    rdata = tcan->write_read(msg, rmsg, 4);
    this->fw_id = (0xFFFFFF & rdata);

    if (--level >= 0)
      for(uint8 i = 0; i < 8; ++i) tdig[i]->update_status(level);
  }
}

std::string Tcpu::firmware_id() {
  char buff[32];
  sprintf(buff, "%x%c/%x", (fw_id >> 8) & 0xff, fw_id & 0xff, (fw_id >> 16) & 0xff);
  return std::string(buff);
}

void Tcpu::dump() {
  printf("Tcpu:#0x%llx\n", this);
  printf("  ID                 : %d\n",     fid);
  printf("  UT ID              : %d\n",     futid);
  printf("  CANBus Address     : 0x%02x\n", addr);
  printf("  Enable             : %d\n",     enable);
  printf("  Firmware ID        : %s\n",     firmware_id().c_str());
  printf("  Temperature        : %fC\n",    temp);
  printf("  ESCR               : 0x%02x\n", ecsr);
}
