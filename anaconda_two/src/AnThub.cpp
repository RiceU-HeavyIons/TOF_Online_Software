/*
 * AnThub.cpp
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#include "AnRoot.h"
#include "AnSock.h"
#include "AnThub.h"

AnThub::AnThub(const AnAddress &laddr, const AnAddress &haddr, AnCanObject *parent)
 : AnBoard(laddr, haddr, parent)
{
	setObjectName(QString("THUB ") + lAddress().toString());

	AnAddress lad = lAddress();
	AnAddress had = hAddress();

	for(int i = 0; i < 8; ++i) {
		lad.set(2, i+1);
		had.set(2, i+1);
		m_serdes[i] = new AnSerdes(lad, had, this);
	}
}

AnThub::~AnThub() {
  // TODO Auto-generated destructor stub
}

void AnThub::sync(int level)
{
	if (active() && level >= 0) {
		quint8 devid = hAddress().at(0);
		quint32 canid = (hAddress().at(1) << 4) | 0x4;

		TPCANMsg    msg;
		TPCANRdMsg  rmsg;
		quint64 rdata;
		AnSock *sock = static_cast<AnRoot*>(parent())->sock(devid);

		// readout master firmware id
		AnSock::set_msg(msg, canid, MSGTYPE_STANDARD, 1, 0x01);
		rdata = sock->write_read(msg, rmsg, 3);
		setMcuFirmwareId(rdata);
		AnSock::set_msg(msg, canid, MSGTYPE_STANDARD, 2, 0x02, 0x00);
		sock->write_read(msg, rmsg, 3);
		setFpgaFirmwareId(rmsg.Msg.DATA[2]);

		// readout temperature
		for (int i = 0; i < 2; ++i) {
			AnSock::set_msg(msg, canid, MSGTYPE_STANDARD, 2, 0x03, i);
			rdata = sock->write_read(msg, rmsg, 3);
			setTemp((double)rmsg.Msg.DATA[2] + (double)(rmsg.Msg.DATA[1])/100.0, i);
		}

		// readout crc error bits
		AnSock::set_msg(msg, canid, MSGTYPE_STANDARD, 1, 0x05);
		sock->write_read(msg, rmsg, 2);
		setEcsr(rmsg.Msg.DATA[1]);

		// readout serdes infomation
		if (--level >= 0)
			for(quint8 i = 0; i < 8; ++i) m_serdes[i]->sync(level);
	}
}