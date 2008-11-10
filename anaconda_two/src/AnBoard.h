/*
 * AnBoard.h
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#ifndef ANBOARD_H_
#define ANBOARD_H_

#include "AnCanObject.h"

class AnBoard: public AnCanObject {
public:
  AnBoard(const AnLAddress &laddr, const AnHAddress &haddr, AnCanObject *parent = 0);

  quint8 ecsr() const { return m_ecsr; }
  double temp() const { return m_temp; }
  quint32 firmwareId() const { return m_firmware; }
  quint64 chipId() const { return m_chipid; }

protected:
  quint8 setEcsr(quint8 ecsr) { return (m_ecsr = ecsr); }
  double setTemp(double temp) { return (m_temp = temp); }
  double setFirmwareId(quint32 firmware) { return (m_firmware = firmware); }
  double setChipId(quint64 chipid) { return (m_chipid = chipid); }

private:
  quint8  m_ecsr;
  double  m_temp;
  quint32 m_firmware;
  quint64 m_chipid;
};

#endif /* ANBOARD_H_ */
