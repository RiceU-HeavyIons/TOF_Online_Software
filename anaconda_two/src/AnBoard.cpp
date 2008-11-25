/*
 * AnBoard.cpp
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */
#include <cstdio>
#include "AnBoard.h"

AnBoard::AnBoard(const AnAddress &laddr, const AnAddress &haddr,
    AnCanObject *parent) : AnCanObject(laddr, haddr, parent),
    m_ecsr(0), m_firmware_mcu(0), m_firmware_fpga(0), m_chipid(0)
{
	m_temp[0] = 0.0;
	m_temp[1] = 0.0;
}

AnBoard::AnBoard(const AnBoard &rhs) : AnCanObject(rhs),
	m_ecsr(rhs.ecsr()),
	m_firmware_mcu(rhs.mcuFirmwareId()),
	m_firmware_fpga(rhs.fpgaFirmwareId()),
	m_chipid(rhs.chipId())
{
}

QString AnBoard::firmwareString() const
{
  char buf[32];
  sprintf(buf, "%x%c/%x", 0xff & m_firmware_mcu >> 8,
                          0xff & m_firmware_mcu,
                          m_firmware_fpga);
  return QString(buf);
}

quint32 AnBoard::setFirmwareId(quint32 firmware)
{
	m_firmware_mcu  = 0xffff & firmware;
	m_firmware_fpga = 0xff & (firmware >> 16);
	return firmware;
} 
