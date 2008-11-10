/*
 * AnCanObject.cpp
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#include "AnCanObject.h"

AnCanObject::AnCanObject(AnCanObject *parent) : QObject(parent),
 m_laddr(AnLAddress(0, 0, 0)),
 m_haddr(AnHAddress(0, 0, 0, 0)),
 enable(false)
{
  setObjectName(QString("CanObject ") + m_laddr.toString());
}

AnCanObject::AnCanObject(
    const AnLAddress &laddr, const AnHAddress &haddr, AnCanObject *parent)
  : QObject(parent), m_laddr(laddr), m_haddr(haddr), enable(true)
{
}

AnHAddress AnCanObject::hAddress() const
{
  return AnHAddress(m_haddr);
}


AnLAddress AnCanObject::lAddress() const
{
  return AnLAddress(m_laddr);
}

QString AnCanObject::name() const
{
  return objectName();
}
