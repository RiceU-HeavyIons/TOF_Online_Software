/*
 * Ttray.cpp
 *
 *  Created on: Oct 31, 2008
 *      Author: koheik
 */

#include "Ttray.h"

Ttray::Ttray() {

  tcpu = new Tcpu();
  tcpu->set_ttray(this);
  tcpu->set_id(1);

}

Ttray::~Ttray() {

  delete tcpu;
}

void Ttray::set_tcan(Tcan *tc) {
  tcan = tc;
  tcpu->set_tcan(tcan);
}
