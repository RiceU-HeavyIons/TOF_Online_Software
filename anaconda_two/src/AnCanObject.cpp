/*
 * AnCanObject.cpp
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#include "AnCanObject.h"

AnCanObject::AnCanObject(AnCanObject *parent) : QObject(parent),
 m_laddr(AnAddress(0, 0, 0, 0)),
 m_haddr(AnAddress(0, 0, 0, 0)),
 m_active(false)
{
  setObjectName(QString("CanObject ") + m_laddr.toString());
}

AnCanObject::AnCanObject(const AnCanObject& rhs) : QObject(dynamic_cast<AnCanObject*>(rhs.parent())),
 m_laddr(rhs.m_laddr), m_haddr(rhs.m_haddr), m_active(rhs.m_active)
{

}

AnCanObject::AnCanObject(
    const AnAddress &laddr, const AnAddress &haddr, AnCanObject *parent)
  : QObject(parent), m_laddr(laddr), m_haddr(haddr), m_active(true)
{
}

QString AnCanObject::name() const
{
  return objectName();
}

AnCanObject *AnCanObject::root()
{
	AnCanObject *ptr = this;
	for (ptr = this;
		dynamic_cast<AnCanObject*>(ptr->parent());
		ptr = dynamic_cast<AnCanObject*>(ptr->parent()) ) {}

	return ptr;
}
