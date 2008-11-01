/*
 * Tdig.h
 *
 *  Created on: Oct 31, 2008
 *      Author: koheik
 */

#ifndef TDIG_H_
#define TDIG_H_

#include "tof_types.h"
#include "Tcomp.h"
#include "Ttdc.h"

class Tcpu;

class Tdig : public Tcomp {

  Tcpu      *tcpu;
  Ttdc      *ttdc[3];

  // payload=0xb0
  uint32    fw_id;
  uint16    fw_id_mcu;
  uint8     fw_id_fpga;

  // payload=0xb1
  double    temp;
  uint8     ecsr;

public:
  Tdig();
  virtual ~Tdig();

  Tcpu  *set_tcpu(Tcpu *t) { return (tcpu = t); }
  Tcpu  *get_tcpu() { return tcpu; }

  Ttdc *get_tdc(uint8 i) { return ttdc[i]; }

  void update_status(int level = 0);
  std::string firmware_id();
  void dump();

  int set_threshold(double threhosld);
  int reset_tdc(uint8 tdc = 0);
};

#endif /* TDIG_H_ */
