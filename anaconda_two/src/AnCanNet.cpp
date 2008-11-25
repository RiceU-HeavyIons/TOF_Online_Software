/*
 * AnCanNet.cpp
 *
 *  Created on: Nov 23, 2008
 *      Author: koheik
 */

#include "AnCanNet.h"

AnCanNet::AnCanNet(AnCanObject *parent) : AnCanObject(parent) {
	setObjectName(QString("CanNet ") + laddr().toString());
}

AnCanNet::AnCanNet(const AnAddress &laddr, const AnAddress &haddr,
	AnCanObject *parent) : AnCanObject(laddr, haddr, parent)
{
}