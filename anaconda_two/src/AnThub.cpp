/*
 * $Id$
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#include "AnRoot.h"
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
		TPCANMsg    msg;
		TPCANRdMsg  rmsg;
		quint64 rdata;

		// readout master firmware id
		AnAgent::set_msg(msg, canidr(), MSGTYPE_STANDARD, 1, 0x01);
		rdata = agent()->write_read(msg, rmsg, 3);
		setMcuFirmwareId(rdata);
		AnAgent::set_msg(msg, canidr(), MSGTYPE_STANDARD, 2, 0x02, 0x00);
		agent()->write_read(msg, rmsg, 3);
		setFpgaFirmwareId(rmsg.Msg.DATA[2]);

		// readout temperature
		for (int i = 0; i < 2; ++i) {
			AnAgent::set_msg(msg, canidr(), MSGTYPE_STANDARD, 2, 0x03, i);
			rdata = agent()->write_read(msg, rmsg, 3);
			setTemp((double)rmsg.Msg.DATA[2] + (double)(rmsg.Msg.DATA[1])/100.0, i);
		}

		// readout crc error bits
		AnAgent::set_msg(msg, canidr(), MSGTYPE_STANDARD, 1, 0x05);
		agent()->write_read(msg, rmsg, 2);
		setEcsr(rmsg.Msg.DATA[1]);

		// readout serdes infomation
		if (--level >= 0)
			for(quint8 i = 0; i < 8; ++i) m_serdes[i]->sync(level);
	}
}

quint32 AnThub::canidr() const
{
	return haddr().at(1) << 4 | 0x4;
}

quint32 AnThub::canidw() const
{
	return haddr().at(1) << 4 | 0x2;
}

AnAgent *AnThub::agent() const
{
	return dynamic_cast<AnRoot*>(parent())->agent(hAddress().at(0));
}

int AnThub::status() const
{
// TO-DO implement real logic
	int err = 0;
	if (maxTemp() > 40.0) err++;

	if (active()) {
		if (err) return STATUS_ERROR;
		return STATUS_ON;
	} else
		return STATUS_UNKNOWN;
}
