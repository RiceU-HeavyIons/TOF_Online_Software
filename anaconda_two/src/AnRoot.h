/*
 * AnRoot.h
 *
 *  Created on: Nov 10, 2008
 *      Author: koheik
 */

#ifndef ANROOT_H_
#define ANROOT_H_
#include <QtCore/QList>
#include "AnCanObject.h"
#include "AnTcpu.h"
#include "AnSock.h"

class AnRoot : public AnCanObject {
public:
  AnRoot(AnCanObject *parent = 0);
  virtual ~AnRoot();

  inline QList<AnTcpu*> tcpus() const { return m_tcpus; }
  inline AnCanObject* at(int i) const { return m_tcpus.at(i); }
  inline int count() const { return m_tcpus.count(); }

private:
  QList<AnTcpu*> m_tcpus;
  QList<AnSock*> m_socks;

};

#endif /* ANROOT_H_ */
