/*
 * Tcpu.h
 *
 *  Created on: Oct 31, 2008
 *      Author: koheik
 */

#ifndef TCPU_H_
#define TCPU_H_
#include "Tcomp.h"
#include "Tdig.h"

class Ttray;

class Tcpu : public Tcomp {

  Ttray     *ttray;
  Tdig      *tdig[8];

  // payload=0xb0
  uint32    fw_id;

  // payload=0xb1
  double    temp;
  uint8     ecsr;


public:
  Tcpu();
  virtual ~Tcpu();

  Ttray *set_ttray(Ttray *t) { return(ttray = t); }
  Ttray *get_ttray() { return ttray; }

  Tcan* set_tcan(Tcan *tc);

  Tdig *get_tdig(uint8 i) { return tdig[i]; }

  void update_status(int level = 0);
  std::string firmware_id();
  void dump();
};

#endif /* TCPU_H_ */
