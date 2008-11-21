/*
 * Config.cpp
 *
 *  Created on: Nov 7, 2008
 *      Author: koheik
 */

#include "Config.h"

Config::Config(int ms) : QThread() {
  m_sleep = ms;
}

void Config::run() {
  for(int i = 0; i <= 100; ++i) {
    emit step(i);
    msleep(m_sleep);
  }
}
