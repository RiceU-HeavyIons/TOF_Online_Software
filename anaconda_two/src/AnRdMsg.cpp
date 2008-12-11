/*
 * $Id$
 *
 *  Created on: Dec 10, 2008
 *      Author: koheik
 */

#include "AnRdMsg.h"

AnRdMsg::AnRdMsg()
{
	// c-tor
}

AnRdMsg::AnRdMsg(const quint8 devid, const PCAN::TPCANRdMsg &rmsg)
{
	m_devid = devid;
	m_id    = rmsg.Msg.ID;
	m_type  = rmsg.Msg.MSGTYPE;
	m_len   = rmsg.Msg.LEN;
	for (int i = 0; i < m_len; ++i) m_data[i] = rmsg.Msg.DATA[i];
}

AnRdMsg::AnRdMsg(const quint8 devid, const PCAN::TPCANMsg &msg)
{
	m_devid = devid;
	m_id    = msg.ID;
	m_type  = msg.MSGTYPE;
	m_len   = msg.LEN;
	for (int i = 0; i < m_len; ++i) m_data[i] = msg.DATA[i];
}

AnRdMsg::~AnRdMsg()
{
	// d-tor
}

QDebug operator<<(QDebug dbg, const AnRdMsg &m)
{
	char buf1[64];
	char buf2[32];
	sprintf(buf1, "AnRdMsg(DEVID=0x%x ID=0x%x TYPE=0x%x LEN=%d DATA=[",
		m.devid(), m.id(), m.type(), m.len());
	for(int i = 0; i < m.len(); ++i) {
		if (i == 0)
			sprintf(buf2, "0x%02x", m.data(i));
		else
			sprintf(buf2, " 0x%02x", m.data(i));
		strcat(buf1, buf2);
	}
	strcat(buf1, "])");
	dbg << buf1;
	return dbg;
}
