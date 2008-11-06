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
  printf("Parity                     [61] = [%d]\n", parity);
  printf("DLL lock                   [60] = [%d]\n", dll_lock);
  printf("Trigger_fifo_empty         [59] = [%d]\n", trigger_fifo_empty);
  printf("Trigger_fifo_full          [58] = [%d]\n", trigger_fifo_full);
  printf("Trigger_fifo_occupancy [57..54] = 0x%02x\n", trigger_fifo_occupancy);
  printf("Group0_L1_occupancy    [53..46] = 0x%02x\n", group0_l1_occupancy);
  printf("Group1_L1_occupancy    [45..38] = 0x%02x\n", group1_l1_occupancy);
  printf("Group2_L1_occupancy    [37..30] = 0x%02x\n", group2_l1_occupancy);
  printf("Group3_L1_occupancy    [29..22] = 0x%02x\n", group3_l1_occupancy);
  printf("Readout_fifo_empty         [21] = [%d]\n", readout_fifo_empty);
  printf("Readout_fifo_full          [20] = [%d]\n", readout_fifo_full);
  printf("Readout_fifo_occupancy [19..12] = 0x%02x\n", readout_fifo_occupancy);
  printf("Have token                 [11] = [%d]\n", have_token);
  printf("Status                 [10..00] = 0x%04x\n", status);

  const char *error_bits[] =
  {
      "VERNIER ERROR",
      "COURSE ERROR",
      "CHANNEL SELECT ERROR",
      "L1 BUFFER PARITY ERROR",
      "TRIGGER FIFO PARITY ERROR",
      "TRIGGER MATCHING STATE ERROR",
      "READOUT FIFO PARITY ERROR",
      "READOUT STATE ERROR",
      "SETUP PARITY ERROR",
      "CONTROL PARITY ERROR",
      "JTAG INSTRUCTION ERROR",
      NULL};


  for(int i = 0; i < 11; ++i) {
    if ((status >> i) & 0x1)
      printf("    [%d] %s\n", i, error_bits[i]);
  }
}
