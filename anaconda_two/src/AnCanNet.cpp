/*
 * $Id$
 *
 *  Created on: Nov 23, 2008
 *      Author: koheik
 */

#include "AnCanNet.h"

AnCanNet::AnCanNet(AnCanObject *parent) : AnCanObject(parent)
{
	setObjectName(QString("CanNet ") + laddr().toString());
}

AnCanNet::AnCanNet(const AnCanNet &rhs)
  : AnCanObject(rhs.laddr(), rhs.haddr(), dynamic_cast<AnCanObject*>(rhs.parent()))
{
	setObjectName( rhs.objectName() );
	m_map = rhs.m_map;
}

AnCanNet::AnCanNet(
    const AnAddress &laddr,
    const AnAddress &haddr,
    AnCanObject *parent)
      : AnCanObject(laddr, haddr, parent)
{
	// do nothing here
}

AnCanNet& AnCanNet::operator=(const AnCanNet& rhs)
{
	setActive(rhs.active());
	setSynced(rhs.synced());
	laddr() = rhs.laddr();
	haddr() = rhs.haddr();
	setObjectName( rhs.objectName() );
}

AnCanObject *AnCanNet::hat(int i)
{
	foreach(AnBoard *brd, m_map) {
		if (brd->haddr().at(1) == i) return brd;
	}
	return NULL;
}

/**
 * Dump object
 */
QString AnCanNet::dump() const
{
	QStringList sl;

	sl << QString().sprintf("AnCanNet(%p):", this);
	sl << QString("  Name             : ") + name();
	sl << QString("  Hardware Address : ") + haddr().toString().toStdString().c_str();
	sl << QString("  Logical Address  : ") + laddr().toString().toStdString().c_str();
	sl << QString("  Active           : ") + (active() ? "yes" : "no");
//	sl << QString("  Status           : ") + QString::number(status());
//	sl << QString("  East / West      : ") + (isEast()? "East" : "West");

	return sl.join("\n");
}

QDebug operator<<(QDebug dbg, const AnCanNet &a)
{
	char buf[64];
	sprintf(buf, "AnCanNet(%p)", &a);
	dbg << buf;
	return dbg;
}
