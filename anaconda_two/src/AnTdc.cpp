/*
 * AnTdc.cpp
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#include "AnTdc.h"

AnTdc::AnTdc(const AnLAddress &laddr, const AnHAddress &haddr,
    AnCanObject *parent) :
  AnCanObject(laddr, haddr, parent), m_status(0)
{
  setObjectName(QString("TDC ") + lAddress().toString());
}
