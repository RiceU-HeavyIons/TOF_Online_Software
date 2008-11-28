/*
 * Config.cpp
 *
 *  Created on: Nov 7, 2008
 *      Author: koheik
 */

#include "Config.h"

Config::Config(int i, int ms) : QThread() {
	m_id = i;
	m_sleep = ms;
}

void Config::run() {
  for(int i = 0; i <= 100; ++i) {
    emit step(m_id, i);
    msleep(m_sleep);
  }
}
