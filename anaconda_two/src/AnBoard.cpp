/*
 * AnBoard.cpp
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */
#include <cstdio>
#include "AnBoard.h"

AnBoard::AnBoard(const AnLAddress &laddr, const AnHAddress &haddr,
    AnCanObject *parent) : AnCanObject(laddr, haddr, parent),
    m_ecsr(0), m_temp(0.0), m_firmware(0), m_chipid(0)
{
  // no-op
}

QString AnBoard::firmwareString() const
{
  char buf[32];
  sprintf(buf, "%x%c/%x", 0xff & m_firmware >> 8,
                          0xff & m_firmware,
                          0xff & m_firmware >> 16);
  return QString(buf);
}
