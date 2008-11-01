/*
 * Tdig.cpp
 *  Created on: Oct 31, 2008
 *      Author: koheik
 */
#include "Tcpu.h"
#include "Tdig.h"

Tdig::Tdig() {

    for (uint8 i = 0; i < 3; i++) {
      ttdc[i] = new Ttdc();
      ttdc[i]->set_tdig(this);
      ttdc[i]->set_addr(i + 1);
    }
}

Tdig::~Tdig() {

  for (uint8 i = 0; i < 3; ++i) {
    delete ttdc[i];
  }
}

void Tdig::update_status(int level)
{
  if (enable && level >= 0) {
    Tcan  *tcan = get_tcan();
    uint8  ctcpu = get_tcpu()->get_addr();
    uint8  ctdig = get_addr();
    uint32 cid = ((ctdig << 4 | 0x4) << 18 ) | ctcpu;

    TPCANMsg    msg;
    TPCANRdMsg  rmsg;
    uint64 rdata;


    Tcan::set_msg(msg, cid, MSGTYPE_EXTENDED, 1, 0xb0);
    rdata = tcan->write_read(msg, rmsg, 8);
    ecsr = rmsg.Msg.DATA[3];
    temp = (double)rmsg.Msg.DATA[2] + (double)(rmsg.Msg.DATA[1])/100.0;

    Tcan::set_msg(msg, cid, MSGTYPE_EXTENDED, 1, 0xb1);
    rdata = tcan->write_read(msg, rmsg, 4);
    this->fw_id = (0xFFFFFF & rdata);

    if (--level >= 0)
      for(uint8 i = 0; i < 3; ++i) ttdc[i]->update_status(level);
  }
}

std::string Tdig::firmware_id() {
  char buff[32];
  sprintf(buff, "%x%c/%x", (fw_id >> 8) & 0xff, fw_id & 0xff, (fw_id >> 16) & 0xff);
  return std::string(buff);
}

void Tdig::dump() {
  printf("Tdig:#0x%llx\n", (uint64)(this));
  printf("  ID                 : %d\n", fid);
  printf("  UT ID              : %d\n", futid);
  printf("  CANBus Address     : 0x%02x\n", addr);
  printf("  Firmware ID        : %s\n", firmware_id().c_str());
  printf("  Temperature        : %fC\n", temp);
  printf("  ESCR               : 0x%02x\n", ecsr);
}

int Tdig::set_threshold(double threshold) {

  Tcan  *tcan = get_tcan();
  uint8  ctcpu = get_tcpu()->get_addr();
  uint8  ctdig = get_addr();
  uint32 cid = ((ctdig << 4 | 0x2) << 18 ) | ctcpu;

  TPCANMsg    msg;
  TPCANRdMsg  rmsg;

  uint16 val = (uint16)(threshold * 4095.0 / 3300.0 + 0.5);

  Tcan::set_msg(msg, cid, MSGTYPE_EXTENDED, 3, 0x8, (val & 0xff), ((val >> 8) & 0xf));
  tcan->write_read(msg, rmsg, 2);

  return 0;
}

int Tdig::reset_tdc(uint8 tdc) {

  Tcan  *tcan  = get_tcan();
  uint8  ctcpu = get_tcpu()->get_addr();
  uint8  ctdig = get_addr();
  uint32 cid   = ((ctdig << 4 | 0x2) << 18 ) | ctcpu;
  uint8  ctdc  = 0x4 | tdc;

  TPCANMsg    msg;
  TPCANRdMsg  rmsg;

  Tcan::set_msg(msg, cid, MSGTYPE_EXTENDED, 6, ctdc, 0xe4, 0xff, 0xff, 0xff, 0xff);
  tcan->write_read(msg, rmsg, 2);
  Tcan::set_msg(msg, cid, MSGTYPE_EXTENDED, 6, ctdc, 0xe4, 0xff, 0xff, 0xff, 0x3f);
  tcan->write_read(msg, rmsg, 2);
  Tcan::set_msg(msg, cid, MSGTYPE_EXTENDED, 6, ctdc, 0xe4, 0xff, 0xff, 0xff, 0x9f);
  tcan->write_read(msg, rmsg, 2);
  Tcan::set_msg(msg, cid, MSGTYPE_EXTENDED, 6, ctdc, 0xf4, 0xff, 0xff, 0xff, 0x1f);
  tcan->write_read(msg, rmsg, 2);
  Tcan::set_msg(msg, cid, MSGTYPE_EXTENDED, 6, ctdc, 0xe4, 0xff, 0xff, 0xff, 0x9f);
  tcan->write_read(msg, rmsg, 2);

  return 0;
}
