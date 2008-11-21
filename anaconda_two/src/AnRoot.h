/*
 * AnRoot.h
 *
 *  Created on: Nov 10, 2008
 *      Author: koheik
 */

#ifndef ANROOT_H_
#define ANROOT_H_
#include <QtCore/QList>
#include <QtCore/QMap>

#include "AnCanObject.h"
#include "AnThub.h"
#include "AnTcpu.h"
#include "AnSock.h"

class AnRoot : public AnCanObject {
public:
	AnRoot(AnCanObject *parent = 0);
 	virtual ~AnRoot();

  inline QList<AnCanObject*> list() const { return m_list; }
  inline AnCanObject* at(int i) const { return m_list.at(i); }
  inline int count() const { return m_list.count(); }
  inline AnSock* sock(int i) const { return m_socks[i]; }



private:
  QList<AnCanObject*> m_list;
  QMap<int, AnSock*> m_socks;

};

#endif /* ANROOT_H_ */
