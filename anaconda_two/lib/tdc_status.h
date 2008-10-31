/*
 * tdc_status.h
 *
 *  Created on: Oct 30, 2008
 *      Author: koheik
 */

#ifndef TDC_STATUS_H_
#define TDC_STATUS_H_

#include "tofc_types.h"

class tdc_status {

private:
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
  tdc_status(uint64 stat);
  virtual ~tdc_status();
  void print();
};
/*
Status reply for TDIG Board (node) 16, TDC # 2
Parity   [61] = 1
DLL lock [60] = 1
Trigger_fifo_empty [59]= 1
Trigger_fifo_full  [58]= 0
Trigger_fifo_occupancy [57..54]= 0000 (0 words)
Group0_L1_occupancy [53..46]= 00000001 (1 words)
Group1_L1_occupancy [45..38]= 00000001 (1 words)
Group2_L1_occupancy [37..30]= 00000001 (1 words)
Group3_L1_occupancy [29..22]= 00000001 (1 words)
Readout_fifo_empty [21]= 1
Readout_fifo_full  [20]= 0
Readout_fifo_occupancy [19..11]= 00000000 (0 words)
have_token = 0
Status (Error) [10..0] = 00000000000
*/
#endif /* TDC_STATUS_H_ */
