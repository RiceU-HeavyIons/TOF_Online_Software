/*
 * Ttray.h
 *
 *  Created on: Oct 31, 2008
 *      Author: koheik
 */

#ifndef TTRAY_H_
#define TTRAY_H_

#include <string>
#include "Tcan.h"
#include "Tcpu.h"
#include "Tdig.h"

class Ttray {
  uint8         utid;
  std::string   sn;
  uint8         fid;    // STAR Position ID

  Tcan          *tcan;
  Tcpu          *tcpu;

  bool          enable;

public:
  Ttray();
  virtual ~Ttray();

  uint8  set_utid(uint8 uid) { return (utid = uid); }
  uint8  get_utid() { return utid; }

  std::string set_sn(std::string s) { return (sn = s); }
  std::string get_sn() { return sn; }

  uint8  set_id(uint8 i) { return (fid = i); }
  uint8  id() { return fid; }

  uint8 set_addr(uint8 addr) { return tcpu->set_addr(addr); }
  uint8 get_addr() { return tcpu->get_addr(); }

  bool set_enable(bool en) { return tcpu->set_enable(en); }
  bool get_enable() { return tcpu->get_enable(); }

  void set_tcan(Tcan *tc);
  Tcan *get_tcan();

  Tcpu *get_tcpu() { return tcpu; }

  void update_status(int level = 0) { tcpu->update_status(level); }
  void configure(Ttdc::config *cfig1, Ttdc::config *cfig2);
};

#endif /* TTRAY_H_ */
