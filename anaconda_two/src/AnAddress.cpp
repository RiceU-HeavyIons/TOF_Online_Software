/*
 * AnAddress.cpp
 *
 *  Created on: Nov 23, 2008
 *      Author: koheik
 */

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

AnAddress::AnAddress(const AnAddress &rhs) : QObject(rhs.parent())
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
