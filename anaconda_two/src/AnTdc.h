/*
 * AnTdc.h
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#ifndef ANTDC_H_
#define ANTDC_H_

#include "AnCanObject.h"

class AnTdc: public AnCanObject {
public:
  AnTdc(const AnLAddress &laddr, const AnHAddress &haddr, AnCanObject *parent = 0);

private:
  quint64 m_status;
};

#endif /* ANTDC_H_ */
