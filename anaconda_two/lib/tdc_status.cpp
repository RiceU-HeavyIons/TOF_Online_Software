/*
 * tdc_status.cpp
 *
 *  Created on: Oct 30, 2008
 *      Author: koheik
 */

#include <cstdio>
#include "tdc_status.h"

tdc_status::tdc_status(uint64 stat) {

  parity                 = static_cast<bool>   (0x001 & (stat >> 61));
  dll_lock               = static_cast<bool>   (0x001 & (stat >> 60));
  trigger_fifo_empty     = static_cast<bool>   (0x001 & (stat >> 59));
  trigger_fifo_full      = static_cast<bool>   (0x001 & (stat >> 58));
  trigger_fifo_occupancy = static_cast<uint8>  (0x00F & (stat >> 54));
  group0_l1_occupancy    = static_cast<uint8>  (0x0FF & (stat >> 46));
  group1_l1_occupancy    = static_cast<uint8>  (0x0FF & (stat >> 38));
  group2_l1_occupancy    = static_cast<uint8>  (0x0FF & (stat >> 30));
  group3_l1_occupancy    = static_cast<uint8>  (0x0FF & (stat >> 22));
  readout_fifo_empty     = static_cast<bool>   (0x001 & (stat >> 21));
  readout_fifo_full      = static_cast<bool>   (0x001 & (stat >> 20));
  readout_fifo_occupancy = static_cast<uint16> (0x0FF & (stat >> 12));
  have_token             = static_cast<bool>   (0x001 & (stat >> 11));
  status                 = static_cast<uint16> (0x7FF & stat);
}

tdc_status::~tdc_status() {
  // TODO Auto-generated destructor stub
}

void tdc_status::print() {
  printf("Parity                     [61] = 0x%01x\n", parity);
  printf("DLL lock                   [60] = 0x%01x\n", dll_lock);
  printf("Trigger_fifo_empty         [59] = 0x%01x\n", trigger_fifo_empty);
  printf("Trigger_fifo_full          [58] = 0x%01x\n", trigger_fifo_full);
  printf("Trigger_fifo_occupancy [57..54] = 0x%02x\n", trigger_fifo_occupancy);
  printf("Group0_L1_occupancy    [53..46] = 0x%02x\n", group0_l1_occupancy);
  printf("Group1_L1_occupancy    [45..38] = 0x%02x\n", group1_l1_occupancy);
  printf("Group2_L1_occupancy    [37..30] = 0x%02x\n", group2_l1_occupancy);
  printf("Group3_L1_occupancy    [29..22] = 0x%02x\n", group3_l1_occupancy);
  printf("Readout_fifo_empty         [21] = 0x%01x\n", readout_fifo_empty);
  printf("Readout_fifo_full          [20] = 0x%01x\n", readout_fifo_full);
  printf("Readout_fifo_occupancy [19..12] = 0x%02x\n", readout_fifo_occupancy);
  printf("Have token                 [11] = 0x%01x\n", have_token);
  printf("Status                 [10..00] = 0x%04x\n", status);
}

