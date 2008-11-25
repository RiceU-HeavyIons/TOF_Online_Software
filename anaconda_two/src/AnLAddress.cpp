/*
 * AnLAddress.cpp
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#include <QtCore/QStringList>
#include "AnLAddress.h"

AnLAddress::AnLAddress(QObject *parent) : m_x(0), m_y(0), m_z(0), QObject(parent)
{

}

AnLAddress::AnLAddress(const quint8 x, const quint8 y, const quint8 z, QObject *parent) : QObject(parent)
{
  m_x = x;
  m_y = y;
  m_z = z;
}

AnLAddress::AnLAddress(const AnLAddress &rhs, QObject *parent) : QObject(parent)
{
  m_x = rhs.m_x;
  m_y = rhs.m_y;
  m_z = rhs.m_z;
}

AnLAddress& AnLAddress::operator =(const AnLAddress &rhs)
{
  m_x = rhs.m_x;
  m_y = rhs.m_y;
  m_z = rhs.m_z;

  return *this;
}

quint8 AnLAddress::at(const int i) const
{
  switch(i) {
  case 0:   return m_x;
  case 1:   return m_y;
  case 2:   return m_z;
  default:
            return 0;
  }
}

quint8 AnLAddress::set(const int i, const quint8 val)
{
  switch(i) {
  case 0:   return (m_x = val);
  case 1:   return (m_y = val);
  case 2:   return (m_z = val);
  default:
            return 0;
  }
}

QString AnLAddress::toString() const
{
  QStringList list;
  list << QString::number(m_x);
  list << QString::number(m_y);
  list << QString::number(m_z);
  return list.join(".");
}
