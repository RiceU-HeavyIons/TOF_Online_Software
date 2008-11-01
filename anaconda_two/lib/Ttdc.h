/*
 * Ttdc.h
 *
 *  Created on: Oct 31, 2008
 *      Author: koheik
 */

#ifndef TTDC_H_
#define TTDC_H_

#include "tof_types.h"
#include "Tcomp.h"

class Tdig;

class Ttdc : public Tcomp {

  Tdig     *tdig;

  bool   parity;
  bool   dll_lock;

  bool   trigger_fifo_empty;
  bool   trigger_fifo_full;
  uint8  trigger_fifo_occupancy;

  uint8  group0_l1_occupancy;
  uint8  group1_l1_occupancy;
  uint8  group2_l1_occupancy;
  uint8  group3_l1_occupancy;

  bool   readout_fifo_empty;
  bool   readout_fifo_full;
  uint16 readout_fifo_occupancy;

  bool   have_token;

  uint16 status;

public:
  Ttdc();
  virtual ~Ttdc();

  Tdig *set_tdig(Tdig *tdg) { return (tdig = tdg); }

  void parse_status(uint64 stat);
  void print_status();

  void update_status(int level = 0);

  class config { };
};

#endif /* TTDC_H_ */
