/*
 * $Id$
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#include "AnCanObject.h"

AnCanObject::AnCanObject(AnCanObject *parent)
  : QObject(parent),
    m_synced(0, 0, 0, 0),
    m_laddr(AnAddress(0, 0, 0, 0)),
    m_haddr(AnAddress(0, 0, 0, 0)),
    m_comm_err(0)
{
	setObjectName(QString("CanObject ") + m_laddr.toString());
	m_active = false;
	m_installed = false;
}

AnCanObject::AnCanObject(const AnCanObject& rhs)
  : QObject(dynamic_cast<AnCanObject*>(rhs.parent())),
    m_active(rhs.m_active),
    m_synced(rhs.m_synced),
    m_laddr(rhs.m_laddr), m_haddr(rhs.m_haddr),
    m_comm_err(rhs.m_comm_err)

{
  // do nothing here
}

AnCanObject::AnCanObject(const AnAddress &laddr, const AnAddress &haddr,
                                                          AnCanObject *parent)
  : QObject(parent), m_active(true), 
    m_synced(0,0,0,0),
    m_laddr(laddr), m_haddr(haddr),
    m_comm_err(0)
{
	  // do nothing here
}

QString AnCanObject::name() const
{
	if (m_name == "")
		return objectName();
	else
		return m_name;
}

QString AnCanObject::setName(const QString& name) {
	return (m_name = name);
}

AnCanObject *AnCanObject::root()
{
	AnCanObject *ptr = this;
	for (ptr = this;
		dynamic_cast<AnCanObject*>(ptr->parent());
		ptr = dynamic_cast<AnCanObject*>(ptr->parent()) ) {}

	return ptr;
}
