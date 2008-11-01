/*
 * Tcan.h
 *
 *  Created on: Oct 31, 2008
 *      Author: koheik
 */

#ifndef TCAN_H_
#define TCAN_H_
#include <string>
#include <stdarg.h>
#include "libpcan.h"
#include "tof_types.h"

#define PCAN_DEVICE_PATTERN     "/dev/pcan*"

class Tcan {
  HANDLE        handle;
  uint8         addr;
  std::string   dev_path;

  static int TCAN_DEBUG;

public:
  Tcan();

  virtual ~Tcan();

  static void set_msg(TPCANMsg &msg, ...);

  uint8 set_addr(uint8 ad) { return (addr = ad); }
  uint8 get_addr() { return addr; }

  void write_read() {}

  int open(uint8 dev_id);
  uint64 Tcan::write_read(TPCANMsg &msg, TPCANRdMsg &rmsg,
      unsigned int return_length = 2, unsigned int time_out = 4000000);

private:
  void print(const TPCANMsg &msg);
  void print(const TPCANRdMsg &rmsg);

  void pcan_error() {}
};

#endif /* TCAN_H_ */
