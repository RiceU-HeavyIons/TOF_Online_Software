/*
 * AnRoot.cpp
 *
 *  Created on: Nov 10, 2008
 *      Author: koheik
 */

#include "AnRoot.h"

AnRoot::AnRoot(AnCanObject *parent) : AnCanObject (parent) {

  this->setObjectName("ROOT");
  m_list << new AnThub(AnLAddress(30, 0, 0), AnHAddress(253, 0x40, 0, 0), this);

  for(int i = 0; i < 30; i++) {
    AnTcpu *tcpu = new AnTcpu(AnLAddress(i+1, 0, 0),
                              AnHAddress(253, 0x20, 0, 0),
                              this);
    m_list << tcpu;
  }
  AnSock* sock = new AnSock();
  sock->open(253);
  m_socks << sock;
}

AnRoot::~AnRoot() {
  // TODO Auto-generated destructor stub
}
