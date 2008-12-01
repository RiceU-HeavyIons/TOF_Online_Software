/*
 * $Id$
 *
 *  Created on: Nov 23, 2008
 *      Author: koheik
 */
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include "AnAddress.h"

AnAddress::AnAddress(QObject *parent) : QObject(parent)
{
  for(int i = 0; i < 3; ++i) m_addr[i] = 0;
}

AnAddress::AnAddress(quint8 w, quint8 x, quint8 y, quint8 z, QObject *parent) :
  QObject(parent)
{
  m_addr[0] = w;
  m_addr[1] = x;
  m_addr[2] = y;
  m_addr[3] = z;
}

AnAddress::AnAddress(const QString &rhs) : QObject(0)
{
	QStringList sl = rhs.split(".");
	for (int i = 0; i < 4 && i < sl.size(); ++i) {
		if (sl[i] == "*")
			m_addr[i] = 255;
		else
		 m_addr[i] = sl[i].toInt();
	}
	for (int i = sl.size(); i < 4; ++i) m_addr[i] = 0;
}

AnAddress::AnAddress(const AnAddress &rhs) : QObject(0)
{
  for(int i = 0; i < 4; ++i)
    m_addr[i] = rhs.m_addr[i];
}

AnAddress& AnAddress::operator =(const AnAddress &rhs) {
  for(int i = 0; i < 4; ++i)
    m_addr[i] = rhs.m_addr[i];
  return *this;
}

quint8 AnAddress::at(const int i) const
{
  return m_addr[i];
}

QString AnAddress::toString() const
{
  QStringList list;
  for(int i = 0; i < 4; ++i)
    list << QString::number(m_addr[i]);
  return list.join(".");
}

QDebug operator<<(QDebug dbg, const AnAddress &a)
{
	char buf[32];
	sprintf(buf, "AnAddress(%d.%d.%d.%d)", a.at(0), a.at(1), a.at(2), a.at(3));
	dbg << buf;
	return dbg;
}
