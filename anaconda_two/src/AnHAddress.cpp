/*
 * AnHAddress.cpp
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#include <QtCore/QStringList>
#include "AnHAddress.h"

AnHAddress::AnHAddress(QObject *parent) : QObject(parent)
{
  for(int i = 0; i < 3; ++i) m_addr[i] = 0;
}

AnHAddress::AnHAddress(quint8 w, quint8 x, quint8 y, quint8 z, QObject *parent) :
  QObject(parent)
{
  m_addr[0] = w;
  m_addr[1] = x;
  m_addr[2] = y;
  m_addr[3] = z;
}

AnHAddress::AnHAddress(const AnHAddress &lh) : QObject(lh.parent())
{
  for(int i = 0; i < 4; ++i)
    m_addr[i] = lh.m_addr[i];
}

AnHAddress& AnHAddress::operator =(const AnHAddress &lh) {
  for(int i = 0; i < 4; ++i)
    m_addr[i] = lh.m_addr[i];
  return *this;
}

quint8 AnHAddress::at(const int i) const
{
  return m_addr[i];
}

QString AnHAddress::toString() const
{
  QStringList list;
  for(int i = 0; i < 4; ++i)
    list << QString::number(m_addr[i]);
  return list.join(".");
}
