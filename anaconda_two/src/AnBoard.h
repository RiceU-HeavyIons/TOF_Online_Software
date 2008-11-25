/*
* AnBoard.h
	*
	*  Created on: Nov 9, 2008
	*      Author: koheik
*/

#ifndef ANBOARD_H_
#define ANBOARD_H_

#include "AnCanObject.h"

class AnBoard;
	
class AnBoard: public AnCanObject {
public:

	AnBoard(const AnBoard& brd);
	AnBoard(const AnAddress &laddr, const AnAddress &haddr, AnCanObject *parent = 0);

	double temp(int i = 0) const { return m_temp[i]; }
	virtual double maxTemp() const { return m_temp[0]; };

	quint8 ecsr() const { return m_ecsr; }
	virtual QString ecsrString() const { return QString(); }

	quint32 firmwareId() const { return (m_firmware_mcu | (m_firmware_fpga << 16)); }
	quint16 mcuFirmwareId() const { return m_firmware_mcu; }
	quint16 fpgaFirmwareId() const { return m_firmware_fpga; }
	virtual QString firmwareString() const;

	quint64 chipId() const { return m_chipid; }

	virtual int status() const { return STATUS_UNKNOWN; }

	virtual bool isEast() const = 0;
	virtual bool isWest() const = 0;
	virtual bool hasError() const { return false; }

	enum {
		STATUS_UNKNOWN = 0,
		STATUS_STANBY  = 1,
		STATUS_ON      = 2,
		STATUS_ERROR   = 3		
	};

protected:
	quint8 setEcsr(quint8 ecsr) { return (m_ecsr = ecsr); }
	double setTemp(double temp, int i = 0) { return (m_temp[i] = temp); }
	quint32 setFirmwareId(quint32 firmware);
	quint16 setMcuFirmwareId(quint16 firmware) { return (m_firmware_mcu  = firmware); }
	quint8 setFpgaFirmwareId(quint8  firmware) { return (m_firmware_fpga = firmware); }
	double setChipId(quint64 chipid) { return (m_chipid = chipid); }

private:
	quint8  m_ecsr;
	double  m_temp[2];
	quint16 m_firmware_mcu;
	quint8  m_firmware_fpga;
	quint64 m_chipid;
};

#endif /* ANBOARD_H_ */
