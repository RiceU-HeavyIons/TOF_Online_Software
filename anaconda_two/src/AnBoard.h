/* $Id$ */
/*
 * AnBoard.h
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#ifndef ANBOARD_H_
#define ANBOARD_H_

#include "AnCanObject.h"
#include "AnAgent.h"
class AnBoard;

class AnBoard: public AnCanObject {
 public:

  AnBoard(const AnBoard& brd);
  AnBoard(const AnAddress &laddr, const AnAddress &haddr, AnCanObject *parent = 0);

  double temp(int i = 0) const { return m_temp[i]; }
  virtual QString tempString(int i = 0, bool hilit = false) const;
  virtual double maxTemp() const { return m_temp[0]; };
  virtual QString maxTempString() const;

  virtual int tempAlarm() const { return m_temp_alarm; }
  virtual int setTempAlarm(int ta) { return (m_temp_alarm = ta); }
  virtual QString tempAlarmString() const;

  quint16 ecsr() const { return m_ecsr; }
  virtual QString ecsrString(bool hilight = false) const { Q_UNUSED(hilight); return QString(); }
  virtual QString ecsrToolTipString() const { return QString(); }

  quint32 firmwareId() const { return (m_firmware_mcu | (m_firmware_fpga << 16)); }
  quint16 mcuFirmwareId() const { return m_firmware_mcu; }
  quint16 fpgaFirmwareId() const { return m_firmware_fpga; }
  virtual QString firmwareString() const;

  quint64 chipId() const { return m_chipid; }

  virtual int status() const { return STATUS_UNKNOWN; }

  virtual bool isEast() const = 0;
  virtual bool isWest() const = 0;
  virtual bool hasError() const { return false; }

  virtual void msgr(const QList<quint8>& dary);
  virtual void msgw(const QList<quint8>& dary);

  virtual quint32 canidr() const = 0; // read
  virtual quint32 canidw() const = 0; // write
  virtual quint32 canida() const = 0; // alert
  virtual quint32 cantyp() const { return MSGTYPE_STANDARD; }
  virtual AnAgent* agent() const = 0;

  virtual QString lvHvString() const { return QString(); }
  virtual QString lvString()   const { return QString(); }
  virtual QString hvString()   const { return QString(); }
  virtual int lvOrder() const { return 0; }
  virtual int hvOrder() const { return 0; }

  enum {
    STATUS_UNKNOWN  = 0,
    STATUS_STANDBY  = 1,
    STATUS_ON       = 2,
    STATUS_WARNING  = 3,
    STATUS_ERROR    = 4,
    STATUS_COMM_ERR = 5
  };

 protected:
  quint16 setEcsr(quint16 ecsr) { return (m_ecsr = ecsr); }
  double setTemp(double temp, int i = 0) { return (m_temp[i] = temp); }
  quint32 setFirmwareId(quint32 firmware);
  quint16 setMcuFirmwareId(quint16 firmware) { return (m_firmware_mcu  = firmware); }
  quint8 setFpgaFirmwareId(quint8  firmware) { return (m_firmware_fpga = firmware); }
  double setChipId(quint64 chipid) { return (m_chipid = chipid); }

 private:
  quint16 m_ecsr;
  double  m_temp[2];
  quint16 m_firmware_mcu;
  quint8  m_firmware_fpga;
  quint64 m_chipid;
  int     m_temp_alarm;
};

#endif /* ANBOARD_H_ */
