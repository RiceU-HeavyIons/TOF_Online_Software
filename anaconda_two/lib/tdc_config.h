/*
 * tdc_config.h
 *
 *  Created on: Oct 30, 2008
 *
 *      Author: koheik
 */

#ifndef TDC_CONFIG_H_
#define TDC_CONFIG_H_

#include <cstdio>
#include "libpcan.h"
#include "pcan.h"

class tdc_config {

private:
  unsigned int check_sum;
  unsigned int length;
  unsigned int block_length;
  unsigned char data[256];

public:
  tdc_config();
  virtual ~tdc_config();

  unsigned char load_from_file(const char *path);

  unsigned int get_length() { return length; }
  unsigned int get_check_sum() { return check_sum; }

  unsigned int get_block_length() { return block_length; }
  void set_block_msg(TPCANMsg *msg, int line);

private:
  unsigned char bits_to_num(unsigned int bs);

};

#endif /* TDC_CONFIG_H_ */
