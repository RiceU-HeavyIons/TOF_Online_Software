/*
 * Thub.h
 *
 *  Created on: Oct 31, 2008
 *      Author: koheik
 */

#ifndef THUB_H_
#define THUB_H_

#include "Tcan.h"

class Thub {

  Tcan      *tcan;
  uint8     addr;   /* CANBus address. It's usually 0x40 */

public:
  Thub();
  virtual ~Thub();
};

#endif /* THUB_H_ */
