/*
 * $Id$
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#ifndef ANHADDRESS_H_
#define ANHADDRESS_H_
#include <QtCore/QObject>

class AnHAddress;

class AnHAddress : public QObject {
public:
  AnHAddress(QObject *parent = 0);
  AnHAddress(quint8 w, quint8 x, quint8 y, quint8 z, QObject *parent = 0);
  AnHAddress(const AnHAddress &rhs);

  AnHAddress& operator =(const AnHAddress &rhs);
  quint8 at(const int i) const;
  quint8 set(const int i, const quint8 val) { return (m_addr[i] = val); }
  QString toString() const;

private:
  quint8 m_addr[4];
};

#endif /* ANHADDRESS_H_ */
