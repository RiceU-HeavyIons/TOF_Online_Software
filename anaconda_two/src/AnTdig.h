/*
 * AnTdig.h
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#ifndef ANTDIG_H_
#define ANTDIG_H_

#include "AnBoard.h"
#include "AnTdc.h"

class AnTdig: public AnBoard {
public:
  AnTdig(const AnLAddress& ladrr, const AnHAddress& laddr, AnCanObject *parent = 0);
  virtual ~AnTdig();

  AnTdc *tdc(int i) const { return m_tdc[i-1]; }

private:
  AnTdc *m_tdc[3];
};

#endif /* ANTDIG_H_ */
