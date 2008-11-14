/*
 * AnThub.h
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#ifndef ANTHUB_H_
#define ANTHUB_H_
#include "AnCanObject.h"

class AnThub : public AnCanObject {
public:
  AnThub(const AnLAddress &laddr, const AnHAddress &haddr, AnCanObject *parent = 0);
  virtual ~AnThub();
};

#endif /* ANTHUB_H_ */
