/*
 * AnTdig.cpp
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#include "AnTdig.h"

AnTdig::AnTdig(const AnLAddress& laddr, const AnHAddress& haddr, AnCanObject *parent)
  : AnBoard(laddr, haddr, parent)
{
  setObjectName(QString("TDIG ") + lAddress().toString());

  AnLAddress lad = lAddress();
  AnHAddress had = hAddress();

  for(int i = 0; i < 3; ++i) {
    lad.set(2, i+1);
    had.set(3, i+1);
    m_tdc[i] = new AnTdc(lad, had, this);
  }
}

AnTdig::~AnTdig()
{
  for(int i = 0; i < 3; ++i)
    delete m_tdc[i];
}
