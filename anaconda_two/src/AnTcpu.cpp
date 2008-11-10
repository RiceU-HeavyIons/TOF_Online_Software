/*
 * AnTcpu.cpp
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#include "AnTcpu.h"

AnTcpu::AnTcpu(const AnLAddress &laddr, const AnHAddress &haddr,
    AnCanObject *parent) :
  AnBoard(laddr, haddr, parent)
{
  setObjectName(QString("TCPU ") + lAddress().toString());

  AnLAddress lad = lAddress();
  AnHAddress had = hAddress();

  for(int i = 0; i < 8; ++i) {
    lad.set(1, i+1);
    had.set(2, 0x10+i);
    m_tdig[i] = new AnTdig(lad, had, this);
  }
}

AnTcpu::~AnTcpu()
{
  for(int i = 0; i < 8; ++i)
    delete m_tdig[i];
}
