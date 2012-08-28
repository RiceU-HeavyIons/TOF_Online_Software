/*
 * $Id$
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */
#include <cstdio>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
//#include "locallibpcan.h"
//using namespace PCAN;
#include "AnBoard.h"

AnBoard::AnBoard(const AnAddress &laddr, const AnAddress &haddr,
    AnCanObject *parent) : AnCanObject(laddr, haddr, parent),
    m_ecsr(0), m_firmware_mcu(0), m_firmware_fpga(0), m_chipid(0)
{
	m_temp[0] = 0.0;
	m_temp[1] = 0.0;
	m_temp_alarm = 0;
}

AnBoard::AnBoard(const AnBoard &rhs) : AnCanObject(rhs),
	m_ecsr(rhs.ecsr()),
	m_firmware_mcu(rhs.mcuFirmwareId()),
	m_firmware_fpga(rhs.fpgaFirmwareId()),
	m_chipid(rhs.chipId())
{
}

QString AnBoard::tempString(int i, bool hilit) const
{
	QString ret = QString::number(m_temp[i], 'f', 2) + " C";

	if (hilit && (temp(i) > tempAlarm()))
		ret = QString("<font color='red'>%1</font>").arg(ret);

	return ret;
}

QString AnBoard::maxTempString() const
{
	if (active())
		return QString::number(maxTemp(), 'f', 2);
	else
		return QString();
}

QString AnBoard::tempAlarmString() const
{
	return QString::number(m_temp_alarm) + " C";
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

void AnBoard::msgr(const QList<quint8>& dary)
{
  if (active()) {
    struct can_frame msg;
    struct can_frame rmsg;
    
    msg.can_id      = canidr() | cantyp();
    //msg.MSGTYPE = cantyp();
    msg.can_dlc     = dary.count();
    for(int i = 0; i < msg.can_dlc; ++i) msg.data[i] = dary[i];
    agent()->write_read(msg, rmsg, 2);
  }
}

void AnBoard::msgw(const QList<quint8>& dary)
{
  if (active()) {
    struct can_frame msg;
    struct can_frame rmsg;
    
    msg.can_id      = canidw() | cantyp();
    //msg.MSGTYPE = cantyp();
    msg.can_dlc     = dary.count();
    for(int i = 0; i < msg.can_dlc; ++i) msg.data[i] = dary[i];
    agent()->write_read(msg, rmsg, 2);
  }
}
