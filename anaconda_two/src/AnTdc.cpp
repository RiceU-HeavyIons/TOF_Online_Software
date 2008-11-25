/*
 * AnTdc.cpp
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#include "AnTdc.h"
#include "AnRoot.h"

AnTdc::AnTdc(const AnAddress &laddr, const AnAddress &haddr,
    AnCanObject *parent)
  : AnCanObject(laddr, haddr, parent), m_status(0)
{
  setObjectName(QString("TDC ") + lAddress().toString());
}

void AnTdc::sync(int level)
{
	if (active() and level >= 0) {
		quint8  devid = hAddress().at(0);
		quint8  ctcpu = hAddress().at(1);
		quint32 canid = ((hAddress().at(2) << 4) | 0x2) << 18 | ctcpu;
		quint8  data0 = 0x04 | hAddress().at(3);

	    TPCANMsg    msg;
	    TPCANRdMsg  rmsg;
	    AnSock *sock = dynamic_cast<AnRoot*>(root())->sock(devid);
        AnSock::set_msg(msg, canid, MSGTYPE_EXTENDED, 1, data0);
        m_status = sock->write_read(msg, rmsg, 10);
	}
}

void AnTdc::reset() {

	if (active()) {
		quint8  devid = hAddress().at(0);
		quint8  ctcpu = hAddress().at(1);
		quint32 canid = (((hAddress().at(2) << 4) | 0x2) << 18) | ctcpu;
		quint8  data0 = 0x04 | hAddress().at(3);

	    TPCANMsg    msg;
	    TPCANRdMsg  rmsg;
	    AnSock *sock = dynamic_cast<AnRoot*>(root())->sock(devid);

	    AnSock::set_msg(msg, canid, MSGTYPE_EXTENDED, 6, data0, 0xe4, 0xff, 0xff, 0xff, 0xff);
	    sock->write_read(msg, rmsg, 2);
	    AnSock::set_msg(msg, canid, MSGTYPE_EXTENDED, 6, data0, 0xe4, 0xff, 0xff, 0xff, 0x3f);
	    sock->write_read(msg, rmsg, 2);
	    AnSock::set_msg(msg, canid, MSGTYPE_EXTENDED, 6, data0, 0xf4, 0xff, 0xff, 0xff, 0x1f);
	    sock->write_read(msg, rmsg, 2);
	    AnSock::set_msg(msg, canid, MSGTYPE_EXTENDED, 6, data0, 0xe4, 0xff, 0xff, 0xff, 0x9f);
	    sock->write_read(msg, rmsg, 2);
	}
}

quint64 AnTdc::setStatus(quint64 sw)
{
	m_status = sw;
	return sw;
}

AnTdc::field_t AnTdc::field[26] = {
	{"NOT USERD",                   62,  2},
	{"Parity",                      61,  1},
	{"DLL lock",                    60,  1},
	{"Trigger FIFO Empty",          59,  1},
	{"Trigger FIFO Full",           58,  1},
	{"Trigger FIFO Occupancy",      54,  4},
	{"Group0 L1 Occupancy",         46,  8},
	{"Group1 L1 Occupancy",         38,  8},
	{"Group3 L1 Occupancy",         30,  8},
	{"Group2 L1 Occupancy",         22,  8},
	{"Readout FIFO Empty",          21,  1},
	{"Readout FIFO Full",           20,  1},
	{"Readout FIFO Occupancy",      12,  8},
	{"Have Token",                  11,  1},
	{"Vernier Error",               10,  1},
	{"Course Error",                 9,  1},
	{"Channel Select Error",         8,  1},
	{"L1 Buffer Parity Error",       7,  1},
	{"Trigger FIFO Parity Error",    6,  1},
	{"Trigger Matching State Error", 5,  1},
	{"Readout FIFO Parity Error",    4,  1},
	{"Readout State Error",          3,  1},						
	{"Setup Parity Error",           2,  1},
	{"Control Parity Error",         1,  1},
	{"JTAG Instruction Error",       0,  1},
	{NULL,                           0,  0}
};

QString AnTdc::fkey(int i)  const { return QString(field[i].name); }
quint8 AnTdc::fvalue(int i) const {
	return (m_status >> field[i].start) & (0xFF >> (8 - field[i].length));
}

//------------------------------------------------------------------------------
QString AnTdc::statusTipString() const
{
	QString ret = QString("<h4>Status Word of TDC %1</h4>").arg(haddr().at(3));
	ret += QString("<table>");
	for(int i = 0; i < 25; ++i) {
		field_t f = field[i];
		if(f.length == 1) {
			ret += QString("<tr><td>[%1]</td><td>%2</td><td>= %3</td></tr>")
				.arg(f.start)
				.arg(f.name)
				.arg(fvalue(i));
		} else {
			ret += QString("<tr><td>[%1..%2]</td><td>%3</td><td>= %4</td></tr>")
				.arg(f.start)
				.arg(f.start + f.length - 1)
				.arg(f.name)
				.arg(fvalue(i));
		}	
	}
	ret += "</table>";

	return ret;
}

