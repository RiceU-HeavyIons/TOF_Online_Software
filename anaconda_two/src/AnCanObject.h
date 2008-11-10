/*
 * AnCanObject.h
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#ifndef ANCANOBJECT_H_
#define ANCANOBJECT_H_
#include <QtCore/QObject>
#include "AnLAddress.h"
#include "AnHAddress.h"

class AnCanObject;

class AnCanObject : public QObject {
public:
  AnCanObject(AnCanObject *parent = 0);
  AnCanObject(const AnLAddress &laddr, const AnHAddress &haddr, AnCanObject *parent = 0);

  AnHAddress hAddress() const;
  AnLAddress lAddress() const;

  virtual QString name() const;
  virtual void sync(int level = 0) { ++level; };
  virtual void dump() const { };

protected:
  bool  enable;

private:
  const AnLAddress m_laddr;
  const AnHAddress m_haddr;

};

#endif /* ANCANOBJECT_H_ */
