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

AnCanNet::AnCanNet(
    const AnAddress &laddr,
    const AnAddress &haddr,
    AnCanObject *parent)
      : AnCanObject(laddr, haddr, parent)
{
	// do nothing here
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

