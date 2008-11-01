/*
 * Tcomp.h
 *
 *  Created on: Oct 31, 2008
 *      Author: koheik
 */

#ifndef TCOMP_H_
#define TCOMP_H_
#include "tof_types.h"
#include "Tcan.h"

class Tcomp {

protected:
  Tcan      *tcan;
  uint8     fid;
  uint16    futid;
  uint8     addr;
  bool      enable;

public:
  Tcomp();
  virtual ~Tcomp();


  virtual Tcan* set_tcan(Tcan *tc) { return (tcan = tc); }
  Tcan *get_tcan() { return tcan; }

  virtual uint8 set_id(uint8 id) { return (fid = id); }
  uint8 id() { return fid; }

  virtual uint16 set_utid(uint16 id) { return (futid = id); }
  uint16 utid() { return futid; }

  virtual uint8 set_addr(uint8 ad) { return (addr = ad); }
  uint8 get_addr() { return addr; }

  bool set_enable(bool en) { return (enable = en); }
  bool get_enable() { return enable; };

  virtual void update_status(int level) =  0;

};

#endif /* TCOMP_H_ */
