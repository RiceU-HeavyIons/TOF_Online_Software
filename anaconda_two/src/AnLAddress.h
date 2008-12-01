/*
 * $Id$
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#ifndef ANLADDRESS_H_
#define ANLADDRESS_H_
#include <QtCore/QObject>

class AnLAddress;

class AnLAddress : public QObject {
public:
  AnLAddress(QObject *parent = 0);
  AnLAddress(const quint8 x, const quint8 y, const quint8 z, QObject *parent = 0);
  AnLAddress(const AnLAddress &rhs, QObject *parent = 0);

  AnLAddress& operator =(const AnLAddress &rhs);

  quint8 at(const int i) const;
  quint8 set(const int i, const quint8 val);
  QString toString() const;

private:
  quint8 m_x;
  quint8 m_y;
  quint8 m_z;
};

#endif /* ANLADDRESS_H_ */
