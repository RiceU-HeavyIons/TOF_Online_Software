/*
 * AnTcpu.h
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#ifndef ANTCPU_H_
#define ANTCPU_H_

#include "AnBoard.h"
#include "AnTdig.h"

class AnTcpu: public AnBoard {
public:
  AnTcpu(const AnLAddress &laddr, const AnHAddress &haddr, AnCanObject *parent = 0);
  virtual ~AnTcpu();

  AnTdig *tdig(int i) const { return m_tdig[i-1]; }

  virtual void sync(int level = 0);
  virtual void dump() const;
  virtual QString ecsrString() const;
private:
  AnTdig *m_tdig[8];
};

#endif /* ANTCPU_H_ */
