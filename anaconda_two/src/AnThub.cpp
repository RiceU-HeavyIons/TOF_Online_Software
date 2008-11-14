/*
 * AnThub.cpp
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#include "AnThub.h"

AnThub::AnThub(const AnLAddress &laddr, const AnHAddress &haddr, AnCanObject *parent)
 : AnCanObject(laddr, haddr, parent)
{
}

AnThub::~AnThub() {
  // TODO Auto-generated destructor stub
}
