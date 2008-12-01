/*
 * AnAddress.h
 *
 *  Created on: Nov 23, 2008
 *      Author: koheik
 */

#ifndef ANADDRESS_H_
#define ANADDRESS_H_
#include <QtCore/QObject>
#include <QtCore/QString>

class AnAddress;

class AnAddress : public QObject {
public:
  AnAddress(QObject *parent = 0);
  AnAddress(quint8 w, quint8 x, quint8 y, quint8 z, QObject *parent = 0);
  AnAddress(const QString &rhs);
  AnAddress(const AnAddress &rhs);

  AnAddress& operator=(const AnAddress &rhs);
  quint8 at(const int i) const;

  quint8 set(const int i, const quint8 val) { return (m_addr[i] = val); }
  QString toString() const;

private:
  quint8 m_addr[4];
};

QDebug operator<<(QDebug dbg, const AnAddress &a);

#endif /* ANADDRESS_H_ */
