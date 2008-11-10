/*
 * AnBoard.cpp
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#include "AnBoard.h"

AnBoard::AnBoard(const AnLAddress &laddr, const AnHAddress &haddr,
    AnCanObject *parent) : AnCanObject(laddr, haddr, parent),
    m_ecsr(0), m_temp(0.0), m_firmware(0), m_chipid(0)
{
  // no-op
}
